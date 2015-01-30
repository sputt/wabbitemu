using System;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.GUI.Menus;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.TextEditor.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WeifenLuo.WinFormsUI.Docking;
using GotoNextBookmark = Revsoft.TextEditor.Actions.GotoNextBookmark;
using InvertCaseAction = Revsoft.TextEditor.Actions.InvertCaseAction;
using ToggleBookmark = Revsoft.TextEditor.Actions.ToggleBookmark;

namespace Revsoft.Wabbitcode.GUI.DocumentWindows
{
    /// <summary>
    /// Summary description for frmDocument.
    /// </summary>
    public partial class TextEditor : AbstractFileEditor, ITextEditor, IBookmarkable
    {
        #region Static Methods

        static TextEditor()
        {
            Z80CodeCompletionBinding completionBinding = new Z80CodeCompletionBinding();
            var codeCompletionFactory = DependencyFactory.Resolve<ICodeCompletionFactory>();
            codeCompletionFactory.RegisterCodeCompletionBinding(".asm", completionBinding);
            codeCompletionFactory.RegisterCodeCompletionBinding(".z80", completionBinding);
            codeCompletionFactory.RegisterCodeCompletionBinding(".inc", completionBinding);
        }

        internal static TextEditor OpenDocument(FilePath filename)
        {
            IDockingService dockingService = DependencyFactory.Resolve<IDockingService>();
            var child = dockingService.Documents.OfType<TextEditor>()
                .SingleOrDefault(e => e.FileName == filename);
            if (child != null)
            {
                child.Show();
                child.Activate();
                return child;
            }

            TextEditor doc = new TextEditor
            {
                Text = Path.GetFileName(filename),
                TabText = Path.GetFileName(filename),
                ToolTipText = filename
            };

            doc.OpenFile(filename);

            dockingService.ShowDockPanel(doc);
            return doc;
        }

        #endregion

        #region Private Memebers

        private readonly IDebuggerService _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        private readonly IDockingService _dockingService = DependencyFactory.Resolve<IDockingService>();
        private readonly IParserService _parserService = DependencyFactory.Resolve<IParserService>();
        private readonly IProjectService _projectService = DependencyFactory.Resolve<IProjectService>();
        private readonly IMacroService _macroService = DependencyFactory.Resolve<IMacroService>();
        private int _stackTop;
        private bool _documentChanged;

        #endregion

        #region Events

        public event FilePathEvent FilePathChanged;

        #endregion

        #region Properties

        protected override bool DocumentChanged
        {
            get { return _stackTop != editorBox.Document.UndoStack.UndoItemCount || _documentChanged; }
            set { _documentChanged = value; }
        }

        public int CaretLine
        {
            get { return editorBox.ActiveTextAreaControl.Caret.Line; }
            set
            {
                editorBox.ActiveTextAreaControl.Caret.Line = value;
                editorBox.ActiveTextAreaControl.ScrollToCaret();
            }
        }

        public int CaretColumn
        {
            get { return editorBox.ActiveTextAreaControl.Caret.Column; }
            set { editorBox.ActiveTextAreaControl.Caret.Column = value; }
        }

        public int CaretOffset
        {
            get { return editorBox.ActiveTextAreaControl.Caret.Offset; }
            set
            {
                var segment = editorBox.Document.GetLineSegmentForOffset(value);
                CaretColumn = value - segment.Offset;
                CaretLine = segment.LineNumber;
            }
        }

        public int TotalLines
        {
            get { return editorBox.Document.TotalNumberOfLines; }
        }

        public IDocument Document
        {
            get { return editorBox.Document; }
        }

        #endregion

        public TextEditor()
        {
            InitializeComponent();

            editorBox.IsIconBarVisible = true;
            editorBox.TextChanged += editorBox_TextChanged;
            editorBox.ContextMenu = new EditorContextMenu(editorBox, _debuggerService, _parserService, _projectService);

            WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
            WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnBreakpointRemoved;
            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += DebuggerService_OnDebuggingEnded;

            if (_debuggerService.CurrentDebugger != null)
            {
                SetDebugging(true);
            }
        }

