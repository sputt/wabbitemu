using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
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
    public partial class Editor : ITextEditor, IBookmarkable
    {
        #region Static Members

        private static bool _bindingsRegistered;

        internal static Editor OpenDocument(FilePath filename)
        {
            IDockingService dockingService = DependencyFactory.Resolve<IDockingService>();
            var child = dockingService.Documents.OfType<Editor>()
                .SingleOrDefault(e => e.FileName == filename);
            if (child != null)
            {
                child.Show();
                child.Activate();
                return child;
            }

            Editor doc = new Editor
            {
                Text = Path.GetFileName(filename),
                TabText = Path.GetFileName(filename),
                ToolTipText = filename
            };

            doc.OpenFile(filename);

            if (!Settings.Default.RecentFiles.Contains(filename))
            {
                Settings.Default.RecentFiles.Add(filename);
            }

            dockingService.ShowDockPanel(doc);
            return doc;
        }

        #endregion

        #region Private Memebers

        private readonly IDebuggerService _debuggerService;
        private readonly IDockingService _dockingService;
        private readonly IParserService _parserService;
        private readonly IProjectService _projectService;
        private int _stackTop;
        private volatile bool _wasExternallyModified;
        private bool _documentChanged;

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

        public Editor()
        {
            InitializeComponent();

            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _parserService = DependencyFactory.Resolve<IParserService>();
            _projectService = DependencyFactory.Resolve<IProjectService>();

            editorBox.TextChanged += editorBox_TextChanged;
            editorBox.ContextMenu = contextMenu;
            contextMenu.Popup += contextMenu_Popup;

            WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
            WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnBreakpointRemoved;
            _debuggerService.OnDebuggingStarted += (sender, e) => SetDebugging(true);
            _debuggerService.OnDebuggingEnded += (sender, e) => SetDebugging(false);

            if (_debuggerService.CurrentDebugger != null)
            {
                SetDebugging(true);
            }

            if (_projectService.Project != null)
            {
                _projectService.Project.FileModifiedExternally += Project_FileModifiedExternally;
            }
            _projectService.ProjectOpened += (sender, args) => _projectService.Project.FileModifiedExternally += Project_FileModifiedExternally;

            if (_bindingsRegistered)
            {
                return;
            }

            // TODO: fix
            //CodeCompletionFactory.RegisterCodeCompletionBinding(".asm", new Z80CodeCompletionBinding());
            //CodeCompletionFactory.RegisterCodeCompletionBinding(".z80", new Z80CodeCompletionBinding());
            //CodeCompletionFactory.RegisterCodeCompletionBinding(".inc", new Z80CodeCompletionBinding());

            _bindingsRegistered = true;
        }

        private void Project_FileModifiedExternally(object sender, FileModifiedEventArgs e)
        {
            if (e.File.FileFullPath != FileName || _wasExternallyModified)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.BeginInvoke(() => Project_FileModifiedExternally(sender, e));
                return;
            }

            if (IsDisposed)
            {
                return;
            }

            _wasExternallyModified = true;
            const string modifiedFormat = "{0} modified outside the editor.\nLoad changes?";
            DialogResult result = MessageBox.Show(this, string.Format(modifiedFormat, e.File.FileFullPath),
                "File modified", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                editorBox.ReloadFile();
            }
            else
            {
                DocumentChanged = true;
            }
            _wasExternallyModified = false;
        }

        private void contextMenu_Popup(object sender, EventArgs e)
        {
            bool hasSelection = editorBox.ActiveTextAreaControl.SelectionManager.HasSomethingSelected;
            cutContext.Enabled = hasSelection;
            copyContext.Enabled = hasSelection;
            renameContext.Enabled = !hasSelection;
            extractMethodContext.Enabled = hasSelection;

            if (string.IsNullOrEmpty(editorBox.Text))
            {
                return;
            }

            int lineNum = editorBox.ActiveTextAreaControl.Caret.Line;
            fixCaseContext.Visible = false;
            fixCaseContext.MenuItems.Clear();

            string line = editorBox.GetLineText(lineNum);
            Match match = Regex.Match(line, "#include ((\"(?<includeFile>.*)(?<paren>\\)?)\")|((?!\")(?<includeFile>.*(?!\"))(?<paren>\\)?)))");
            bool isInclude = match.Success;

            var caret = editorBox.ActiveTextAreaControl.Caret;
            var segment = editorBox.Document.GetLineSegment(caret.Line);
            var word = segment.GetWord(caret.Column);
            string text = word == null ? string.Empty : word.Word;
            if (word != null && !string.IsNullOrEmpty(text) && !isInclude)
            {
                IEnumerable<IParserData> parserData = _parserService.GetParserData(text, Settings.Default.CaseSensitive).ToList();
                if (parserData.Any())
                {
                    foreach (IParserData data in parserData.Where(data => data.Name != text))
                    {
                        fixCaseContext.Visible = true;
                        MenuItem item = new MenuItem(data.Name, fixCaseContext_Click);
                        fixCaseContext.MenuItems.Add(item);
                    }
                    bgotoButton.Enabled = true;
                }
                else
                {
                    bgotoButton.Enabled = false;
                }
            }

            string gotoLabel = isInclude ? match.Groups["includeFile"].Value.Replace('"', ' ').Trim() : text;
            if (gotoLabel == "_")
            {
                match = Regex.Match(line, "(?<offset>(\\+|\\-)*)_");
                gotoLabel = match.Groups["offset"].Value + gotoLabel;
            }

            int num;
            if (!int.TryParse(gotoLabel, out num))
            {
                if (isInclude)
                {
                    bool exists = Path.IsPathRooted(gotoLabel) ? File.Exists(gotoLabel) : FindFileIncludes(gotoLabel);
                    if (exists)
                    {
                        bgotoButton.Text = "Open " + gotoLabel;
                        bgotoButton.Enabled = true;
                    }
                    else
                    {
                        bgotoButton.Text = "File " + gotoLabel + " doesn't exist";
                        bgotoButton.Enabled = false;
                    }
                }
                else
                {
                    if (bgotoButton.Enabled)
                    {
                        bgotoButton.Text = "Goto " + gotoLabel;
                        bgotoButton.Enabled = !string.IsNullOrEmpty(gotoLabel);
                    }
                    else
                    {
                        bgotoButton.Text = "Unable to find " + gotoLabel;
                    }
                }
            }
            else
            {
                bgotoButton.Text = "Goto ";
                bgotoButton.Enabled = false;
            }
        }

        private void SetDebugging(bool debugging)
        {
            setNextStateMenuItem.Visible = debugging;
            editorBox.Document.ReadOnly = debugging;
            editorBox.RemoveDebugHighlight();

            if (!debugging)
            {
                return;
            }

            _debuggerService.CurrentDebugger.DebuggerRunningChanged += Debugger_OnDebuggerRunningChanged;
            _debuggerService.CurrentDebugger.DebuggerStep += Debugger_OnDebuggerStep;
            UpdateDebugHighlight();
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

        private void Debugger_OnDebuggerStep(object sender, DebuggerStepEventArgs e)
        {
            if (InvokeRequired)
            {
                this.Invoke(() => Debugger_OnDebuggerStep(sender, e));
                return;
            }

            editorBox.RemoveDebugHighlight();
            new GotoLineAction(e.Location.FileName, e.Location.LineNumber - 1).Execute();

            if (e.Location.FileName != FileName)
            {
                return;
            }

            editorBox.HighlightDebugLine(e.Location.LineNumber - 1);
        }

        private void Debugger_OnDebuggerRunningChanged(object sender, DebuggerRunningEventArgs e)
        {
            if (!IsHandleCreated)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.Invoke(() => Debugger_OnDebuggerRunningChanged(sender, e));
                return;
            }

            editorBox.RemoveDebugHighlight();
            if (e.Running)
            {
                Form activeForm = _dockingService.ActiveDocument as Form;
                if (activeForm != null)
                {
                    activeForm.Refresh();
                }
            }
            else
            {
                if (e.Location.FileName != FileName)
                {
                    return;
                }

                Activate();
                new GotoLineAction(e.Location.FileName, e.Location.LineNumber - 1).Execute();
                editorBox.HighlightDebugLine(e.Location.LineNumber - 1);
            }
        }

        private void editor_FormClosing(object sender, CancelEventArgs e)
        {
            editorBox.Document.MarkerStrategy.RemoveAll(s => true);

            AddFoldings();

            WabbitcodeBreakpointManager.OnBreakpointAdded -= WabbitcodeBreakpointManager_OnBreakpointAdded;
            WabbitcodeBreakpointManager.OnBreakpointRemoved -= WabbitcodeBreakpointManager_OnBreakpointRemoved;

            if (!DocumentChanged)
            {
                return;
            }

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

            if (MacroService.IsRecording)
            {
                MacroService.RecordKeyData(keyData);
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

        #region Context Menu

        private void cutContext_Click(object sender, EventArgs e)
        {
            Cut();
        }

        private void copyContext_Click(object sender, EventArgs e)
        {
            Copy();
        }

        private void pasteContext_Click(object sender, EventArgs e)
        {
            Paste();
        }

        private void selectAllContext_Click(object sender, EventArgs e)
        {
            SelectAll();
        }

        private void setNextStateMenuItem_Click(object sender, EventArgs e)
        {
            _debuggerService.CurrentDebugger.SetPCToSelect(FileName, CaretLine + 1);
        }

        private void bgotoButton_Click(object sender, EventArgs e)
        {
            // no need to make a stricter regex, as we own the inputs here
            Match match = Regex.Match(bgotoButton.Text, "(?<action>.*?) (?<name>.*)");
            string action = match.Groups["action"].Value;
            FilePath text = new FilePath(match.Groups["name"].Value);

            if (action == "Goto")
            {
                new GotoDefinitionAction(FileName, text, editorBox.ActiveTextAreaControl.Caret.Line).Execute();
            }
            else
            {
                FilePath fileFullPath = Path.IsPathRooted(text) ? text :
                    _projectService.Project.GetFilePathFromRelativePath(text).NormalizePath();
                new GotoFileAction(fileFullPath).Execute();
            }
        }

        #endregion

        #region Drag and Drop

        private void editor_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Copy : DragDropEffects.None;
        }

        private void editor_DragDrop(object sender, DragEventArgs e)
        {
            new DragDropCommand(e.Data).Execute();
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

        private void fixCaseContext_Click(object sender, EventArgs e)
        {
            MenuItem item = sender as MenuItem;
            if (item == null)
            {
                return;
            }

            var caret = editorBox.ActiveTextAreaControl.Caret;
            var segment = editorBox.Document.GetLineSegment(caret.Line);
            var word = segment.GetWord(caret.Column);
            editorBox.Document.Replace(segment.Offset + word.Offset, item.Text.Length, item.Text);
        }

        private void findRefContext_Click(object sender, EventArgs e)
        {
            new FindAllReferencesAction().Execute();
        }

        private void renameContext_Click(object sender, EventArgs e)
        {
            new RefactorRenameAction().Execute();
        }

        private void extractMethodContext_Click(object sender, EventArgs e)
        {
            new RefactorExtractMethodAction().Execute();
        }

        private bool FindFileIncludes(string gotoLabel)
        {
            return !string.IsNullOrEmpty(_projectService.Project.GetFilePathFromRelativePath(gotoLabel));
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

        public override void OpenFile(FilePath fileName)
        {
            base.OpenFile(fileName);

            editorBox.LoadFile(fileName, true, true);
            UpdateTabText();
            LoadFoldings();
            UpdateDebugHighlight();
        }

        public override void SaveFile()
        {
            _projectService.Project.EnableFileWatcher(false);
            if (string.IsNullOrEmpty(FileName))
            {
                new SaveAsCommand().Execute();
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
            IDocument document = editorBox.Document;
            int numLines = document.TotalNumberOfLines - 1;
            TextLocation selectStart = new TextLocation(0, 0);
            TextLocation selectEnd = new TextLocation(document.GetLineSegment(numLines).Length, numLines);
            editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(new DefaultSelection(document, selectStart, selectEnd));
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