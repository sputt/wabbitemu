using System;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.TextEditor.Gui.InsightWindow;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    public sealed class WabbitcodeTextEditor : TextEditorControl
    {
        private readonly Bitmap _warningBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.Warning16.png"));
        private readonly Bitmap _errorBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.error.png"));
        private InsightWindow _insightWindow;
        private CodeCompletionWindow _codeCompletionWindow;
        private bool _inHandleKeyPress;
        private bool _isUpdatingRefs;
        private static readonly Regex LineRegex = new Regex(@"^\s*(?<line>[\w|\s|,|\(|\)|:|\*|/|\+|\-|\$|\%|'|\\|\<|\>]*?)\s*(;.*)?$", RegexOptions.Compiled);
        private readonly Timer _textChangedTimer = new Timer
        {
            Interval = 5000,
            Enabled = false
        };

        #region Services

        private readonly IBackgroundAssemblerService _backgroundAssemblerService = ServiceFactory.Instance.GetServiceInstance<IBackgroundAssemblerService>();
        private readonly IDebuggerService _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        private readonly IParserService _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
        private readonly ISymbolService _symbolService = ServiceFactory.Instance.GetServiceInstance<ISymbolService>();
        private readonly IStatusBarService _statusBarService = ServiceFactory.Instance.GetServiceInstance<IStatusBarService>();

        #endregion

        public WabbitcodeTextEditor()
        {
            Font = Settings.Default.EditorFont;
            IsIconBarVisible = Settings.Default.IconBar;
            IsReadOnly = false;
            LineViewerStyle = Settings.Default.LineEnabled ?
                LineViewerStyle.FullRow : LineViewerStyle.None;
            ShowLineNumbers = Settings.Default.LineNumbers;
            ShowVRuler = false;
            TextEditorProperties.MouseWheelScrollDown = !Settings.Default.InverseScrolling;
            TextRenderingHint = Settings.Default.AntiAlias ?
                TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
            Document.FormattingStrategy = new AsmFormattingStrategy();
            editactions.Add(Keys.Control | Keys.Space, new ShowCodeCompletion());
            SetHighlighting("Z80 Assembly");

            TextChanged += WabbitcodeTextEditor_TextChanged;
            Document.BreakpointManager.Added += BreakpointManager_Added;
            Document.BreakpointManager.Removed += BreakpointManager_Removed;
            Document.BreakpointManager.HighlightRegex = LineRegex;

            _textChangedTimer.Tick += textChangedTimer_Tick;
            _backgroundAssemblerService.OnBackgroundAssemblerComplete += UpdateIcons;
            Settings.Default.SettingChanging += Default_SettingChanging;
        }

        protected override void InitializeTextAreaControl(TextAreaControl newControl)
        {
            base.InitializeTextAreaControl(newControl);

            newControl.TextArea.KeyEventHandler += TextArea_KeyEventHandler;
            newControl.MouseWheel += TextArea_MouseWheel;
            newControl.TextArea.Caret.PositionChanged += Caret_PositionChanged;
            newControl.TextArea.AllowDrop = true;
            newControl.TextArea.SelectionManager.SelectionChanged += SelectionManager_SelectionChanged;
            newControl.TextArea.ToolTipRequest += TextArea_ToolTipRequest;
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);

            _textChangedTimer.Enabled = false;
            _errorBitmap.Dispose();
            _warningBitmap.Dispose();
        }

        #region Events

        private void WabbitcodeTextEditor_TextChanged(object sender, EventArgs e)
        {
            if (_textChangedTimer.Enabled)
            {
                _textChangedTimer.Stop();
            }

            _textChangedTimer.Start();
            if (Settings.Default.EnableFolding)
            {
                Document.FoldingManager.FoldingStrategy = new RegionFoldingStrategy();
                Document.FoldingManager.UpdateFoldings(null, null);
            }
            else
            {
                Document.FoldingManager.FoldingStrategy = null;
                Document.FoldingManager.UpdateFoldings(new List<FoldMarker>());
            }
        }

        private void TextArea_MouseWheel(object sender, MouseEventArgs e)
        {
            TextAreaControl textAreaControl = (TextAreaControl)sender;
            if (_insightWindow != null && !_insightWindow.IsDisposed && _insightWindow.Visible)
            {
                _insightWindow.HandleMouseWheel(e);
                textAreaControl.DoHandleMousewheel = false;
            }
            else if (_codeCompletionWindow != null && !_codeCompletionWindow.IsDisposed && _codeCompletionWindow.Visible)
            {
                _codeCompletionWindow.HandleMouseWheel(e);
                textAreaControl.DoHandleMousewheel = false;
            }
            else
            {
                textAreaControl.HandleMouseWheel(e);
            }
        }

        private bool TextArea_KeyEventHandler(char ch)
        {
            if (_inHandleKeyPress)
            {
                return false;
            }

            _inHandleKeyPress = true;
            try
            {
                if (_codeCompletionWindow != null && !_codeCompletionWindow.IsDisposed)
                {
                    if (_codeCompletionWindow.ProcessKeyEvent(ch))
                    {
                        return true;
                    }
                    if (_codeCompletionWindow != null && !_codeCompletionWindow.IsDisposed)
                    {
                        // code-completion window is still opened but did not want to handle
                        // the keypress -> don't try to restart code-completion
                        return false;
                    }
                }

                if (Settings.Default.EnableAutoComplete)
                {
                    string extension = string.IsNullOrEmpty(FileName) ? ".asm" : Path.GetExtension(FileName);
                    if (CodeCompletionFactory.GetBindingForExtension(extension)
                        .Any(ccBinding => ccBinding.HandleKeyPress(this, ch)))
                    {
                        return false;
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.ToString());
            }
            finally
            {
                _inHandleKeyPress = false;
            }

            return false;
        }

        private void BreakpointManager_Removed(object sender, BreakpointEventArgs e)
        {
            WabbitcodeBreakpointManager.RemoveBreakpoint(FileName, e.Breakpoint.LineNumber);
        }

        private void BreakpointManager_Added(object sender, BreakpointEventArgs e)
        {
            WabbitcodeBreakpointManager.AddBreakpoint(FileName, e.Breakpoint.LineNumber);
        }

        private void TextArea_ToolTipRequest(object sender, ToolTipRequestEventArgs e)
        {
            if (!e.InDocument)
            {
                return;
            }

            var segment = Document.GetLineSegment(e.LogicalPosition.Line);
            var word = segment.GetWord(e.LogicalPosition.Column);
            if (word == null || string.IsNullOrEmpty(word.Word))
            {
                return;
            }

            string text = word.Word;
            string tooltip;
            try
            {
                IParserData data = _parserService.GetParserData(text, Settings.Default.CaseSensitive).FirstOrDefault();
                tooltip = data == null ? _symbolService.SymbolTable.GetAddressFromLabel(text) : data.Description;
            }
            catch (Exception)
            {
                return;
            }

            if (string.IsNullOrEmpty(tooltip))
            {
                if (_debuggerService.CurrentDebugger == null)
                {
                    return;
                }

                ushort? regValue = _debuggerService.CurrentDebugger.GetRegisterValue(text);
                if (!regValue.HasValue)
                {
                    return;
                }

                tooltip = "$" + regValue.Value.ToString("X");
            }

            e.ShowToolTip(tooltip);
        }

        private void SelectionManager_SelectionChanged(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(Text))
            {
                return;
            }

            UpdateCodeCountInfo();
        }

        private void Caret_PositionChanged(object sender, EventArgs e)
        {
            if (Document.TextLength == 0)
            {
                return;
            }

            Document.MarkerStrategy.RemoveAll(marker => marker != null && marker.Tag == "Reference");
            Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
            if (!_isUpdatingRefs)
            {
                _isUpdatingRefs = true;
                string word = GetWordAtCaret();
                var segments = Document.LineSegmentCollection;
                Task.Factory.StartNew(() => GetHighlightReferences(word, segments));
            }

            _statusBarService.SetCaretPosition(ActiveTextAreaControl.Caret.Line, ActiveTextAreaControl.Caret.Column);
            UpdateCodeCountInfo();

            CalcLocation label = _symbolService.ListTable.GetCalcLocation(FileName, ActiveTextAreaControl.Caret.Line);
            if (label == null)
            {
                return;
            }

            string assembledInfo = string.Format("Page: {0} Address: {1}", label.Page, label.Address.ToString("X4"));
            _statusBarService.SetText(assembledInfo);
        }

        private void textChangedTimer_Tick(object sender, EventArgs e)
        {
            string text = Text;
            Task.Factory.StartNew(() => _parserService.ParseFile(text.GetHashCode(), FileName, text));
            _backgroundAssemblerService.RequestAssemble();
            _textChangedTimer.Enabled = false;
        }

        #endregion

        #region Code Completion

        public void ShowInsightWindow(IInsightDataProvider insightDataProvider)
        {
            if (_insightWindow == null || _insightWindow.IsDisposed)
            {
                _insightWindow = new InsightWindow(ParentForm, this);
                _insightWindow.Closed += CloseInsightWindow;
            }
            _insightWindow.AddInsightDataProvider(insightDataProvider, FileName);
            _insightWindow.ShowInsightWindow();
        }

        private void CloseInsightWindow(object sender, EventArgs e)
        {
            if (_insightWindow == null)
            {
                return;
            }

            _insightWindow.Closed -= CloseInsightWindow;
            _insightWindow.Dispose();
            _insightWindow = null;
        }

        public void ShowCompletionWindow(ICompletionDataProvider completionDataProvider, char ch)
        {
            _codeCompletionWindow = CodeCompletionWindow.ShowCompletionWindow(
                ParentForm, this, FileName, completionDataProvider, ch);
            if (_codeCompletionWindow != null)
            {
                _codeCompletionWindow.Closed += CloseCodeCompletionWindow;
            }
        }

        private void CloseCodeCompletionWindow(object sender, EventArgs e)
        {
            if (_codeCompletionWindow == null)
            {
                return;
            }

            _codeCompletionWindow.Closed -= CloseCodeCompletionWindow;
            _codeCompletionWindow.Dispose();
            _codeCompletionWindow = null;
        }

        #endregion

        public string GetWordAtCaret()
        {
            var caret = ActiveTextAreaControl.Caret;
            var segment = Document.GetLineSegment(caret.Line);
            var word = segment.GetWord(caret.Column);
            return word == null ? string.Empty : word.Word;
        }

        public string GetWordBeforeCaret()
        {
            var caret = ActiveTextAreaControl.Caret;
            var segment = Document.GetLineSegment(caret.Line);
            int col = caret.Column;
            while (col >= 0)
            {
                var word = segment.GetWord(col);
                if (word != null && !word.IsWhiteSpace)
                {
                    return word.Word;
                }

                col--;
            }

            return string.Empty;
        }

        private void UpdateCodeCountInfo()
        {
            var codeInfoLines = GetWholeLinesSelected();
            _backgroundAssemblerService.CountCode(codeInfoLines);
        }

        public string GetWholeLinesSelected()
        {
            int startLine;
            int endLine;
            var selection = ActiveTextAreaControl.SelectionManager.SelectionCollection.FirstOrDefault();
            if (selection != null)
            {
                endLine = Document.GetLineNumberForOffset(selection.EndOffset);
                startLine = Document.GetLineNumberForOffset(selection.Offset);
                if (startLine > endLine)
                {
                    int temp = endLine;
                    endLine = startLine;
                    startLine = temp;
                }
            }
            else
            {
                endLine = ActiveTextAreaControl.Caret.Line;
                startLine = endLine;
            }

            string codeInfoLines = string.Empty;
            for (; startLine <= endLine; startLine++)
            {
                codeInfoLines += GetLineText(startLine) + Environment.NewLine;
            }
            return codeInfoLines;
        }

        private void GetHighlightReferences(string word, IEnumerable<LineSegment> segmentCollection)
        {
            var options = Settings.Default.CaseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
            if (string.IsNullOrEmpty(word) || !Settings.Default.ReferencesHighlighter)
            {
                _isUpdatingRefs = false;
                return;
            }

            var references = from segment in segmentCollection
                             from segmentWord in segment.Words
                             where string.Equals(segmentWord.Word, word, options)
                             select new ReferenceMarker(segment.Offset + segmentWord.Offset, segmentWord.Length);
            if (InvokeRequired && !IsDisposed)
            {
                this.Invoke(() => AddMarkers(references));
            }
            _isUpdatingRefs = false;
        }

        private void AddMarkers(IEnumerable<TextMarker> markers)
        {
            foreach (TextMarker marker in markers)
            {
                Document.MarkerStrategy.AddMarker(marker);
            }

            Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
            Refresh();
        }

        private void UpdateIcons(object sender, AssemblyFinishEventArgs e)
        {
            IconManager iconManager = Document.IconManager;
            iconManager.ClearIcons();
            var errorWarnings = e.Output.ParsedErrors.Where(issue => FileOperations.CompareFilePath(issue.File, FileName));
            foreach (MarginIcon marginIcon in errorWarnings.Select(issue =>
                new MarginIcon(issue.IsWarning ? _warningBitmap : _errorBitmap, issue.LineNumber - 1,
                    new ToolTip { ToolTipTitle = issue.Description })))
            {
                iconManager.AddIcon(marginIcon);
            }
        }

        public string GetLineText(int lineNum)
        {
            LineSegment segment = Document.GetLineSegment(lineNum);
            return Document.GetText(segment);
        }

        public void StartCtrlSpaceCompletion()
        {
            string extension = string.IsNullOrEmpty(FileName) ? ".asm" : Path.GetExtension(FileName);
            var bindings = CodeCompletionFactory.GetBindingForExtension(extension);
            bindings.ForEach(ccBinding => ccBinding.CtrlSpace(this));
        }

        private void Default_SettingChanging(object sender, SettingChangingEventArgs e)
        {
            if (e.NewValue == null)
            {
                return;
            }

            switch (e.SettingName)
            {
                case "MouseWheelScrollDown":
                    TextEditorProperties.MouseWheelScrollDown = !((bool)e.NewValue);
                    break;
                case "EnableFolding":
                    if ((bool)e.NewValue)
                    {
                        Document.FoldingManager.FoldingStrategy = new RegionFoldingStrategy();
                        Document.FoldingManager.UpdateFoldings(null, null);
                    }
                    else
                    {
                        Document.FoldingManager.FoldingStrategy = null;
                        Document.FoldingManager.UpdateFoldings(new List<FoldMarker>());
                    }
                    break;
                case "AutoIndent":
                    IndentStyle = (bool)e.NewValue ? IndentStyle.Smart : IndentStyle.None;
                    break;
                case "AntiAlias":
                    TextRenderingHint = (bool)e.NewValue ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
                    break;
                case "EditorFont":
                    Font = (Font)e.NewValue;
                    break;
                case "TabSize":
                    TabIndent = (int)e.NewValue;
                    break;
                case "ConvertTabs":
                    ConvertTabsToSpaces = (bool)e.NewValue;
                    break;
                case "ExternalHighlight":
                    UpdateHighlighting();
                    break;
                case "IconBar":
                    IsIconBarVisible = (bool)e.NewValue;
                    break;
                case "LineNumbers":
                    ShowLineNumbers = (bool)e.NewValue;
                    break;
            }
        }

        private void UpdateHighlighting()
        {
            if (!string.IsNullOrEmpty(FileName))
            {
                Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
            }
            else
            {
                SetHighlighting("Z80 Assembly");
            }
        }

        public void HighlightDebugLine(int lineNum)
        {
            string line = GetLineText(lineNum);
            Match match = LineRegex.Match(line);
            if (!match.Success)
            {
                return;
            }

            var lineGroup = match.Groups["line"];
            var segment = Document.GetLineSegment(lineNum);
            Document.MarkerStrategy.AddMarker(new DebugHighlightMarker(segment.Offset + lineGroup.Index, lineGroup.Length));
        }

        public void RemoveDebugHighlight()
        {
            Document.MarkerStrategy.RemoveAll(m => m is DebugHighlightMarker);
        }
    }

    public interface ICodeCompletionBinding
    {
        bool HandleKeyPress(WabbitcodeTextEditor editor, char ch);
        bool CtrlSpace(WabbitcodeTextEditor editor);
    }

    public static class CodeCompletionFactory
    {
        private static readonly Dictionary<string, List<ICodeCompletionBinding>> Bindings =
            new Dictionary<string, List<ICodeCompletionBinding>>(); 

        public static void RegisterCodeCompletionBinding(string extension, ICodeCompletionBinding binding)
        {
            if (!Bindings.ContainsKey(extension))
            {
                Bindings.Add(extension, new List<ICodeCompletionBinding>());
            }

            Bindings[extension].Add(binding);
        }

        public static IEnumerable<ICodeCompletionBinding> GetBindingForExtension(string extension)
        {
            List<ICodeCompletionBinding> bindings;
            Bindings.TryGetValue(extension, out bindings);
            return bindings ?? new List<ICodeCompletionBinding>();
        }
    }
}
