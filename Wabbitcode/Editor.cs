using System.Text;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.TextEditor.Gui.InsightWindow;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using BreakpointEventArgs = Revsoft.TextEditor.Document.BreakpointEventArgs;
using GotoNextBookmark = Revsoft.TextEditor.Actions.GotoNextBookmark;
using Timer = System.Windows.Forms.Timer;
using ToggleBookmark = Revsoft.TextEditor.Actions.ToggleBookmark;

namespace Revsoft.Wabbitcode
{
    /// <summary>
	/// Summary description for frmDocument.
	/// </summary>
	public partial class Editor : ITextEditor, IBookmarkable
    {
		#region Private Memebers
	    private static readonly Regex LineRegex = new Regex(@"^\s*(?<line>[\w|\s|,|\(|\)|:|\*|/|\+|\-|\$|\%|'|\\|\<|\>]*?)\s*(;.*)?$", RegexOptions.Compiled);

	    private readonly Bitmap _warningBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.Warning16.png"));
		private readonly Bitmap _errorBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.error.png"));
		private readonly List<CancellationTokenSource> _queuedFiles = new List<CancellationTokenSource>();
		private CancellationTokenSource _highlightRefsCancellationTokenSource;
		private readonly IBackgroundAssemblerService _backgroundAssemblerService;
        private readonly IDebuggerService _debuggerService;
		private readonly IDockingService _dockingService;
		private readonly IDocumentService _documentService;
		private readonly IParserService _parserService;
		private readonly IProjectService _projectService;
		private readonly ISymbolService _symbolService;
        private readonly IStatusBarService _statusBarService;
        private int _stackTop;
		private bool _isUpdatingRefs;
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

			_backgroundAssemblerService = ServiceFactory.Instance.GetServiceInstance<IBackgroundAssemblerService>();
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _symbolService = ServiceFactory.Instance.GetServiceInstance<ISymbolService>();
		    _statusBarService = ServiceFactory.Instance.GetServiceInstance<IStatusBarService>();

		    _textChangedTimer.Tick += textChangedTimer_Tick;

			editorBox.Document.FormattingStrategy = new AsmFormattingStrategy();
            editorBox.SetHighlighting("Z80 Assembly");
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

		    _backgroundAssemblerService.OnBackgroundAssemblerComplete += (sender, args) => UpdateIcons(args.Output.ParsedErrors);
		    if (_projectService.Project != null)
		    {
		        _projectService.Project.FileModifiedExternally += ProjectFileModifiedExternally;
		    }
            _projectService.ProjectOpened += (sender, args) => _projectService.Project.FileModifiedExternally += ProjectFileModifiedExternally;

		    CodeCompletionKeyHandler.Attach(this, editorBox, _parserService);
		}

