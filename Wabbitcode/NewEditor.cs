using System.Globalization;
using System.Threading.Tasks;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Parser;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Utils;
using BreakpointEventArgs = Revsoft.TextEditor.Document.BreakpointEventArgs;
using Timer = System.Windows.Forms.Timer;

namespace Revsoft.Wabbitcode
{
	/// <summary>
	/// Summary description for frmDocument.
	/// </summary>
	public partial class NewEditor
	{
		#region Private Memebers
		private const int LabelsCacheSize = 100;

		private readonly Bitmap _warningBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.Warning16.png"));
		private readonly Bitmap _errorBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.error.png"));
		private readonly List<CancellationTokenSource> _queuedFiles = new List<CancellationTokenSource>();
		private CancellationTokenSource _highlightRefsCancellationTokenSource;
		private readonly MainFormRedone _mainForm;
		private readonly IDockingService _dockingService;
		private readonly IBackgroundAssemblerService _backgroundAssemblerService;
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
		public static event EditorClosed OnEditorClosed;

		#endregion

		public NewEditor(IDockingService dockingService, IBackgroundAssemblerService backgroundAssemblerService)
		{
			InitializeComponent();

			_mainForm = dockingService.MainForm;
			_mainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;

			_backgroundAssemblerService = backgroundAssemblerService;
			_dockingService = dockingService;

			_textChangedTimer.Tick += textChangedTimer_Tick;

			editorBox.TextRenderingHint = Settings.Default.antiAlias ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
			editorBox.TextEditorProperties.MouseWheelScrollDown = !Settings.Default.inverseScrolling;
			editorBox.ShowLineNumbers = Settings.Default.lineNumbers;
			editorBox.Font = Settings.Default.editorFont;
			editorBox.LineViewerStyle = Settings.Default.lineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None;
			editorBox.ActiveTextAreaControl.TextArea.ToolTipRequest += TextArea_ToolTipRequest;
			editorBox.Document.FormattingStrategy = new AsmFormattingStrategy();

			CodeCompletionKeyHandler.Attach(this, editorBox);
		}

		void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
		{

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
			if (!Settings.Default.caseSensitive)
			{
				text = text.ToUpper();
			}

			string tooltip;
			try
			{
				tooltip = _mainForm.TranlateSymbolToAddress(text);
			}
			catch (Exception)
			{
				return;
			}
			e.ShowToolTip(tooltip);

		}

		void BreakpointManager_Removed(object sender, BreakpointEventArgs e)
		{
			WabbitcodeBreakpointManager.RemoveBreakpoint(FileName, e.Breakpoint);
		}

		void BreakpointManager_Added(object sender, BreakpointEventArgs e)
		{
			WabbitcodeBreakpointManager.AddBreakpoint(FileName, e.Breakpoint);
		}

		void SelectionManager_SelectionChanged(object sender, EventArgs e)
		{
			if (!string.IsNullOrEmpty(editorBox.Text))
			{
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
				//_codeLinesCancellationSource = new CancellationTokenSource();
				//Task.Factory.StartNew(() => GetCodeInfo(_codeInfoLines), _codeLinesCancellationSource.Token);
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
			//update code info
			SelectionManager_SelectionChanged(sender, e);
		}

		private void GetHighlightReferences(string word, string text)
		{
			try
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
			}
			catch (Exception)
			{ }
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

			if (OnEditorOpened != null)
			{
				OnEditorOpened(this, new EditorEventArgs(editorBox.Document));	
			}
		}

		public bool SaveFile()
		{
			DocumentService.InternalSave = true;
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
			editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
			TabText = Path.GetFileName(FileName);
			DocumentChanged = false;
			_mainForm.UpdateTitle();
			DocumentService.InternalSave = false;
			return saved;
		}

