using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.TextEditor.Gui.InsightWindow;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.EditorExtensions.Markers;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.TextEditor
{
    public sealed class WabbitcodeTextEditor : TextEditorControl
    {
        private InsightWindow _insightWindow;
        private CodeCompletionWindow _codeCompletionWindow;
        private bool _inHandleKeyPress;
        private int _debugHightlightLineNum;
        private TextMarker _debugHighlight;

        private static readonly Regex LineRegex = new Regex(@"^\s*(?<line>[\w|\s|,|\(|\)|:|\*|/|\+|\-|\$|\%|'|\\|\<|\>]*?)\s*(;.*)?$", RegexOptions.Compiled);

        private readonly Timer _textChangedTimer = new Timer
        {
            Interval = 5000,
            Enabled = false
        };

        private readonly Timer _updateRefsTimer = new Timer
        {
            Interval = 500,
            Enabled = false
        };

        private readonly ErrorUnderliner _errorUnderliner;
        private readonly SymbolUnderliner _symbolUnderliner;

        private bool _useTextHighlighting = true;

        public bool UseTextHighlighting
        {
            get { return _useTextHighlighting; }
            set
            {
                _useTextHighlighting = value;
                _errorUnderliner.Enabled = value;
            }
        }

        #region DI Members

        private readonly IBackgroundAssemblerService _backgroundAssemblerService = DependencyFactory.Resolve<IBackgroundAssemblerService>();
        private readonly IDebuggerService _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        private readonly IParserService _parserService = DependencyFactory.Resolve<IParserService>();
        private readonly ISymbolService _symbolService = DependencyFactory.Resolve<ISymbolService>();
        private readonly IStatusBarService _statusBarService = DependencyFactory.Resolve<IStatusBarService>();
        private readonly ICodeCompletionFactory _codeCompletionFactory = DependencyFactory.Resolve<ICodeCompletionFactory>();

        #endregion

        public WabbitcodeTextEditor()
        {
            TextEditorProperties = new TextEditorProperties();
            _errorUnderliner = new ErrorUnderliner(this);
            _symbolUnderliner = new SymbolUnderliner(this);

            Document.FormattingStrategy = new AsmFormattingStrategy();
            editactions.Add(Keys.Control | Keys.Space, new ShowCodeCompletion());
            SetHighlighting("Z80 Assembly");

            TextChanged += WabbitcodeTextEditor_TextChanged;
            Document.BreakpointManager.Added += BreakpointManager_Added;
            Document.BreakpointManager.Removed += BreakpointManager_Removed;
            Document.BreakpointManager.HighlightRegex = LineRegex;
            Enter += WabbitcodeTextEditor_Enter;

            _textChangedTimer.Tick += textChangedTimer_Tick;
            _updateRefsTimer.Tick += updateRefsTimer_Tick;
        }

        void WabbitcodeTextEditor_Enter(object sender, EventArgs e)
        {
            
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
            _updateRefsTimer.Enabled = false;
            _errorUnderliner.Dispose();
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
            TextAreaControl textAreaControl = (TextAreaControl) sender;
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
                    if (_codeCompletionFactory.GetBindingForExtension(extension)
                        .Any(ccBinding => ccBinding.HandleKeyPress(this, ch)))
                    {
                        return false;
                    }
                }
            }
            catch (Exception ex)
            {
                Logger.Log("Code completion exception", ex);
            }
            finally
            {
                _inHandleKeyPress = false;
            }

            return false;
        }

        private void BreakpointManager_Removed(object sender, BreakpointEventArgs e)
        {
            WabbitcodeBreakpointManager.RemoveBreakpoint(new FilePath(FileName), e.Breakpoint.LineNumber);
        }

        private void BreakpointManager_Added(object sender, BreakpointEventArgs e)
        {
            WabbitcodeBreakpointManager.AddBreakpoint(new FilePath(FileName), e.Breakpoint.LineNumber);
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

                if (data == null)
                {
                    var address = _symbolService.SymbolTable.GetAddressFromLabel(text);
                    tooltip = address == null ? string.Empty : address.Value.ToString();
                }
                else
                {
                    tooltip = data.Description;
                }
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

            Document.MarkerStrategy.RemoveAll(marker => marker is ReferenceMarker);
            if (_updateRefsTimer.Enabled)
            {
                _updateRefsTimer.Stop();
            }

            _updateRefsTimer.Start();

            _statusBarService.SetCaretPosition(ActiveTextAreaControl.Caret.Line, ActiveTextAreaControl.Caret.Column);
            UpdateCodeCountInfo();

            CalcLocation label = _symbolService.ListTable.GetCalcLocation(new FilePath(FileName), ActiveTextAreaControl.Caret.Line + 1);
            if (label == null)
            {
                return;
            }

            string assembledInfo = string.Format("Page: {0} Address: {1}", label.Page, label.Address.ToString("X4"));
            _statusBarService.SetText(assembledInfo);
        }

        private void updateRefsTimer_Tick(object sender, EventArgs e)
        {
            string word = GetWordAtCaret();
            var segments = Document.LineSegmentCollection.ToList();
            Task.Factory.StartNew(() => GetHighlightReferences(word, segments));
            _updateRefsTimer.Enabled = false;
        }

        private void textChangedTimer_Tick(object sender, EventArgs e)
        {
            string text = Text;
            Task.Factory.StartNew(() => _parserService.ParseFile(text.GetHashCode(), new FilePath(FileName), text));
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
            var selectionManager = ActiveTextAreaControl.SelectionManager;
            var column = selectionManager.HasSomethingSelected ? 
                selectionManager.GetSelectionAtLine(caret.Line).StartColumn : 
                caret.Column;
            var word = segment.GetWord(column);
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
            LineSegment startLine;
            LineSegment endLine;
            var selection = ActiveTextAreaControl.SelectionManager.SelectionCollection.FirstOrDefault();
            if (selection != null)
            {
                endLine = Document.GetLineSegmentForOffset(selection.EndOffset);
                startLine = Document.GetLineSegmentForOffset(selection.Offset);
                if (startLine.Offset > endLine.Offset)
                {
                    LineSegment temp = endLine;
                    endLine = startLine;
                    startLine = temp;
                }
            }
            else
            {
                endLine = Document.GetLineSegmentForOffset(ActiveTextAreaControl.Caret.Offset);
                startLine = endLine;
            }

            string codeInfoLines = Document.GetText(startLine.Offset,
                endLine.Offset + endLine.Length - startLine.Offset);
            return codeInfoLines;
        }

        private void GetHighlightReferences(string word, IEnumerable<LineSegment> segmentCollection)
        {
            if (string.IsNullOrEmpty(word) || !Settings.Default.ReferencesHighlighter || IsDisposed || Disposing)
            {
                return;
            }

            var options = Settings.Default.CaseSensitive ?
                StringComparison.Ordinal :
                StringComparison.OrdinalIgnoreCase;
            var references = from segment in segmentCollection
                from segmentWord in segment.Words
                where string.Equals(segmentWord.Word, word, options)
                select new ReferenceMarker(segment.Offset + segmentWord.Offset, segmentWord.Length);
            if (InvokeRequired && !IsDisposed)
            {
                this.Invoke(() => AddMarkers(references));
            }
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

        public string GetLineText(int lineNum)
        {
            LineSegment segment = Document.GetLineSegment(lineNum);
            return Document.GetText(segment);
        }

        public void StartCtrlSpaceCompletion()
        {
            string extension = string.IsNullOrEmpty(FileName) ? ".asm" : Path.GetExtension(FileName);
            var bindings = _codeCompletionFactory.GetBindingForExtension(extension);
            bindings.ForEach(ccBinding => ccBinding.CtrlSpace(this));
        }

        protected override void OnReloadHighlighting(object sender, EventArgs e)
        {
            try
            {
                if (!string.IsNullOrEmpty(FileName))
                {
                    IHighlightingStrategy strategy = HighlightingManager.Manager.FindHighlighterForFile(FileName);
                    SetHighlighting(strategy.Name);
                }
                else
                {
                    SetHighlighting("Z80 Assembly");
                }
            }
            catch (HighlightingDefinitionInvalidException ex)
            {
                DockingService.ShowError("Error loading highlighting", ex);
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

            if (_debugHighlight != null)
            {
                RemoveDebugHighlight();
            }

            _debugHightlightLineNum = lineNum;
            _debugHighlight = new DebugHighlightMarker(segment.Offset + lineGroup.Index, lineGroup.Length);
            Document.MarkerStrategy.AddMarker(_debugHighlight);
            Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, lineNum));
            Document.CommitUpdate();
        }

        public void RemoveDebugHighlight()
        {
            if (_debugHighlight == null)
            {
                return;
            }

            Document.MarkerStrategy.RemoveMarker(_debugHighlight);
            Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, _debugHightlightLineNum));
            Document.CommitUpdate();
        }

        public void ReloadFile()
        {
            int line = ActiveTextAreaControl.TextArea.Caret.Line;
            int col = ActiveTextAreaControl.TextArea.Caret.Column;
            LoadFile(FileName);
            ActiveTextAreaControl.TextArea.Caret.Line = line;
            ActiveTextAreaControl.TextArea.Caret.Column = col;
            ActiveTextAreaControl.TextArea.ScrollToCaret();
        }

        public void SelectAll()
        {
            int numLines = Document.TotalNumberOfLines - 1;
            TextLocation selectStart = new TextLocation(0, 0);
            TextLocation selectEnd = new TextLocation(Document.GetLineSegment(numLines).Length, numLines);
            ActiveTextAreaControl.SelectionManager.SetSelection(new DefaultSelection(Document, selectStart, selectEnd));
        }
    }

    public interface ICodeCompletionBinding
    {
        bool HandleKeyPress(WabbitcodeTextEditor editor, char ch);
        bool CtrlSpace(WabbitcodeTextEditor editor);
    }
}