using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Text;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using BreakpointEventArgs = Revsoft.TextEditor.Document.BreakpointEventArgs;
using IFileReaderService = Revsoft.Wabbitcode.Services.IFileReaderService;
using Timer = System.Windows.Forms.Timer;

namespace Revsoft.Wabbitcode
{
	/// <summary>
	/// Summary description for frmDocument.
	/// </summary>
	public partial class Editor : IClipboardOperation
	{
		#region Private Memebers
		private const int LabelsCacheSize = 100;

		private readonly Bitmap _warningBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.Warning16.png"));
		private readonly Bitmap _errorBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.error.png"));
		private readonly List<CancellationTokenSource> _queuedFiles = new List<CancellationTokenSource>();
		private CancellationTokenSource _highlightRefsCancellationTokenSource;
		private readonly MainForm _mainForm;
		private readonly IBackgroundAssemblerService _backgroundAssemblerService;
		private readonly IDockingService _dockingService;
		private readonly IDocumentService _documentService;
		private readonly IParserService _parserService;
		private readonly IProjectService _projectService;
		private readonly ISymbolService _symbolService;
        private readonly IFileReaderService _fileReaderService;
		private string _fileName;
		private readonly List<TextMarker> _codeCheckMarkers = new List<TextMarker>();
		private readonly List<IParserData> _labelsCache = new List<IParserData>(LabelsCacheSize);
		private int _stackTop;
		private bool _isUpdatingRefs;
		#endregion

		#region Properties

		private bool DocumentChanged { get; set; }

		/// <summary>
		/// Gets the full path to the file that is open in this editor
		/// </summary>
		public string FileName
		{
			get
			{
				return _fileName;
			}
			set
			{
				editorBox.FileName = value;
				Text = value;
				_fileName = value;
				TabText = Path.GetFileName(value);
				ToolTipText = value;
			}
		}

		public TextEditorControl EditorBox
		{
			get
			{
				return editorBox;
			}
		}

		private string EditorText
		{
			get
			{
				return editorBox.Text;
			}
		}

		public int TotalNumberOfLines
		{
			get { return editorBox.Document.TotalNumberOfLines; }
		}

		public bool IsIconBarVisible
		{
			set { editorBox.IsIconBarVisible = value; }
		}

		public bool ShowLineNumbers
		{
			set { editorBox.ShowLineNumbers = value; }
		}

		public bool CanSetNextStatement
		{
			set { setNextStateMenuItem.Visible = value; }
		}

		#endregion

		#region Static Events

		public delegate void EditorUpdated(object sender, EditorEventArgs e);
		public static event EditorUpdated OnEditorUpdated;

		public delegate void EditorOpened(object sender, EditorEventArgs e);
		public static event EditorOpened OnEditorOpened;

		public delegate void EditorClosed(object sender, EditorEventArgs e);
		public static event EditorClosed OnEditorClosing;

		public delegate void EditorSelectionChanged(object sender, EditorSelectionEventArgs e);
		public static event EditorSelectionChanged OnEditorSelectionChanged;

		#endregion

		public Editor(IBackgroundAssemblerService backgroundAssemblerService, IDockingService dockingService,
			IDocumentService documentService, IFileReaderService fileReaderService, IParserService parserService,
            IProjectService projectService, ISymbolService symbolService)
		{
			InitializeComponent();

			_mainForm = dockingService.MainForm;

			_backgroundAssemblerService = backgroundAssemblerService;
			_dockingService = dockingService;
			_documentService = documentService;
			_parserService = parserService;
			_projectService = projectService;
			_symbolService = symbolService;
		    _fileReaderService = fileReaderService;

			_textChangedTimer.Tick += textChangedTimer_Tick;

			editorBox.TextRenderingHint = Settings.Default.antiAlias ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
			editorBox.TextEditorProperties.MouseWheelScrollDown = !Settings.Default.inverseScrolling;
			editorBox.ShowLineNumbers = Settings.Default.lineNumbers;
			editorBox.Font = Settings.Default.editorFont;
			editorBox.LineViewerStyle = Settings.Default.lineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None;
			editorBox.ActiveTextAreaControl.TextArea.ToolTipRequest += TextArea_ToolTipRequest;
			editorBox.Document.FormattingStrategy = new AsmFormattingStrategy();

			WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
			WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnBreakpointRemoved;

			CodeCompletionKeyHandler.Attach(this, editorBox, parserService);
		}