        private void ProjectFileModifiedExternally(object sender, FileModifiedEventArgs e)
        {
            if (!FileOperations.CompareFilePath(e.File.FileFullPath, FileName) || _wasExternallyModified)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.BeginInvoke(() => ProjectFileModifiedExternally(sender, e));
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

            if (!string.IsNullOrEmpty(editorBox.Text))
            {
                int lineNum = editorBox.ActiveTextAreaControl.Caret.Line;
                fixCaseContext.Visible = false;
                fixCaseContext.MenuItems.Clear();

                string line = GetLineText(lineNum);
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
        }

        private void SetDebugging(bool debugging)
        {
            setNextStateMenuItem.Visible = debugging;
            editorBox.Document.ReadOnly = debugging;
        }

		protected override string GetPersistString()
		{
			// Add extra information into the persist string for this document
			// so that it is available when deserialized.
			return GetType() + ";" + FileName + ";" + 
				   editorBox.ActiveTextAreaControl.Caret.Column + ";" +
				   editorBox.ActiveTextAreaControl.Caret.Line;
		}

		private void TextArea_ToolTipRequest(object sender, ToolTipRequestEventArgs e)
		{
			if (!e.InDocument)
			{
				return;
			}

		    var segment = editorBox.Document.GetLineSegment(e.LogicalPosition.Line);
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
		    if (string.IsNullOrEmpty(editorBox.Text))
			{
				return;
			}

		    UpdateCodeCountInfo();
		}

        private void UpdateCodeCountInfo()
        {
            int startLine;
            int endLine;
            var selection = editorBox.ActiveTextAreaControl.SelectionManager.SelectionCollection.FirstOrDefault();
            if (selection != null)
            {
                endLine = editorBox.Document.GetLineNumberForOffset(selection.EndOffset);
                startLine = editorBox.Document.GetLineNumberForOffset(selection.Offset);
                if (startLine > endLine)
                {
                    int temp = endLine;
                    endLine = startLine;
                    startLine = temp;
                }
            }
            else
            {
                endLine = CaretLine;
                startLine = endLine;
            }

            string codeInfoLines = string.Empty;
            for (; startLine <= endLine; startLine++)
            {
                codeInfoLines += GetLineText(startLine) + Environment.NewLine;
            }

            _backgroundAssemblerService.CountCode(codeInfoLines);
        }

        private void Caret_PositionChanged(object sender, EventArgs e)
		{
			if (editorBox.Document.TextLength == 0)
			{
				return;
			}

			editorBox.Document.MarkerStrategy.RemoveAll(marker => marker != null && marker.Tag == "Reference");
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
			if (!_isUpdatingRefs)
			{
				_isUpdatingRefs = true;
				string word = GetWordAtCaret();
				_highlightRefsCancellationTokenSource = new CancellationTokenSource();
				var segments = editorBox.Document.LineSegmentCollection;
				Task.Factory.StartNew(() => GetHighlightReferences(word, segments), _highlightRefsCancellationTokenSource.Token);
			}

		    _statusBarService.SetCaretPosition(CaretLine, CaretColumn);
            UpdateCodeCountInfo();

            CalcLocation label = _symbolService.ListTable.GetCalcLocation(FileName, CaretLine);
            if (label == null)
            {
                return;
            }

            string assembledInfo = string.Format("Page: {0} Address: {1}", label.Page, label.Address.ToString("X4"));
            _statusBarService.SetText(assembledInfo);
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
		        select new TextMarker(segment.Offset + segmentWord.Offset, segmentWord.Length, TextMarkerType.SolidBlock, Color.LightGray)
		        {
		            Tag = "Reference"
		        };
		    if (InvokeRequired && !IsDisposed)
		    {
		        this.Invoke(() => AddMarkers(references));
		    }
		    _isUpdatingRefs = false;
		}

		private void UpdateIcons(IEnumerable<BuildError> errorsInFiles)
		{
		    IconManager iconManager = editorBox.Document.IconManager;
		    iconManager.ClearIcons();
			var errorWarnings = errorsInFiles.Where(issue => FileOperations.CompareFilePath(issue.File, editorBox.FileName));
			foreach (MarginIcon marginIcon in errorWarnings.Select(issue => 
                new MarginIcon(issue.IsWarning ? _warningBitmap : _errorBitmap, issue.LineNum - 1, issue.ToolTip)))
			{
			    iconManager.AddIcon(marginIcon);
			}
		}

		private void ClearIcons()
		{
			editorBox.Document.IconManager.ClearIcons();
		}

        /// <summary>
		/// Synchronizes the breakpoints known by the Breakpoint manager with the breakpoint manager
		/// contained within the text editor
		/// </summary>
		private void LoadBreakpoints()
		{
			editorBox.Document.BreakpointManager.Added -= BreakpointManager_Added;
			var breakpoints = WabbitcodeBreakpointManager.Breakpoints.Where(
				b => FileOperations.CompareFilePath(b.File, FileName));
			foreach (var breakpoint in breakpoints)
			{
				editorBox.Document.BreakpointManager.AddMark(new Breakpoint(editorBox.Document, 
					new TextLocation(0, breakpoint.LineNumber)));
			}
			editorBox.Document.BreakpointManager.Added += BreakpointManager_Added;
		}

        private void editor_FormClosing(object sender, CancelEventArgs e)
		{
			_textChangedTimer.Enabled = false;

			editorBox.Document.MarkerStrategy.RemoveAll(s => true);

			// Cancel any tasks
			foreach (CancellationTokenSource item in _queuedFiles)
			{
				item.Cancel();
			}

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

        private void editorBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			TextAreaControl activeTextAreaControl = editorBox.ActiveTextAreaControl;
			activeTextAreaControl.Document.FormattingStrategy.FormatLine(
				activeTextAreaControl.TextArea, activeTextAreaControl.Caret.Line, 0, e.KeyChar);
            if (e.KeyChar == '(')
            {
                ShowInsightWindow(new MacroInsightProvider());
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

		void BreakpointManager_Removed(object sender, BreakpointEventArgs e)
		{
			WabbitcodeBreakpointManager.RemoveBreakpoint(FileName, e.Breakpoint.LineNumber);
		}

		void BreakpointManager_Added(object sender, BreakpointEventArgs e)
		{
			WabbitcodeBreakpointManager.AddBreakpoint(FileName, e.Breakpoint.LineNumber);
		}

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
			    new GotoLabelAction(FileName, text, editorBox.ActiveTextAreaControl.Caret.Line).Execute();
			}
			else
			{
				string fileFullPath = Path.IsPathRooted(text) ? text : FileOperations.NormalizePath(
                    _projectService.Project.GetFilePathFromRelativePath(text));
				_documentService.GotoFile(fileFullPath);
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

        private readonly Timer _textChangedTimer = new Timer
		{
			Interval = 5000,
			Enabled = false
		};

        private void editorBox_TextChanged(object sender, EventArgs e)
		{
			if (_textChangedTimer.Enabled)
			{
				_textChangedTimer.Stop();
			}

			_textChangedTimer.Start();
			if (Settings.Default.EnableFolding)
			{
				editorBox.Document.FoldingManager.FoldingStrategy = new RegionFoldingStrategy();
				editorBox.Document.FoldingManager.UpdateFoldings(null, null);
			}
			else
			{
				editorBox.Document.FoldingManager.FoldingStrategy = null;
				editorBox.Document.FoldingManager.UpdateFoldings(new List<FoldMarker>());
			}
			if (editorBox.Document.UndoStack.UndoItemCount != _stackTop)
			{
				DocumentChanged = true;
				UpdateTabText();
			}
			else
			{
				DocumentChanged = false;
				UpdateTabText();
			}
		}

		void textChangedTimer_Tick(object sender, EventArgs e)
		{
			UpdateAll(editorBox.Text);
			_textChangedTimer.Enabled = false;
		}

		void UpdateAll(string editorText)
		{
			CancellationTokenSource cancellationSource = new CancellationTokenSource();
			_queuedFiles.Add(cancellationSource);
            Task.Factory.StartNew(() => _parserService.ParseFile(editorText.GetHashCode(), FileName, editorText),
                cancellationSource.Token);
			_backgroundAssemblerService.RequestAssemble();
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

	    internal void HighlightLine(int newLineNumber, Color foregroundColor, string tag)
		{
			int lineNum = newLineNumber - 1;
		    string line = GetLineText(lineNum);

	        Match match = LineRegex.Match(line);
            Group group = match.Groups["line"];
		    int start = group.Index + editorBox.Document.GetLineSegment(lineNum).Offset;
            int length = group.Length;
            if (length == 0)
            {
                return;
            }

		    TextMarker highlight = new TextMarker(start, length, TextMarkerType.SolidBlock, foregroundColor, Color.Black)
		    {
		        Tag = tag
		    };
		    editorBox.Document.MarkerStrategy.AddMarker(highlight);
		    editorBox.Refresh();
		    GotoLine(lineNum);
		}

        private string GetLineText(int lineNum)
        {
            LineSegment segment = editorBox.Document.GetLineSegment(lineNum);
            string line = string.Empty;
            foreach (var word in segment.Words)
            {
                switch (word.Type)
                {
                    case TextWordType.Tab:
                        line += '\t';
                        break;
                    case TextWordType.Space:
                        line += ' ';
                        break;
                    default:
                        line += word.Word;
                        break;
                }
            }
            return line;
        }

        internal void RemoveHighlight(int line, string tag)
		{
			editorBox.Document.MarkerStrategy.RemoveAll(marker => editorBox.Document.GetLineNumberForOffset(marker.Offset) == line - 1 && tag == marker.Tag);
			UpdateDocument(line);
		}

		private void AddMarkers(IEnumerable<TextMarker> markers)
		{
			foreach (TextMarker marker in markers)
			{
				editorBox.Document.MarkerStrategy.AddMarker(marker);
			}
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
			Refresh();
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
            var caret = editorBox.ActiveTextAreaControl.Caret;
            var segment = editorBox.Document.GetLineSegment(caret.Line);
		    var word = segment.GetWord(caret.Column);
            return word == null ? string.Empty : word.Word;
		}

	    public void ShowFindForm(Form owner, SearchMode mode)
	    {
	        FindAndReplaceForm.Instance.ShowFor(owner, editorBox, mode);
	    }

        public override void OpenFile(string filename)
        {
            editorBox.LoadFile(filename, true, true);
            editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(filename);
            UpdateTabText();
            ClearIcons();
            LoadBreakpoints();

            base.OpenFile(filename);
        }

        public override void SaveFile()
        {
            _projectService.Project.FileModifiedExternally -= ProjectFileModifiedExternally;
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

            _projectService.Project.FileModifiedExternally += ProjectFileModifiedExternally;

            editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
            base.SaveFile();
        }

        private InsightWindow _insightWindow;

        private void ShowInsightWindow(IInsightDataProvider insightDataProvider)
        {
            if (_insightWindow == null || _insightWindow.IsDisposed)
            {
                _insightWindow = new InsightWindow(this, editorBox);
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
