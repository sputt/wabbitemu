using System.Text;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using GotoNextBookmark = Revsoft.TextEditor.Actions.GotoNextBookmark;
using ToggleBookmark = Revsoft.TextEditor.Actions.ToggleBookmark;

namespace Revsoft.Wabbitcode
{
    /// <summary>
	/// Summary description for frmDocument.
	/// </summary>
	public partial class Editor : ITextEditor, IBookmarkable
    {
        private static bool _bindingsRegistered;

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
            get
            {
                return _stackTop != editorBox.Document.UndoStack.UndoItemCount || _documentChanged;
            }
            set
            {
                _documentChanged = value;
            }
        }


        public int CaretLine
        {
            get
            {
                return editorBox.ActiveTextAreaControl.Caret.Line;
            }
            set
            {
                editorBox.ActiveTextAreaControl.Caret.Line = value;
                editorBox.ActiveTextAreaControl.ScrollToCaret();
            }
        }

        public int CaretColumn
        {
            get
            {
                return editorBox.ActiveTextAreaControl.Caret.Column;
            }
            set
            {
                editorBox.ActiveTextAreaControl.Caret.Column = value;
            }
        }

        public int CaretOffset
        {
            get
            {
                return editorBox.ActiveTextAreaControl.Caret.Offset;
            }
            set
            {
                var segment = editorBox.Document.GetLineSegmentForOffset(value);
                CaretColumn = value - segment.Offset;
                CaretLine = segment.LineNumber;
            }
        }

        public int TotalLines
        {
            get
            {
                return editorBox.Document.TotalNumberOfLines;
            }
        }

        #endregion

		public Editor()
		{
		    InitializeComponent();

            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();

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

		    if (!_bindingsRegistered)
		    {
		        return;
		    }

		    CodeCompletionFactory.RegisterCodeCompletionBinding(".asm", new Z80CodeCompletionBinding());
		    CodeCompletionFactory.RegisterCodeCompletionBinding(".z80", new Z80CodeCompletionBinding());
		    CodeCompletionFactory.RegisterCodeCompletionBinding(".inc", new Z80CodeCompletionBinding());

		    FileTypeMethodFactory.RegisterFileType(".asm", path => OpenDocument(path) != null);
		    FileTypeMethodFactory.RegisterFileType(".z80", path => OpenDocument(path) != null);
		    FileTypeMethodFactory.RegisterFileType(".inc", path => OpenDocument(path) != null);
		    FileTypeMethodFactory.RegisterDefaultHandler(path => OpenDocument(path) != null);
		    _bindingsRegistered = true;
		}

        private Editor OpenDocument(string filename)
        {
            var child = _dockingService.Documents.OfType<Editor>()
                .SingleOrDefault(e => FileOperations.CompareFilePath(e.FileName, filename));
            if (child != null)
            {
                child.Show();
                return child;
            }

            Editor doc = new Editor();
            OpenDocument(doc, filename);
            return doc;
        }

        private void OpenDocument(AbstractFileEditor doc, string filename)
        {
            doc.Text = Path.GetFileName(filename);
            doc.TabText = Path.GetFileName(filename);
            doc.ToolTipText = filename;
            doc.OpenFile(filename);
            AddRecentFile(filename);
            _dockingService.ShowDockPanel(doc);
        }

        /// <summary>
        /// Adds a string to the recent file list
        /// </summary>
        /// <param name="filename">Full path of the file to save to the list</param>
        private static void AddRecentFile(string filename)
        {
            if (!Settings.Default.RecentFiles.Contains(filename))
            {
                Settings.Default.RecentFiles.Add(filename);
            }
        }

        private void Project_FileModifiedExternally(object sender, FileModifiedEventArgs e)
        {
            if (!FileOperations.CompareFilePath(e.File.FileFullPath, FileName) || _wasExternallyModified)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.BeginInvoke(() => Project_FileModifiedExternally(sender, e));
                return;
            }

            _wasExternallyModified = true;
            const string modifiedFormat = "{0} modified outside the editor.\nLoad changes?";
            DialogResult result = MessageBox.Show(this, string.Format(modifiedFormat, e.File.FileFullPath),
                "File modified", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                editorBox.LoadFile(e.File.FileFullPath);
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
        }

		protected override string GetPersistString()
		{
			// Add extra information into the persist string for this document
			// so that it is available when deserialized.
			return GetType() + ";" + FileName + ";" + 
				   editorBox.ActiveTextAreaControl.Caret.Column + ";" +
				   editorBox.ActiveTextAreaControl.Caret.Line;
		}

        private void Debugger_OnDebuggerStep(object sender, DebuggerStepEventArgs e)
        {
            this.Invoke(() =>
            {
                editorBox.RemoveDebugHighlight();
                if (!FileOperations.CompareFilePath(e.Location.FileName, FileName))
                {
                    return;
                }

                new GotoLineAction(e.Location).Execute();
                editorBox.HighlightDebugLine(e.Location.LineNumber);
            });
        }