		protected override string GetPersistString()
		{
			// Add extra information into the persist string for this document
			// so that it is available when deserialized.
			return GetType() + ";" + FileName + ";" + editorBox.ActiveTextAreaControl.HorizontalScroll.Value + ";" +
				   editorBox.ActiveTextAreaControl.VerticalScroll.Value + ";" +
				   editorBox.ActiveTextAreaControl.Caret.Column + ";" +
				   editorBox.ActiveTextAreaControl.Caret.Line;
		}

		void TextArea_ToolTipRequest(object sender, ToolTipRequestEventArgs e)
		{
			if (!e.InDocument)
			{
				return;
			}
			TextLocation loc = e.LogicalPosition;
			int offset = editorBox.Document.GetOffsetForLineNumber(loc.Line);
			string text = editorBox.Document.GetWord(offset + loc.Column);

			if (string.IsNullOrEmpty(text))
			{
				return;
			}

			string tooltip;
			try
			{
				IParserData data = _parserService.GetParserData(text, Settings.Default.caseSensitive).FirstOrDefault();
				tooltip = data == null ? _symbolService.SymbolTable.GetAddressFromLabel(text) : data.Description;
			}
			catch (Exception)
			{
				return;
			}

			if (!string.IsNullOrEmpty(tooltip))
			{
				e.ShowToolTip(tooltip);
			}
		}

		void SelectionManager_SelectionChanged(object sender, EventArgs e)
		{
			if (string.IsNullOrEmpty(editorBox.Text))
			{
				return;
			}

			_mainForm.UpdateAssembledInfo(_fileName, editorBox.ActiveTextAreaControl.Caret.Line);
			int start;
			int end;
			if (editorBox.ActiveTextAreaControl.SelectionManager.SelectionCollection.Count == 1)
			{
				end = editorBox.ActiveTextAreaControl.SelectionManager.SelectionCollection[0].EndOffset;
				start = editorBox.ActiveTextAreaControl.SelectionManager.SelectionCollection[0].Offset;
			}
			else
			{
				end = editorBox.ActiveTextAreaControl.Caret.Offset;
				start = end;
			}

			if (start == editorBox.Text.Length)
			{
				start--;
			}

			while (start >= 0 && editorBox.Text[start] != '\n')
			{
				start--;
			}

			start++;
			while (end < editorBox.Text.Length && editorBox.Text[end] != '\n')
			{
				end++;
			}

			end--;
			if (start > end)
			{
				start = end;
			}

			string codeInfoLines = editorBox.Document.GetText(start, end - start);

			if (OnEditorSelectionChanged != null)
			{
				OnEditorSelectionChanged(this, new EditorSelectionEventArgs(editorBox.Document, FileName, codeInfoLines));
			}
		}

		void Caret_PositionChanged(object sender, EventArgs e)
		{
			_mainForm.SetLineAndColStatus(editorBox.ActiveTextAreaControl.Caret.Line.ToString(CultureInfo.InvariantCulture),
					editorBox.ActiveTextAreaControl.Caret.Column.ToString(CultureInfo.InvariantCulture));
			if (editorBox.Document.TextLength == 0)
			{
				return;
			}
			editorBox.Document.MarkerStrategy.RemoveAll(marker => marker != null && marker.Tag == "Reference");
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
			if (!_isUpdatingRefs)
			{
				_isUpdatingRefs = true;
				string word = GetWord();
				_highlightRefsCancellationTokenSource = new CancellationTokenSource();
				string text = editorBox.Document.TextContent;
				Task.Factory.StartNew(() => GetHighlightReferences(word, text), _highlightRefsCancellationTokenSource.Token);
			}
			// update code info
			SelectionManager_SelectionChanged(sender, e);
		}