		private void newEditor_FormClosing(object sender, CancelEventArgs e)
		{
			_textChangedTimer.Enabled = false;

			editorBox.Document.MarkerStrategy.RemoveAll(s => true);

			// Cancel any tasks
			foreach (CancellationTokenSource item in _queuedFiles)
			{
				item.Cancel();
			}

			if (OnEditorClosed != null)
			{
				OnEditorClosed(this, new EditorEventArgs(null));
			}

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
					if (string.IsNullOrEmpty(FileName))
					{
						DocumentService.SaveDocument(this);
					}
					else
					{
						SaveFile();
					}
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
				ParserService parserService = new ParserService();
				parserService.ParseFile(editorText.GetHashCode(), FileName, editorText);
				if (OnEditorUpdated != null)
				{
					OnEditorUpdated(this, new EditorEventArgs(EditorBox.Document));
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

		private void newEditor_DragEnter(object sender, DragEventArgs e)
		{
			_mainForm.MainFormRedone_DragEnter(sender, e);
		}

		private void newEditor_DragDrop(object sender, DragEventArgs e)
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

		//private void bgotoButton_Click(object sender, EventArgs e)
		//{
		//	string text = bgotoButton.Text.Substring(5, bgotoButton.Text.Length - 5);
		//	bool isMacro = text[text.Length - 1] == '(';
		//	if (isMacro)
		//		text = text.Remove(text.Length - 1);
		//	if (bgotoButton.Text.Substring(0, 4) == "Goto")
		//	{
		//		List<IParserData> parserData = new List<IParserData>();
		//		if (text[0] == '+' || text[0] == '-' || text == "_")
		//		{
		//			bool negate = text[0] == '-';
		//			int steps = text == "_" ? 1 : 0;
		//			while (text != "_")
		//			{
		//				text = text.Remove(0, 1);
		//				steps++;
		//			}
		//			if (negate)
		//			{
		//				steps *= -1;
		//			}
		//			int i;
		//			for (i = 0; i < _parseInfo.LabelsList.Count; i++)
		//			{
		//				if (_parseInfo.LabelsList[i].Name == "_")
		//				{
		//					parserData.Add(_parseInfo.LabelsList[i]);
		//				}
		//			}
		//			i = 0;
		//			while (i < parserData.Count && parserData[i].Location.Offset < editorBox.ActiveTextAreaControl.Caret.Offset)
		//			{
		//				i++;
		//			}
		//			if (negate)
		//			{
		//				i += steps;
		//			}
		//			else
		//			{
		//				i += steps - 1;
		//			}
		//			IParserData data = parserData[i];
		//			parserData.Clear();
		//			parserData.Add(data);
		//		}
		//		else
		//		{
		//			var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
		//			foreach (ParserInformation info in ProjectService.ParserInfomInformation)
		//			{
		//				foreach (IParserData data in info.GeneratedList)
		//				{
		//					if (string.Equals(data.Name, text, options))
		//					{
		//						parserData.Add(data);
		//						break;
		//					}
		//				}
		//			}
		//		}
		//		if (parserData.Count == 0)
		//		{
		//			MessageBox.Show("Unable to locate " + text);
		//		}
		//		else if (parserData.Count == 1)
		//		{
		//			DocumentService.GotoLabel(parserData[0]);
		//			if (!_labelsCache.Contains(parserData[0]))
		//			{
		//				_labelsCache.Add(parserData[0]);
		//			}
		//		}
		//		else
		//		{
		//			DockingService.FindResults.NewFindResults(text, _projectService.ProjectName);
		//			foreach (IParserData data in parserData)
		//			{
		//				string line;
		//				StreamReader reader = null;
		//				try
		//				{
		//					reader = new StreamReader(data.Parent.SourceFile);
		//					line = reader.ReadToEnd().Split('\n')[data.Location.Line];
		//				}
		//				finally
		//				{
		//					if (reader != null)
		//					{
		//						reader.Close();
		//					}
		//				}
		//				_dockingService.FindResults.AddFindResult(data.Parent.SourceFile, data.Location.Line, line);
		//			}
		//			DockingService.ShowDockPanel(DockingService.FindResults);
		//		}
		//	}
		//	else
		//	{
		//		DocumentService.GotoFile(Path.IsPathRooted(text) ? text : FileOperations.NormalizePath(FindFilePathIncludes(text)));
		//	}
		//}

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

		//private void findRefContext_Click(object sender, EventArgs e)
		//{
		//	string word = GetWord();
		//	DockingService.FindResults.NewFindResults(word, ProjectService.ProjectName);
		//	var refs = ProjectService.FindAllReferences(word);
		//	foreach (var fileRef in refs.SelectMany(reference => reference))
		//	{
		//		DockingService.FindResults.AddFindResult(fileRef);
		//	}
		//	DockingService.FindResults.DoneSearching();
		//	DockingService.ShowDockPanel(DockingService.FindResults);
		//}

		//private void renameContext_Click(object sender, EventArgs e)
		//{
		//	RefactorForm form = new RefactorForm();
		//	form.ShowDialog();
		//}

		private void extractMethodContext_Click(object sender, EventArgs e)
		{

		}

		private void editorBox_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Right)
			{
				return;
			}

			//bool hasSelection = editorBox.ActiveTextAreaControl.SelectionManager.HasSomethingSelected;
			//cutContext.Enabled = hasSelection;
			//copyContext.Enabled = hasSelection;
			//if (!string.IsNullOrEmpty(editorBox.Text))
			//{
			//	var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
			//	int offset = editorBox.ActiveTextAreaControl.Caret.Offset;
			//	fixCaseContext.Visible = false;
			//	string text = editorBox.Document.GetWord(offset);
			//	if (!string.IsNullOrEmpty(text))
			//	{
			//		List<IParserData> parserData = new List<IParserData>();
			//		foreach (ParserInformation info in ProjectService.ParserInfomInformation)
			//		{
			//			parserData.AddRange(info.GeneratedList.Where(d => string.Equals(d.Name, text, options)));
			//		}
			//		if (parserData.Count > 0)
			//		{
			//			fixCaseContext.MenuItems.Clear();
			//			foreach (IParserData data in parserData)
			//			{
			//				// check if they already the same case
			//				if (data.Name == text)
			//				{
			//					continue;
			//				}
			//				fixCaseContext.Visible = true;
			//				MenuItem item = new MenuItem(data.Name, fixCaseContext_Click);
			//				fixCaseContext.MenuItems.Add(item);
			//			}
			//			bgotoButton.Enabled = true;
			//		}
			//		else
			//		{
			//			bgotoButton.Enabled = false;
			//		}
			//	}
			//	//if the user clicked after the last char we need to catch this
			//	if (offset == editorBox.Text.Length)
			//	{
			//		offset--;
			//	}

			//	int startLine = offset;
			//	while (startLine >= 0 && editorBox.Text[startLine] != '\n')
			//	{
			//		startLine--;
			//	}

			//	startLine++;
			//	bool isInclude = false;
			//	if (string.Compare(editorBox.Text, startLine, "#include", 0, "#include".Length, options) == 0)
			//	{
			//		offset = startLine + "#include".Length;
			//		isInclude = true;
			//	}
			//	else
			//	{
			//		while (offset >= 0 && (char.IsLetterOrDigit(editorBox.Text[offset]) || editorBox.Text[offset] == '_'))
			//		{
			//			offset--;
			//		}
			//		offset++;
			//	}

			//	int length = 1;
			//	if (isInclude)
			//	{
			//		while (char.IsWhiteSpace(editorBox.Text[offset]))
			//		{
			//			offset++;
			//		}
			//		while (offset + length < editorBox.Text.Length && (editorBox.Text[offset + length] != ';' && editorBox.Text[offset + length] != '\"' && editorBox.Text[offset + length] != '\r' && editorBox.Text[offset + length] != '\n'))
			//		{
			//			length++;
			//		}
			//		if (offset + length < editorBox.Text.Length && editorBox.Text[offset + length] == '\"')
			//		{
			//			length++;
			//		}
			//	}
			//	else
			//	{
			//		while (offset + length < editorBox.Text.Length && (char.IsLetterOrDigit(editorBox.Text[offset + length]) || editorBox.Text[offset + length] == '_'))
			//		{
			//			length++;
			//		}
			//	}

			//	string gotoLabel;
			//	try
			//	{
			//		gotoLabel = editorBox.Document.GetText(offset, length).Trim();
			//		if (offset + length < editorBox.Text.Length && editorBox.Text[offset + length] == '(')
			//		{
			//			gotoLabel += "(";
			//		}
			//		if (gotoLabel == "_")
			//		{
			//			if (editorBox.Document.TextContent[offset] == '_')
			//			{
			//				offset--;
			//			}
			//			else
			//			{
			//				while (offset > 0 && editorBox.Document.TextContent[offset] != '_')
			//				{
			//					offset--;
			//				}
			//				offset--;
			//			}
			//			while (offset > 0 && (editorBox.Document.TextContent[offset] == '-' || editorBox.Document.TextContent[offset] == '+'))
			//			{
			//				offset--; length++;
			//			}
			//			gotoLabel = editorBox.Document.GetText(++offset, length).Trim();
			//		}
			//	}
			//	catch (Exception)
			//	{
			//		throw new Exception("Error getting label");
			//	}
			//	int num;
			//	if ("afbcdehlixiypcspnzncpm".IndexOf(gotoLabel) == -1 && !int.TryParse(gotoLabel, out num))
			//	{
			//		if (isInclude)
			//		{
			//			if (gotoLabel[0] == '\"')
			//			{
			//				gotoLabel = gotoLabel.Substring(1, gotoLabel.Length - 2);
			//			}
			//			bool exists = Path.IsPathRooted(gotoLabel) ? File.Exists(gotoLabel) : FindFileIncludes(gotoLabel);
			//			if (exists)
			//			{
			//				bgotoButton.Text = "Open " + gotoLabel;
			//				bgotoButton.Enabled = true;
			//			}
			//			else
			//			{
			//				bgotoButton.Text = "File " + gotoLabel + " doesn't exist";
			//				bgotoButton.Enabled = false;
			//			}
			//		}
			//		else
			//		{
			//			if (bgotoButton.Enabled)
			//			{
			//				bgotoButton.Text = "Goto " + gotoLabel;
			//				bgotoButton.Enabled = !string.IsNullOrEmpty(gotoLabel);
			//			}
			//			else
			//			{
			//				bgotoButton.Text = "Unable to find " + gotoLabel;
			//			}
			//		}
			//	}
			//	else
			//	{
			//		bgotoButton.Text = "Goto ";
			//		bgotoButton.Enabled = false;
			//	}
			//}
			//contextMenu.Show(editorBox, editorBox.PointToClient(MousePosition));
		}