        private void DebuggerService_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            SetDebugging(false);
        }

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            SetDebugging(true);
        }

        private void SetDebugging(bool debugging)
        {
            editorBox.Document.ReadOnly = debugging;
            editorBox.RemoveDebugHighlight();

            if (debugging)
            {
                UpdateDebugHighlight();
            }
        }

        private void UpdateDebugHighlight()
        {
            IWabbitcodeDebugger debugger = _debuggerService.CurrentDebugger;
            if (debugger == null)
            {
                return;
            }

            DocumentLocation debugLine = debugger.GetAddressLocation(_debuggerService.CurrentDebugger.CPU.PC);
            if (debugLine == null || debugLine.FileName != FileName)
            {
                return;
            }

            editorBox.HighlightDebugLine(debugLine.LineNumber - 1);
        }

        protected override string GetPersistString()
        {
            // Add extra information into the persist string for this document
            // so that it is available when deserialized.
            return base.GetPersistString() + ";" +
                   editorBox.ActiveTextAreaControl.Caret.Line + ";" +
                   editorBox.ActiveTextAreaControl.Caret.Column + ";" +
                   editorBox.ActiveTextAreaControl.VScrollBar.Value + ";" +
                   editorBox.ActiveTextAreaControl.HScrollBar.Value;
        }

        private void Debugger_OnDebuggerRunningChanged(object sender, DebuggerRunningEventArgs e)
        {
            this.Invoke(() =>
            {
                
            });
        }

        private void editor_FormClosing(object sender, CancelEventArgs e)
        {
            if (DocumentChanged)
            {
                if (string.IsNullOrEmpty(FileName))
                {
                    FileName = new FilePath("New Document");
                }

                DialogResult dlg = MessageBox.Show(this, "Document '" + FileName + "' has changed. Save changes?", "Wabbitcode", MessageBoxButtons.YesNoCancel);
                switch (dlg)
                {
                    case DialogResult.Cancel:
                        e.Cancel = true;
                        break;
                    case DialogResult.Yes:
                        SaveFile();
                        break;
                }
            }

            CloseFile();
        }

        #region Folding

        private void AddFoldings()
        {
            if (string.IsNullOrEmpty(FileName))
            {
                return;
            }

            string foldingString = string.Format("{0}|{1}", FileName, editorBox.Document.FoldingManager.SerializeToString());
            FindDocumentFolding(folding => Settings.Default.DocumentFoldings.Remove(folding));

            Settings.Default.DocumentFoldings.Add(foldingString);
        }

        private void FindDocumentFolding(Action<string> action)
        {
            if (Settings.Default.DocumentFoldings == null)
            {
                Settings.Default.DocumentFoldings = new StringCollection();
            }

            foreach (string folding in Settings.Default.DocumentFoldings)
            {
                FilePath fileName = new FilePath(folding.Split('|').First());
                if (fileName != FileName)
                {
                    continue;
                }

                action(folding);
                return;
            }
        }

        private void LoadFoldings()
        {
            FindDocumentFolding(folding => editorBox.Document.FoldingManager.DeserializeFromString(folding.Split('|').Last()));
        }

        #endregion

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (keyData == Keys.F3)
            {
                //_dockingService.FindForm.FindNext(true, false, "Text not found");
            }

            if (_macroService.IsRecording)
            {
                _macroService.RecordKeyData(keyData);
            }
            return base.ProcessCmdKey(ref msg, keyData);
        }

        #region Breakpoints

        private void AddBreakpoint(int lineNum)
        {
            IDocument document = editorBox.Document;
            if (document.BreakpointManager.IsMarked(lineNum))
            {
                return;
            }

            document.BreakpointManager.AddMark(new Breakpoint(document, new TextLocation(0, lineNum)));
            UpdateDocument(lineNum);
            Refresh();
        }

        private void RemoveBreakpoint(int lineNum)
        {
            IDocument document = editorBox.Document;
            Breakpoint breakpoint = document.BreakpointManager.GetFirstMark(s => s.Anchor.LineNumber == lineNum);
            if (breakpoint == null)
            {
                return;
            }

            document.BreakpointManager.RemoveMark(breakpoint);
            UpdateDocument(lineNum);
        }

        private void UpdateAllBreakpoints()
        {
            foreach (var breakpoint in WabbitcodeBreakpointManager.Breakpoints.Where(br => br.File == FileName))
            {
                AddBreakpoint(breakpoint.LineNumber);
            }
        }

        #region Breakpoint Manager

        private void WabbitcodeBreakpointManager_OnBreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e)
        {
            if (e.Breakpoint.File != FileName)
            {
                return;
            }

            RemoveBreakpoint(e.Breakpoint.LineNumber);
        }

        private void WabbitcodeBreakpointManager_OnBreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e)
        {
            if (e.Breakpoint.File != FileName)
            {
                return;
            }

            AddBreakpoint(e.Breakpoint.LineNumber);
        }

        #endregion

        #endregion

        #region Drag and Drop

        private void editor_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Copy : DragDropEffects.None;
        }

        private void editor_DragDrop(object sender, DragEventArgs e)
        {
            AbstractUiAction.RunCommand(new DragDropCommand(e.Data));
        }

        #endregion

        private void editorBox_TextChanged(object sender, EventArgs e)
        {
            UpdateTabText();
        }

        private void UpdateTabText()
        {
            string changedString = DocumentChanged ? "*" : string.Empty;
            if (!string.IsNullOrEmpty(FileName))
            {
                TabText = Path.GetFileName(FileName) + changedString;
            }
            else
            {
                TabText = "New Document" + changedString;
            }
        }

        #region TitleBarContext

        private void saveMenuItem_Click(object sender, EventArgs e)
        {
            SaveFile();
        }

        private void closeMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void closeAllOtherMenuItem_Click(object sender, EventArgs e)
        {
            var array = _dockingService.Documents.OfType<DockContent>().Where(child => child != this).ToArray();
            foreach (DockContent child in array)
            {
                child.Close();
            }
        }

        private void closeAllMenuItem_Click(object sender, EventArgs e)
        {
            var array = _dockingService.Documents.OfType<DockContent>().ToArray();
            foreach (DockContent child in array)
            {
                child.Close();
            }
        }

        private void copyPathMenuItem_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(FileName);
        }

        private void openFolderMenuItem_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(FileName))
            {
                return;
            }

            string dir = Path.GetDirectoryName(FileName);
            if (string.IsNullOrEmpty(dir))
            {
                return;
            }

            Process explorer = new Process
            {
                StartInfo = {FileName = dir}
            };
            explorer.Start();
        }

        #endregion

        public void GotoLine(int line)
        {
            editorBox.ActiveTextAreaControl.ScrollTo(line);
            editorBox.ActiveTextAreaControl.Caret.Line = line;
            editorBox.ActiveTextAreaControl.Caret.Column = 0;
        }

        public void HighlightDebugLine(int lineNumber)
        {
            editorBox.HighlightDebugLine(lineNumber);
        }

        public void RemoveDebugHighlight()
        {
            editorBox.RemoveDebugHighlight();
        }

        #region Modify Selected Text

        public string GetSelection()
        {
            return editorBox.GetWholeLinesSelected();
        }

        public void SetSelection(int offset, int length)
        {
            var segment = editorBox.Document.GetLineSegmentForOffset(offset);
            int line = segment.LineNumber;
            int col = offset - segment.Offset;
            editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(new TextLocation(col, line),
                new TextLocation(col + length, line));
            editorBox.ActiveTextAreaControl.ScrollTo(line, col);
        }

        public void SelectedTextToLower()
        {
            var action = new ToLowerCase();
            action.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        public void SelectedTextToUpper()
        {
            var action = new ToUpperCase();
            action.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        public void SelectedTextInvertCase()
        {
            var action = new InvertCaseAction();
            action.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        public void SelectedTextToSentenceCase()
        {
            var action = new CapitalizeAction();
            action.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        public void FormatLines()
        {
            string[] lines = editorBox.Text.Split('\n');
            const string indent = "\t";
            string currentIndent = indent;
            for (int i = 0; i < lines.Length; i++)
            {
                string line = lines[i];
                string comment = string.Empty;
                if (line.Trim().Length == 0)
                {
                    continue;
                }

                if (line.IndexOf(';') != -1)
                {
                    comment = line.Substring(line.IndexOf(';'));
                    line = line.Remove(line.IndexOf(';'));
                }
                bool islabel = !string.IsNullOrEmpty(line) && (!char.IsWhiteSpace(line[0]) || line[0] == '_');
                line = line.Trim();
                if (line.StartsWith("push"))
                {
                    currentIndent += indent;
                }
                if ((line.StartsWith("pop") || line.StartsWith("ret")) && currentIndent.Length > 1)
                {
                    currentIndent = currentIndent.Remove(currentIndent.Length - 1);
                }
                if (!islabel)
                {
                    line = currentIndent + line;
                }
                lines[i] = line + comment;
            }
            StringBuilder newText = new StringBuilder();
            foreach (string line in lines)
            {
                newText.AppendLine(line);
            }

            editorBox.Document.TextContent = newText.ToString();
        }

        #endregion

        #region Bookmarks

        public void GotoNextBookmark()
        {
            GotoNextBookmark next = new GotoNextBookmark(bookmark => true);
            next.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        public void GotoPrevBookmark()
        {
            GotoPrevBookmark next = new GotoPrevBookmark(bookmark => true);
            next.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        public void ToggleBookmark()
        {
            ToggleBookmark toggle = new ToggleBookmark();
            toggle.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        #endregion

        protected override void CloseFile()
        {
            base.CloseFile();

            AddFoldings();
            editorBox.Document.MarkerStrategy.RemoveAll(s => true);
            WabbitcodeBreakpointManager.OnBreakpointAdded -= WabbitcodeBreakpointManager_OnBreakpointAdded;
            WabbitcodeBreakpointManager.OnBreakpointRemoved -= WabbitcodeBreakpointManager_OnBreakpointRemoved;

            _debuggerService.OnDebuggingStarted -= DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded -= DebuggerService_OnDebuggingEnded;
        }

        private void UpdateDocument(int line)
        {
            editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, line));
        }

        public void ConvertSpacesToTabs()
        {
            var action = new ConvertSpacesToTabs();
            action.Execute(editorBox.ActiveTextAreaControl.TextArea);
        }

        public string GetWordAtCaret()
        {
            return editorBox.GetWordAtCaret();
        }

        public void ShowFindForm(Form owner, SearchMode mode)
        {
            FindAndReplaceForm.Instance.ShowFor(owner, editorBox, mode);
        }

        protected override void OpenFile(FilePath fileName)
        {
            base.OpenFile(fileName);

            editorBox.LoadFile(fileName, true, true);
            UpdateTabText();
            LoadFoldings();
            UpdateDebugHighlight();
            UpdateAllBreakpoints();
        }

        public override void SaveFile()
        {
            _projectService.Project.EnableFileWatcher(false);
            if (string.IsNullOrEmpty(FileName))
            {
                AbstractUiAction.RunCommand(new SaveAsCommand(this));
                return;
            }

            _stackTop = editorBox.Document.UndoStack.UndoItemCount;
            try
            {
                editorBox.SaveFile(FileName);
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error saving the file", ex);
            }

            _projectService.Project.EnableFileWatcher(true);

            editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
            base.SaveFile();
        }

        public override void SaveFile(FilePath filePath)
        {
            FilePath oldFilePath = FileName;
            bool notifyChanged = filePath != FileName;
            
            base.SaveFile();

            if (!notifyChanged)
            {
                return;
            }

            if (FilePathChanged != null)
            {
                FilePathChanged(this, new FilePathEventArgs(oldFilePath, filePath));
            }
        }

        protected override void ReloadFile()
        {
            editorBox.ReloadFile();
        }

        public override void PersistStringLoad(params string[] persistStrings)
        {
            base.PersistStringLoad(persistStrings);

            int column;
            int line;
            int vScrollValue;
            int hScrollValue;

            if (persistStrings.Length > 2 && int.TryParse(persistStrings[2], out line))
            {
                CaretLine = line;
            }

            if (persistStrings.Length > 3 && int.TryParse(persistStrings[3], out column))
            {
                CaretColumn = column;
            }

            if (persistStrings.Length > 4 && int.TryParse(persistStrings[4], out vScrollValue))
            {
                editorBox.ActiveTextAreaControl.VScrollBar.Value = vScrollValue;
            }

            if (persistStrings.Length > 5 && int.TryParse(persistStrings[5], out hScrollValue))
            {
                editorBox.ActiveTextAreaControl.HScrollBar.Value = hScrollValue;
            }
        }

        #region IUndoable

        public override void Undo()
        {
            editorBox.Undo();
        }

        public override void Redo()
        {
            editorBox.Redo();
        }

        #endregion

        #region ISelectable

        public override void SelectAll()
        {
            editorBox.SelectAll();
        }

        #endregion

        #region IClipboardOperation

        public override void Cut()
        {
            editorBox.Cut();
        }

        public override void Copy()
        {
            editorBox.Copy();
        }

        public override void Paste()
        {
            editorBox.Paste();
        }

        #endregion
    }
}