		private void GetHighlightReferences(string word, string text)
		{
			var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
			if (string.IsNullOrEmpty(word) || !Settings.Default.referencesHighlighter)
			{
				_isUpdatingRefs = false;
				return;
			}
			int counter = 0;
			List<TextMarker> references = new List<TextMarker>();
			while (counter < text.Length)
			{
				string possibleReference = TextUtils.GetWord(text, counter);
				if (!string.IsNullOrEmpty(possibleReference) && string.Equals(possibleReference, word, options))
				{
					references.Add(new TextMarker(counter, word.Length, TextMarkerType.SolidBlock, Color.LightGray)
					{
						Tag = "Reference"
					});
				}
				if (possibleReference != null)
				{
					counter += possibleReference.Length + 1;
				}
			}
			_mainForm.Invoke(() => AddMarkers(references));
			_isUpdatingRefs = false;
		}

		public void UpdateIcons(IEnumerable<Errors> errorsInFiles)
		{
			editorBox.ActiveTextAreaControl.TextArea.Document.IconManager.ClearIcons();
			var errorWarnings = errorsInFiles.Where(errorWarning => string.Equals(errorWarning.File, editorBox.FileName, StringComparison.OrdinalIgnoreCase));
			foreach (Errors errorWarning in errorWarnings)
			{
				Bitmap newIcon = errorWarning.IsWarning ? _warningBitmap : _errorBitmap;
				MarginIcon marginIcon = new MarginIcon(newIcon, errorWarning.LineNum - 1, errorWarning.ToolTip);
				editorBox.Document.IconManager.AddIcon(marginIcon);
			}
		}

		private void ClearIcons()
		{
			editorBox.ActiveTextAreaControl.TextArea.Document.IconManager.ClearIcons();
		}

		public void OpenFile(string filename)
		{
			FileName = filename;
			editorBox.LoadFile(filename, true, true);
			editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
			DocumentChanged = false;
			UpdateTabText();
			ClearIcons();
			LoadBreakpoints();

			if (OnEditorOpened != null)
			{
				OnEditorOpened(this, new EditorEventArgs(editorBox.Document, filename));
			}
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

		public bool SaveFileAs()
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog
			{
				DefaultExt = "asm",
				RestoreDirectory = true,
				Filter = "All Know File Types | *.asm; *.z80| Assembly Files (*.asm)|*.asm|Z80" +
						 " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
				FilterIndex = 0,
				Title = "Save File As"
			};

			if (saveFileDialog.ShowDialog() != DialogResult.OK)
			{
				return false;
			}
			if (string.IsNullOrEmpty(saveFileDialog.FileName))
			{
				return false;
			}
			FileName = saveFileDialog.FileName;
			return true;
		}

		public bool SaveFile()
		{
			if (_projectService.Project.ProjectWatcher != null)
			{
				_projectService.Project.ProjectWatcher.EnableRaisingEvents = false;
			}
			if (string.IsNullOrEmpty(FileName))
			{
				bool success = SaveFileAs();
				if (!success)
				{
					return false;
				}
			}

			if (string.IsNullOrEmpty(FileName))
			{
				return false;
			}

			bool saved = true;
			_stackTop = editorBox.Document.UndoStack.UndoItemCount;
			try
			{
				editorBox.SaveFile(FileName);
			}
			catch (Exception ex)
			{
				saved = false;
				DockingService.ShowError("Error saving the file", ex);
			}

			if (_projectService.Project.ProjectWatcher != null)
			{
				_projectService.Project.ProjectWatcher.EnableRaisingEvents = true;
			}

			editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
			TabText = Path.GetFileName(FileName);
			DocumentChanged = false;
			_mainForm.UpdateTitle();
			return saved;
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

			if (OnEditorClosing != null)
			{
				OnEditorClosing(this, new EditorEventArgs(editorBox.Document, FileName));
			}

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

		public void Undo()
		{
			editorBox.Undo();
		}

		public void Redo()
		{
			editorBox.Redo();
		}

		private void editorBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			TextAreaControl activeTextAreaControl = editorBox.ActiveTextAreaControl;
			activeTextAreaControl.Document.FormattingStrategy.FormatLine(
				activeTextAreaControl.TextArea, activeTextAreaControl.Caret.Line, 0, e.KeyChar);
		}

		protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
		{
			// TODO: fix this
			//if (keyData == Keys.F3)
			//{
			//	DockingService.FindForm.FindNext(true, false, "Text not found");
			//}

			if (MacroService.IsRecording)
			{
				MacroService.RecordKeyData(keyData);
			}
			return base.ProcessCmdKey(ref msg, keyData);
		}