		//private bool FindFileIncludes(string gotoLabel)
		//{
		//	return !string.IsNullOrEmpty(FindFilePathIncludes(gotoLabel));
		//}

		//private string FindFilePathIncludes(string gotoLabel)
		//{
		//	IEnumerable<string> includeDirs = _projectService.Project.IsInternal ?
		//				Settings.Default.includeDirs.Split('\n').ToList() : ProjectService.IncludeDirs;
		//	foreach (string dir in includeDirs)
		//	{
		//		if (File.Exists(Path.Combine(dir, gotoLabel)))
		//		{
		//			return Path.Combine(dir, gotoLabel);
		//		}
		//	}
			
		//	if (File.Exists(Path.Combine(Path.GetDirectoryName(FileName), gotoLabel)))
		//	{
		//		return Path.Combine(Path.GetDirectoryName(FileName), gotoLabel);
		//	}
		//	return null;
		//}

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
			foreach (NewEditor child in _dockingService.Documents.Where(child => child != this))
			{
				child.Close();
			}
		}

		private void closeAllMenuItem_Click(object sender, EventArgs e)
		{
			var list = _dockingService.Documents.ToList();
			foreach (NewEditor child in list)
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

		// TODO: fix
		//private void GetCodeInfo(string lines)
		//{
		//	if (string.IsNullOrEmpty(lines))
		//	{
		//		return;
		//	}

		//	if (_infoLinesRunning)
		//	{
		//		_infoLinesQueued = true;
		//		return;
		//	}

		//	_infoLinesRunning = true;
		//	CodeCountInfo info = AssemblerService.Instance.CountCode(lines);
		//	_mainForm.Invoke(() => _mainForm.UpdateCodeInfo(info));
		//	if (_infoLinesQueued)
		//	{
		//		_infoLinesQueued = false;
		//		GetCodeInfo(lines);
		//	}
		//	_infoLinesRunning = false;
		//}

		private void setNextStateMenuItem_Click(object sender, EventArgs e)
		{
			_mainForm.SetPC(_fileName, editorBox.ActiveTextAreaControl.Caret.Line);
		}

		internal void Cut()
		{
			editorBox.Cut();
		}

		internal void Copy()
		{
			editorBox.Copy();
		}

		internal void Paste()
		{
			editorBox.Paste();
		}

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

		internal void ToggleBreakpoint()
		{
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
			IEditAction newBreakpoint = new ToggleBreakpoint(textArea.Caret.Position);
			newBreakpoint.Execute(textArea);
		}

		internal void ToggleBreakpoint(int lineNum)
		{
			IEditAction newBreakpoint = new ToggleBreakpoint(new TextLocation(0, lineNum));
			newBreakpoint.Execute(editorBox.ActiveTextAreaControl.TextArea);
		}

		internal void RemoveBreakpoint(int lineNum)
		{
			Breakpoint breakpoint = editorBox.Document.BreakpointManager.GetNextMark(lineNum);
			editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
		}

		internal void ClearBreakpoints()
		{
			editorBox.Document.BreakpointManager.Clear();
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

		private static void AddSquiggleLine(NewEditor doc, int newLineNumber, Color underlineColor, string description)
		{
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

			int start = textArea.Document.GetOffsetForLineNumber(lineNum);
			start = TextUtils.SkipWhitespace(editorBox.Text, start);
			int end = TextUtils.SkipToEndOfCodeLine(editorBox.Text, start, ParserService.EndOfLineChar, ParserService.CommentChar);
			// backtrack whitespace
			if (TextUtils.IsValidIndex(end, editorBox.Text.Length) && editorBox.Text[end] == ParserService.CommentChar)
			{
				end--;
				while (TextUtils.IsValidIndex(end, editorBox.Text.Length) && char.IsWhiteSpace(editorBox.Text[end]))
				{
					end--;
				}
				if (TextUtils.IsValidIndex(end, editorBox.Text.Length) && !char.IsWhiteSpace(editorBox.Text[end]))
				{
					end++;
				}
			}

			TextMarker highlight = new TextMarker(start, end - start, TextMarkerType.SolidBlock, foregroundColor, Color.Black)
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
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, line));
		}

		internal void RemoveDebugHighlight(int line)
		{
			editorBox.Document.MarkerStrategy.RemoveAll(marker => editorBox.Document.GetLineNumberForOffset(marker.Offset) == line - 1 && marker.Color == Color.Yellow);
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, line));
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
				spacesString.Append(' ');
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