        private void Debugger_OnDebuggerRunningChanged(object sender, DebuggerRunningEventArgs e)
        {
            this.Invoke(() =>
            {
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
                    Activate();
                    if (!FileOperations.CompareFilePath(e.Location.FileName, FileName))
                    {
                        return;
                    }

                    new GotoLineAction(e.Location).Execute();
                    editorBox.HighlightDebugLine(e.Location.LineNumber);
                }
            });
        }

		private void ClearIcons()
		{
			editorBox.Document.IconManager.ClearIcons();
		}

        private void editor_FormClosing(object sender, CancelEventArgs e)
		{
			editorBox.Document.MarkerStrategy.RemoveAll(s => true);

            // TODO: save document foldings

            WabbitcodeBreakpointManager.OnBreakpointAdded -= WabbitcodeBreakpointManager_OnBreakpointAdded;
            WabbitcodeBreakpointManager.OnBreakpointRemoved -= WabbitcodeBreakpointManager_OnBreakpointRemoved;

			if (!DocumentChanged)
			{
				return;
			}

			if (string.IsNullOrEmpty(FileName))
			{
				FileName = "New Document";
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

			document.BreakpointManager.AddMark(new Breakpoint(document, new TextLocation(0,lineNum)));
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

		void WabbitcodeBreakpointManager_OnBreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e)
		{
			if (!FileOperations.CompareFilePath(e.Breakpoint.File, FileName))
			{
				return;
			}

			RemoveBreakpoint(e.Breakpoint.LineNumber);
		}

		void WabbitcodeBreakpointManager_OnBreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e)
		{
			if (!FileOperations.CompareFilePath(e.Breakpoint.File, FileName))
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
			string text = match.Groups["name"].Value;

			if (action == "Goto")
			{
			    new GotoDefinitionAction(FileName, text, editorBox.ActiveTextAreaControl.Caret.Line).Execute();
			}
			else
			{
				string fileFullPath = Path.IsPathRooted(text) ? text : FileOperations.NormalizePath(
                    _projectService.Project.GetFilePathFromRelativePath(text));
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
		    var array = _dockingService.Documents.Where(child => child != this).ToArray();
			foreach (Editor child in array)
			{
				child.Close();
			}
		}

		private void closeAllMenuItem_Click(object sender, EventArgs e)
		{
			var array = _dockingService.Documents.ToArray();
			foreach (Editor child in array)
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
				StartInfo = { FileName = dir }
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

        public void SetSelection(int offset, int length)
        {
            var segment = editorBox.Document.GetLineSegmentForOffset(offset);
            int line = segment.Offset;
            int col = offset - segment.Offset;
            editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(new TextLocation(line, col), new TextLocation(line, col + length));
            editorBox.ActiveTextAreaControl.ScrollToCaret();
        }

        public void SelectedTextToLower()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string newText = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText.ToLower();
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
			editorBox.Document.UndoStack.EndUndoGroup();
		}

        public void SelectedTextToUpper()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string newText = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText.ToUpper();
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
			editorBox.Document.UndoStack.EndUndoGroup();
		}

        public void SelectedTextInvertCase()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string text = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText;
			char[] textarray = text.ToCharArray();
			for (int i = 0; i < textarray.Length; i++)
			{
			    if (textarray[i] >= 'A' && textarray[i] <= 'Z')
			    {
			        textarray[i] = (char) (textarray[i] + 'A' - 'a');
			    }
                else if (textarray[i] >= 'a' && textarray[i] <= 'z')
                {
                    textarray[i] = (char) (textarray[i] - 'A' - 'a');
                }
			}
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(new string(textarray));
			editorBox.Document.UndoStack.EndUndoGroup();
		}

        public void SelectedTextToSentenceCase()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string text = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText;
			string newText = CultureInfo.CurrentCulture.TextInfo.ToTitleCase(text);
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
			editorBox.Document.UndoStack.EndUndoGroup();
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
					continue;

				if (line.IndexOf(';') != -1)
				{
					comment = line.Substring(line.IndexOf(';'));
					line = line.Remove(line.IndexOf(';'));
				}
				bool islabel = !string.IsNullOrEmpty(line) && (!char.IsWhiteSpace(line[0]) || line[0] == '_');
				line = line.Trim();
				if (line.StartsWith("push"))
					currentIndent += indent;
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
			StringBuilder spacesString = new StringBuilder();
			for (int i = 0; i < Settings.Default.TabSize; i++)
			{
				spacesString.Append(' ');
			}
			editorBox.Document.TextContent = editorBox.Document.TextContent.Replace(spacesString.ToString(), "\t");
		}

        public string GetWordAtCaret()
        {
            return editorBox.GetWordAtCaret();
        }

	    public void ShowFindForm(Form owner, SearchMode mode)
	    {
	        FindAndReplaceForm.Instance.ShowFor(owner, editorBox, mode);
	    }

        public override void OpenFile(string filename)
        {
            editorBox.LoadFile(filename, true, true);
            UpdateTabText();
            ClearIcons();

            base.OpenFile(filename);
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