		#region Breakpoints

		internal void ToggleBreakpoint()
		{
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
			ToggleBreakpoint(textArea.Caret.Position.Line);
		}

		private void ToggleBreakpoint(int lineNum)
		{
			WabbitcodeBreakpoint breakpoint = WabbitcodeBreakpointManager.Breakpoints
				.SingleOrDefault(b => b.File == FileName && b.LineNumber == lineNum);
			if (breakpoint == null)
			{
				AddBreakpoint(lineNum);
			}
			else
			{
				RemoveBreakpoint(lineNum);
			}
		}

		private void AddBreakpoint(int lineNum)
		{
			IDocument document = editorBox.Document;
			if (document.BreakpointManager.IsMarked(lineNum))
			{
				return;
			}

			document.BreakpointManager.AddMark(new Breakpoint(document, new TextLocation(0,lineNum)));
		}

		private void RemoveBreakpoint(int lineNum)
		{
			IDocument document = editorBox.Document;
			Breakpoint breakpoint = document.BreakpointManager.GetFirstMark(s => s.Anchor.LineNumber == lineNum);
			if (breakpoint != null)
			{
				document.BreakpointManager.RemoveMark(breakpoint);
			}
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
			_mainForm.SetPC(_fileName, editorBox.ActiveTextAreaControl.Caret.Line);
		}

		private void bgotoButton_Click(object sender, EventArgs e)
		{
			// no need to make a stricter regex, as we own the inputs here
			Match match = Regex.Match(bgotoButton.Text, "(?<action>.*?) (?<name>.*)");
		    string action = match.Groups["action"].Value;
			string text = match.Groups["name"].Value;
			bool isMacro = text.Last() == '(';
			if (isMacro)
			{
				text = text.Remove(text.Length - 1);
			}

			if (action == "Goto")
			{
				IList<IParserData> parserData;
				if (text.StartsWith("+") || text.StartsWith("-") || text == "_")
				{
				    int steps = text.Count(c => c == '+') - text.Count(c => c == '-');
				    if (steps > 0)
				    {
				        steps--;
				    }
				    var parserInfo = _parserService.GetParserInfo(FileName);
				    List<ILabel> reusableLabels = parserInfo.LabelsList.Where(l => l.IsReusable).ToList();
				    ILabel currentLabel = reusableLabels.FirstOrDefault(l => l.Location.Line >= editorBox.ActiveTextAreaControl.Caret.Line);
				    if (currentLabel == null)
				    {
				        return;
				    }

				    int index = reusableLabels.IndexOf(currentLabel) + steps;
                    parserData = new List<IParserData> {reusableLabels[index]};
				}
				else
				{
					parserData = _parserService.GetParserData(text, Settings.Default.caseSensitive).ToList(); 
				}
				
				if (parserData.Count == 1)
				{
					_documentService.GotoLabel(parserData.Single());
					if (!_labelsCache.Contains(parserData.Single()))
					{
						_labelsCache.Add(parserData.Single());
					}
				}
				else
				{
					_dockingService.FindResults.NewFindResults(text, _projectService.Project.ProjectName);
					foreach (IParserData data in parserData)
					{
					    string line = _fileReaderService.GetLine(data.Parent.SourceFile, data.Location.Line + 1);
						_dockingService.FindResults.AddFindResult(data.Parent.SourceFile, data.Location.Line, line);
					}
					_dockingService.FindResults.DoneSearching();
					_dockingService.ShowDockPanel(_dockingService.FindResults);
				}
			}
			else
			{
				string fileFullPath = Path.IsPathRooted(text) ? text : FileOperations.NormalizePath(FindFilePathIncludes(text));
				_documentService.GotoFile(fileFullPath);
			}
		}
		#endregion

		#region Drag and Drop

		private void editor_DragEnter(object sender, DragEventArgs e)
		{
			_mainForm.MainFormRedone_DragEnter(sender, e);
		}

		private void editor_DragDrop(object sender, DragEventArgs e)
		{
			_mainForm.MainFormRedone_DragDrop(sender, e);
		}

		private void editorBox_DragDrop(object sender, DragEventArgs e)
		{
			_mainForm.MainFormRedone_DragDrop(sender, e);
		}

		private void editorBox_DragEnter(object sender, DragEventArgs e)
		{
			_mainForm.MainFormRedone_DragEnter(sender, e);
		}

		#endregion

		internal void SelectAll()
		{
			TextLocation selectStart = new TextLocation(0, 0);
			TextLocation selectEnd = new TextLocation(editorBox.Text.Split('\n')[editorBox.Document.TotalNumberOfLines - 1].Length, editorBox.Document.TotalNumberOfLines - 1);
			editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(selectStart, selectEnd);
		}

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
			if (Settings.Default.enableFolding)
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
			_mainForm.UpdateTitle();
		}

		void textChangedTimer_Tick(object sender, EventArgs e)
		{
			UpdateAll(EditorText);
			_textChangedTimer.Enabled = false;
		}

		void UpdateAll(string editorText)
		{
			Action parseFile = () =>
			{
                _parserService.ParseFile(editorText.GetHashCode(), FileName, editorText);
				if (OnEditorUpdated != null)
				{
					OnEditorUpdated(this, new EditorEventArgs(EditorBox.Document, FileName));
				}
			};
			CancellationTokenSource cancellationSource = new CancellationTokenSource();
			_queuedFiles.Add(cancellationSource);
			Task.Factory.StartNew(parseFile, cancellationSource.Token);
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

			int offset = editorBox.ActiveTextAreaControl.Caret.Offset;
			if (TextUtils.DefaultDelimiters.Contains(editorBox.Document.GetCharAt(offset)))
			{
				offset--;
			}

			while (!TextUtils.DefaultDelimiters.Contains(editorBox.Document.GetCharAt(offset)))
			{
				offset--;
			}
			offset++;
			editorBox.Document.Replace(offset, item.Text.Length, item.Text);
		}

		private void findRefContext_Click(object sender, EventArgs e)
		{
			string word = GetWord();
			_dockingService.FindResults.NewFindResults(word, _projectService.Project.ProjectName);
			var refs = _projectService.FindAllReferences(word);
			foreach (var fileRef in refs.SelectMany(reference => reference))
			{
				_dockingService.FindResults.AddFindResult(fileRef);
			}
			_dockingService.FindResults.DoneSearching();
			_dockingService.ShowDockPanel(_dockingService.FindResults);
		}

		private void renameContext_Click(object sender, EventArgs e)
		{
			_mainForm.ShowRefactorForm();
		}

		private void extractMethodContext_Click(object sender, EventArgs e)
		{

		}

		private void editorBox_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Right)
			{
				return;
			}

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

				string[] lines = EditorText.Split('\n');
				string line = lines[lineNum];
				Match match = Regex.Match(line, "#include ((\"(?<includeFile>.*)(?<paren>\\)?)\")|((?!\")(?<includeFile>.*(?!\"))(?<paren>\\)?)))");
				bool isInclude = match.Success;

				string text = editorBox.Document.GetWord(editorBox.ActiveTextAreaControl.Caret.Offset);
				if (!string.IsNullOrEmpty(text) && !isInclude)
				{
					IEnumerable<IParserData> parserData =  _parserService.GetParserData(text, Settings.Default.caseSensitive).ToList();
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
				bool isMacro = !string.IsNullOrEmpty(match.Groups["paren"].Value);
				if (isMacro)
				{
					gotoLabel += "(";
				}

				if (gotoLabel == "_")
				{
					match = Regex.Match(line, "(?<offset>(\\+|\\-)*)_");
					gotoLabel = match.Groups["offset"].Value + gotoLabel;
				}
				int num;
				if ("afbcdehlixiypcspnzncpm".IndexOf(gotoLabel) == -1 && !int.TryParse(gotoLabel, out num))
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
			contextMenu.Show(editorBox, editorBox.PointToClient(MousePosition));
		}

		private bool FindFileIncludes(string gotoLabel)
		{
			return !string.IsNullOrEmpty(FindFilePathIncludes(gotoLabel));
		}

		private string FindFilePathIncludes(string gotoLabel)
		{
			IEnumerable<string> includeDirs = _projectService.Project.IsInternal ?
				Settings.Default.includeDirs.Cast<string>() :
				_projectService.Project.IncludeDirs;

			foreach (string dir in includeDirs.Where(dir => File.Exists(Path.Combine(dir, gotoLabel))))
			{
				return Path.Combine(dir, gotoLabel);
			}

			if (string.IsNullOrEmpty(FileName))
			{
				return null;
			}
			string dirPath = Path.GetDirectoryName(FileName);
			if (string.IsNullOrEmpty(dirPath))
			{
				return null;
			}

			string filePath = Path.Combine(dirPath, gotoLabel);
			return File.Exists(filePath) ? filePath : null;
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
			Process explorer = new Process
			{
				StartInfo = { FileName = Path.GetDirectoryName(FileName) }
			};
			explorer.Start();
		}

		#endregion

		#region IClipboardOperation

		public void Cut()
		{
			editorBox.Cut();
		}

		public void Copy()
		{
			editorBox.Copy();
		}

		public void Paste()
		{
			editorBox.Paste();
		}

		#endregion

		internal void ScrollToLine(int scrollToLine)
		{
			editorBox.ActiveTextAreaControl.ScrollTo(scrollToLine);
		}

		internal void ScrollToOffset(int offset)
		{
			int line = editorBox.Document.GetLineNumberForOffset(offset);
			editorBox.ActiveTextAreaControl.ScrollTo(line);
			editorBox.ActiveTextAreaControl.Caret.Line = line;
			editorBox.ActiveTextAreaControl.Caret.Column = 0;
		}

		internal void FixError(int line, DocumentService.FixableErrorType type)
		{
			int offset = editorBox.Document.GetOffsetForLineNumber(line - 1);
			int endline = offset;
			while (endline < editorBox.Document.TextLength && editorBox.Document.GetCharAt(endline) != '\n')
				endline++;
			string lineContent = editorBox.Document.GetText(offset, endline - offset);
			switch (type)
			{
				case DocumentService.FixableErrorType.RelativeJump:
					lineContent = lineContent.Replace("jr", "jp");
					int scroll = editorBox.ActiveTextAreaControl.VScrollBar.Value;
					editorBox.Document.Remove(offset, endline - offset);
					editorBox.Document.Insert(offset, lineContent);
					editorBox.ActiveTextAreaControl.VScrollBar.Value = scroll;
					break;
			}
		}

		internal void SetHighlighting(string highlightString)
		{
			editorBox.SetHighlighting(highlightString);
		}

		internal void SetCaretPosition(int caretCol, int caretLine)
		{
			editorBox.ActiveTextAreaControl.Caret.Column = caretCol;
			editorBox.ActiveTextAreaControl.Caret.Line = caretLine;
		}

		internal void SetPosition(int horzVal, int vertVal, int caretCol, int caretLine)
		{
			editorBox.ActiveTextAreaControl.HorizontalScroll.Value = horzVal;
			editorBox.ActiveTextAreaControl.VerticalScroll.Value = vertVal;
			editorBox.ActiveTextAreaControl.Caret.Column = caretCol;
			editorBox.ActiveTextAreaControl.Caret.Line = caretLine;
		}

		#region Modify Selected Text

		internal void SelectedTextToLower()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string newText = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText.ToLower();
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
			editorBox.Document.UndoStack.EndUndoGroup();
		}

		internal void SelectedTextToUpper()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string newText = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText.ToUpper();
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
			editorBox.Document.UndoStack.EndUndoGroup();
		}

		internal void SelectedTextInvertCase()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string text = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText;
			char[] textarray = text.ToCharArray();
			for (int i = 0; i < textarray.Length; i++)
			{
				if (textarray[i] >= 65 && textarray[i] <= 90)
					textarray[i] = (char)(textarray[i] + 32);
				else if (textarray[i] >= 97 && textarray[i] <= 122)
					textarray[i] = (char)(textarray[i] - 32);
			}
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(new string(textarray));
			editorBox.Document.UndoStack.EndUndoGroup();
		}

		internal void SelectedTextToSentenceCase()
		{
			editorBox.Document.UndoStack.StartUndoGroup();
			string text = editorBox.ActiveTextAreaControl.SelectionManager.SelectedText;
			string newText = CultureInfo.CurrentCulture.TextInfo.ToTitleCase(text);
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
			editorBox.Document.UndoStack.EndUndoGroup();
		}

		internal void FormatLines()
		{
			string[] lines = editorBox.Text.Split('\n');
			const string indent = "\t";
			string currentIndent = indent;
			for (int i = 0; i < lines.Length; i++)
			{
				string line = lines[i];
				string comment = "";
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
				newText.AppendLine(line);
			editorBox.Document.TextContent = newText.ToString();
		}

		#endregion

		internal void UpdateOptions(TempSettings settings)
		{
			editorBox.TextEditorProperties.MouseWheelScrollDown = !settings.inverseScrolling;
			if (settings.enableFolding)
			{
				editorBox.Document.FoldingManager.FoldingStrategy = new RegionFoldingStrategy();
				editorBox.Document.FoldingManager.UpdateFoldings(null, null);
			}
			else
			{
				editorBox.Document.FoldingManager.FoldingStrategy = null;
				editorBox.Document.FoldingManager.UpdateFoldings(new List<FoldMarker>());
			}
			editorBox.IndentStyle = settings.autoIndent ? IndentStyle.Smart : IndentStyle.None;
			editorBox.TextRenderingHint = settings.antiAlias ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
			editorBox.Font = settings.editorFont;
			editorBox.TabIndent = Settings.Default.tabSize;
			editorBox.ConvertTabsToSpaces = Settings.Default.convertTabs;
		}

		internal void RemoveInvisibleMarkers()
		{
			editorBox.Document.MarkerStrategy.RemoveAll(match => match.TextMarkerType == TextMarkerType.Invisible);
		}

		#region Bookmarks

		internal void GotoNextBookmark()
		{
			GotoNextBookmark next = new GotoNextBookmark(bookmark => true);
			next.Execute(editorBox.ActiveTextAreaControl.TextArea);
		}

		internal void GotoPrevBookmark()
		{
			GotoPrevBookmark next = new GotoPrevBookmark(bookmark => true);
			next.Execute(editorBox.ActiveTextAreaControl.TextArea);
		}

		internal void ToggleBookmark()
		{
			ToggleBookmark toggle = new ToggleBookmark();
			toggle.Execute(editorBox.ActiveTextAreaControl.TextArea);
		}

		#endregion

		internal bool Find(string textToFind)
		{
			int startOffset = editorBox.ActiveTextAreaControl.Caret.Offset + textToFind.Length;
			if (startOffset > editorBox.Text.Length)
			{
				startOffset = 0;
			}
			int newOffset = editorBox.Text.IndexOf(textToFind, startOffset);
			if (newOffset == -1)
			{
				return false;
			}
			int line = editorBox.Document.GetLineNumberForOffset(newOffset);
			editorBox.ActiveTextAreaControl.Caret.Line = line;
			int col = editorBox.Text.Split('\n')[line].IndexOf(textToFind);
			editorBox.ActiveTextAreaControl.Caret.Column = textToFind.Length + col;
			TextLocation start = new TextLocation(col, line);
			TextLocation end = new TextLocation(textToFind.Length + col, line);
			editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(start, end);
			editorBox.ActiveTextAreaControl.ScrollTo(line);
			return true;
		}

		private void UpdateDocument(int line)
		{
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, line));
		}

		private static void AddSquiggleLine(Editor doc, int newLineNumber, Color underlineColor, string description)
		{
			// TODO: regex this
			var document = doc.EditorBox.Document;
			int start = document.GetOffsetForLineNumber(newLineNumber - 1);
			int length;
			string text;
			if (description.Contains("Can't") || description.Contains("isn't") || description.Contains("Could not"))
			{
				int quote1 = description.IndexOf('\'');
				if (description.Contains("Can't"))
				{
					quote1 = description.IndexOf('\'', 7);
				}
				int quote2 = description.IndexOf('\'', quote1 + 1);
				length = quote2 - quote1 - 1;
				text = description.Substring(quote1 + 1, length);
				start = document.TextContent.IndexOf(text, start);
			}
			else if (description.Contains("No such file or directory"))
			{
				text = description.Substring(0, description.IndexOf(':', 4));
				length = text.Length;
				start = document.TextContent.IndexOf(text, start);
			}
			else
			{
				length = doc.EditorBox.Text.Split('\n')[newLineNumber - 1].Length - 1;
				while (document.TextContent[start] == ' ' || document.TextContent[start] == '\t')
				{
					start++;
					length--;
				}
			}
			TextMarker highlight = new TextMarker(start, length, TextMarkerType.WaveLine, underlineColor)
			{
				ToolTip = description,
				Tag = "Code Check"
			};
			doc.EditorBox.Document.MarkerStrategy.AddMarker(highlight);
		}

		internal void AddSquiggleLine(int newLineNumber, Color underlineColor, string description)
		{
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
			editorBox.ActiveTextAreaControl.ScrollTo(newLineNumber - 1);
			editorBox.ActiveTextAreaControl.Caret.Line = newLineNumber - 1;
			int start = textArea.Caret.Offset;
			int length = editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
			while (start > 0 && textArea.Document.TextContent[start] != '\n')
			{
				start--;
			}

			start++;
			length--;
			while (char.IsWhiteSpace(textArea.Document.GetCharAt(start)))
			{
				start++;
				length--;
			}
			TextMarker highlight = new TextMarker(start, length, TextMarkerType.WaveLine, underlineColor)
			{
				ToolTip = description,
				Tag = "Code Check"
			};
			editorBox.Document.MarkerStrategy.AddMarker(highlight);
		}

		internal void HighlightLine(int newLineNumber, Color foregroundColor)
		{
			int lineNum = newLineNumber - 1;
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;

		    LineSegment segment = textArea.Document.GetLineSegment(lineNum);
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

		    Regex highlightRegex = new Regex(@"^\s*(?<line>[\w|\s|,|\(|\)|:|\*|/|\+|\-|\$|\%|'|\\]*?)\s*(;.*)?$", RegexOptions.Compiled);
		    Match match = highlightRegex.Match(line);
            Group group = match.Groups["line"];
		    int start = group.Index + segment.Offset;
            int length = group.Length;
            if (length == 0)
            {
                return;
            }

		    TextMarker highlight = new TextMarker(start, length, TextMarkerType.SolidBlock, foregroundColor, Color.Black)
		    {
		        Tag = FileName
		    };
		    editorBox.Document.MarkerStrategy.AddMarker(highlight);
		    editorBox.Refresh();
		    ScrollToLine(lineNum);
		}

		internal void HighlightCall(int lineNumber)
		{
			string line = editorBox.Document.TextContent.Split('\n')[lineNumber - 1];
			if (line.Contains(';'))
			{
				line = line.Remove(line.IndexOf(';'));
			}
			if (line.Contains("call") || line.Contains("bcall") || line.Contains("b_call") || line.Contains("rst"))
			{
				_mainForm.AddStackEntry(lineNumber);
				//DocumentService.HighlightLine(lineNumber, Color.Green);
			}
			//if (line.Contains("ret"))
			//	Debugger.Instance.StepStack.Pop();
		}

		internal void RemoveHighlight(int line)
		{
			editorBox.Document.MarkerStrategy.RemoveAll(marker => editorBox.Document.GetLineNumberForOffset(marker.Offset) == line - 1);
			UpdateDocument(line);
		}

		internal void RemoveDebugHighlight(int line)
		{
			editorBox.Document.MarkerStrategy.RemoveAll(marker => editorBox.Document.GetLineNumberForOffset(marker.Offset) == line - 1 && marker.Color == Color.Yellow);
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

		internal void ConvertSpacesToTabs()
		{
			StringBuilder spacesString = new StringBuilder();
			for (int i = 0; i < Settings.Default.tabSize; i++)
			{
				spacesString.Append(' ');
			}
			editorBox.Document.TextContent = editorBox.Document.TextContent.Replace(spacesString.ToString(), "\t");
		}

		internal string GetWord()
		{
			int offset = editorBox.ActiveTextAreaControl.Caret.Offset;
			return editorBox.Document.GetWord(offset);
		}
	}

	public class FoldingItem
	{
		readonly int _offset;
		public int Offset
		{
			get { return _offset; }
		}

		readonly string _text;
		public string Text
		{
			get { return _text; }
		}
		public FoldingItem(int offset, string text)
		{
			_offset = offset;
			_text = text;
		}
	}
}
