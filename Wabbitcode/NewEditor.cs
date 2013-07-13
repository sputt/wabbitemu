using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.Collections;
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
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

namespace Revsoft.Wabbitcode
{
	/// <summary>
	/// Summary description for frmDocument.
	/// </summary>
	public partial class NewEditor
	{
		#region Private Memebers
		private Bitmap warningBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.Warning16.png"));
		private Bitmap errorBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.error.png"));
		private List<CancellationTokenSource> queuedFiles = new List<CancellationTokenSource>();
		CancellationTokenSource codeCheckerCancellationSource;
		CancellationTokenSource codeLinesCancellationSource;
		private MainFormRedone mainForm;
		private bool docChanged;
		private string fileName;
		private List<TextMarker> codeCheckMarkers = new List<TextMarker>();
		#endregion

		#region Properties

		public bool DocumentChanged
		{
			get
			{
				return docChanged;
			}
		}

		public string FileName
		{
			get
			{
				return fileName;
			}
			set 
			{ 
				editorBox.FileName = value;
				Text = value;
				fileName = value;
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

		public string EditorText
		{
			get
			{
				return editorBox.Text;
			}
		}

		public List<Breakpoint> Breakpoints
		{
			get
			{
				return editorBox.Document.BreakpointManager.Marks.ToList();
			}
		}

		public int TotalNumberOfLines
		{
			get { return editorBox.Document.TotalNumberOfLines; }
		}

		public bool IsIconBarVisible
		{
			get { return editorBox.IsIconBarVisible; }
			set { editorBox.IsIconBarVisible = value; }
		}

		public bool ShowLineNumbers
		{
			get { return editorBox.ShowLineNumbers; }
			set { editorBox.ShowLineNumbers = value; }
		}

		public bool CanSetNextStatement
		{
			get { return setNextStateMenuItem.Visible; }
			set { setNextStateMenuItem.Visible = value; }
		}

		#endregion

		bool hasInited;
		public NewEditor(MainFormRedone mainForm)
		{
			InitializeComponent();

			this.mainForm = mainForm;
			this.mainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;

			this.hasInited = true;
			textChangedTimer.Tick += new EventHandler(textChangedTimer_Tick);
			
			if (Settings.Default.antiAlias)
			{
				editorBox.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
			}
			else
			{
				editorBox.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;
			}

			editorBox.TextEditorProperties.MouseWheelScrollDown = !Settings.Default.inverseScrolling;
			editorBox.ShowLineNumbers = Settings.Default.lineNumbers;
			editorBox.Font = Settings.Default.editorFont;
			editorBox.LineViewerStyle = Settings.Default.lineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None;
			editorBox.ActiveTextAreaControl.TextArea.ToolTipRequest += new ToolTipRequestEventHandler(TextArea_ToolTipRequest);
			editorBox.Document.FormattingStrategy = new Extensions.AsmFormattingStrategy();
			codeChecker.DoWork += new DoWorkEventHandler((sender, e) =>
			{
				codeCheckMarkers.Clear();
				if (ProjectService.IsInternal)
				{
				}
				else
				{
					AssemblerService.Instance.AssemblerProjectFinished += codeCheckAssemblerFinished;
					codeCheckerCancellationSource = new CancellationTokenSource();
					//Task.Factory.StartNew(() => AssemblerService.Instance.AssembleProject(ProjectService.Project), codeCheckerCancellationSource.Token);
				}
				runningAssembly = false;
			});
			
			CodeCompletionKeyHandler.Attach(this, editorBox);
		}

		void mainForm_OnDebuggingStarted(object sender, Services.Debugger.DebuggingEventArgs e)
		{
			
		}

		protected override string GetPersistString()
		{
			// Add extra information into the persist string for this document
			// so that it is available when deserialized.
			return GetType() +  ";" + FileName + ";" + editorBox.ActiveTextAreaControl.HorizontalScroll.Value + ";" +
				   editorBox.ActiveTextAreaControl.VerticalScroll.Value + ";" +
				   editorBox.ActiveTextAreaControl.Caret.Column + ";" +
				   editorBox.ActiveTextAreaControl.Caret.Line;
		}

		void TextArea_ToolTipRequest(object sender, TextEditor.ToolTipRequestEventArgs e)
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
			e.ShowToolTip(mainForm.TranlateSymbolToAddress(text));

		}

		void BreakpointManager_Removed(object sender, BreakpointEventArgs e)
		{
			if (e.Breakpoint.Anchor.IsDeleted == false)
			{
				mainForm.RemoveBreakpoint(e.Breakpoint.LineNumber, FileName);
			}
			if (DockingService.BreakManager != null)
			{
				DockingService.BreakManager.UpdateManager();
			}
		}

		void BreakpointManager_Added(object sender, BreakpointEventArgs e)
		{
			mainForm.AddBreakpoint(e.Breakpoint.LineNumber, FileName);
			if (DockingService.BreakManager != null)
			{
				DockingService.BreakManager.UpdateManager();
			}
		}

		string codeInfoLines = String.Empty;
		void SelectionManager_SelectionChanged(object sender, EventArgs e)
		{
			if (!string.IsNullOrEmpty(editorBox.Text))
			{
				mainForm.UpdateAssembledInfo(fileName, editorBox.ActiveTextAreaControl.Caret.Line);
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
				codeInfoLines = editorBox.Document.GetText(start, end - start);
				codeLinesCancellationSource = new CancellationTokenSource();
				//Task.Factory.StartNew(() => GetCodeInfo(codeInfoLines), codeLinesCancellationSource.Token);
				infoLinesQueued = true;
			}
		}

		static bool infoLinesQueued = false;
		static bool infoLinesRunning = false;
		static bool isUpdatingRefs = false;
		void Caret_PositionChanged(object sender, EventArgs e)
		{
			mainForm.SetLineAndColStatus(editorBox.ActiveTextAreaControl.Caret.Line.ToString(),
														editorBox.ActiveTextAreaControl.Caret.Column.ToString());
			if (editorBox.Document.TextLength == 0)
			{
				return;
			}
			editorBox.Document.MarkerStrategy.RemoveAll(marker => marker != null && marker.Tag == "Reference");
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
			if (!isUpdatingRefs)
			{
				isUpdatingRefs = true;
				int offset = editorBox.ActiveTextAreaControl.Caret.Offset;
				string word = GetWord();
				ThreadPool.QueueUserWorkItem(new WaitCallback(GetHighlightReferences), new ReferencesHighlightData(offset, word, editorBox.Document.TextContent));
			}
			//update code info
			SelectionManager_SelectionChanged(sender, e);
		}
		class ReferencesHighlightData {
			public int Offset { get; private set; }
			public string Word { get; private set; }
			public string Text { get; private set; }
			public ReferencesHighlightData(int offset, string word, string text)
			{
				Offset = offset;
				Word = word;
				Text = text;
			}
		}

		private void GetHighlightReferences(object data)
		{
			try
			{
				ReferencesHighlightData highlightData = data as ReferencesHighlightData;
				int offset = highlightData.Offset;
				string word = highlightData.Word;
				string text = highlightData.Text;
				if (offset == text.Length)
				{
					offset--;
				}
				var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
				if (string.IsNullOrEmpty(word) || !Settings.Default.referencesHighlighter)
				{
					isUpdatingRefs = false;
					return;
				}
				int counter = 0;
				string possibleReference;
				List<TextMarker> references = new List<TextMarker>();
				while (counter < text.Length)
				{
					possibleReference = GetWord(text, counter);
					if (!string.IsNullOrEmpty(possibleReference) && string.Equals(possibleReference, word, options))
					{
						references.Add(new TextMarker(counter, word.Length, TextMarkerType.SolidBlock, Color.LightGray) { Tag = "Reference" });
					}
					counter += possibleReference.Length + 1;
				}
				mainForm.Invoke(() => AddMarkers(references));
			}
			catch (Exception) { }
			isUpdatingRefs = false;
		}

		const string defaultDelimiters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";
		string GetWord(string text, int offset, string delimiters = null)
		{
			delimiters = delimiters ?? defaultDelimiters;
			if (offset >= text.Length)
			{
				return String.Empty;
			}
			int newOffset = offset;
			char test = text[offset];
			while (offset > 0 && delimiters.IndexOf(test) == -1)
			{
				test = text[--offset];
			}
			if (offset > 0)
			{
				offset++;
			}
			test = text[newOffset];
			while (newOffset + 1 < text.Length && delimiters.IndexOf(test) == -1)
			{
				test = text[++newOffset];
			}
			if (newOffset < offset)
			{
				return String.Empty;
			}
			return text.Substring(offset, newOffset - offset);
		}

		public void UpdateIcons(List<Errors> errorsInFiles)
		{
			editorBox.ActiveTextAreaControl.TextArea.Document.IconManager.ClearIcons();
			foreach (Errors errorWarning in errorsInFiles)
			{
				if (!string.Equals(errorWarning.File, editorBox.FileName, StringComparison.OrdinalIgnoreCase))
				{
					continue;
				}
				Bitmap newIcon;
				if (errorWarning.IsWarning)
				{
					newIcon = warningBitmap;
				}
				else
				{
					newIcon = errorBitmap;
				}
				MarginIcon marginIcon = new MarginIcon(newIcon, errorWarning.LineNum - 1, errorWarning.ToolTip);
				editorBox.Document.IconManager.AddIcon(marginIcon);
			}
		}

		private void ClearIcons()
		{
			editorBox.ActiveTextAreaControl.TextArea.Document.IconManager.ClearIcons();
		}

		private ParserInformation parseInfo;
		public void OpenFile(string filename)
		{
			FileName = filename;
			editorBox.LoadFile(filename, true, true);
			editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
			docChanged = false;
			UpdateTabText();
			ClearIcons();
			//try out projects cache of parse info
			parseInfo = ProjectService.GetParseInfo(filename);
			if (parseInfo == null)
			{
				UpdateAll(EditorText);
			}
			else
			{
				UpdateLabelBox();
			}
			if (!ProjectService.IsInternal && ProjectService.ContainsFile(filename))
			{
				string foldings = ProjectService.Project.FindFile(filename).FileFoldings;
				editorBox.Document.FoldingManager.DeserializeFromString(foldings);
			}
		}

		public bool SaveFile()
		{
			DocumentService.InternalSave = true;
			bool saved = true;
			stackTop = editorBox.Document.UndoStack.UndoItemCount;
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
			docChanged = false;
			mainForm.UpdateTitle();
			DocumentService.InternalSave = false;
			return saved;
		}

		private void newEditor_FormClosing(object sender, CancelEventArgs e)
		{
			textChangedTimer.Enabled = false;
			if (!ProjectService.IsInternal)
			{
				if (ProjectService.ContainsFile(FileName))
				{
					ProjectFile file = ProjectService.Project.FindFile(FileName);
					file.FileFoldings = editorBox.Document.FoldingManager.SerializeToString();
				}
			}

			editorBox.Document.MarkerStrategy.RemoveAll(s => true);

			// Cancel any tasks
			foreach (CancellationTokenSource item in queuedFiles)
			{
				item.Cancel();
			}
			codeCheckerCancellationSource.Cancel();
			codeLinesCancellationSource.Cancel();
			codeChecker.CancelAsync();

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
			editorBox.ActiveTextAreaControl.Document.FormattingStrategy.FormatLine(editorBox.ActiveTextAreaControl.TextArea, editorBox.ActiveTextAreaControl.Caret.Line, 0, e.KeyChar);
		}

		protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
		{
			if (keyData == Keys.F3)
			{
				DockingService.FindForm.FindNext(true, false, "Text not found");
			}
			if (MacroService.IsRecording)
			{
				MacroService.RecordKeyData(keyData);
			}
			return base.ProcessCmdKey(ref msg, keyData);
		}

		public void UpdateLabelBox()
		{
			if (parseInfo != null)
			{
				DockingService.LabelList.EnableLabelBox();
				DockingService.LabelList.AddLabels(parseInfo);
			}
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

		private static void UpdateTimerCallback(object input)
		{

		}

		private int stackTop;
		private System.Windows.Forms.Timer textChangedTimer = new System.Windows.Forms.Timer()
		{
			Interval = 1000,
			Enabled = false
		};

		private void editorBox_TextChanged(object sender, EventArgs e)
		{
			if (textChangedTimer.Enabled)
				textChangedTimer.Stop();
			textChangedTimer.Start();
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
			if (editorBox.Document.UndoStack.UndoItemCount != stackTop)
			{
				docChanged = true;
				UpdateTabText();
			}
			else
			{
				docChanged = false;
				UpdateTabText();
			}
			mainForm.UpdateTitle();
		}

		void textChangedTimer_Tick(object sender, EventArgs e)
		{
			UpdateAll(EditorText);
			textChangedTimer.Enabled = false;
		}

		private static bool runningAssembly = false;
		private static BackgroundWorker codeChecker = new System.ComponentModel.BackgroundWorker();
		void UpdateAll(string editorText)
		{
			infoLinesQueued = true;
			Action ParseFile = () =>
			{
				ParserService parserService = new ParserService();
				parseInfo = parserService.ParseFile(editorText.GetHashCode() , FileName, editorText);
				if (DockingService.HasBeenInited && string.Compare(FileName, DocumentService.ActiveFileName, true) == 0)
				{
					mainForm.Invoke(() => UpdateLabelBox());
				}
			};
			CancellationTokenSource cancellationSource = new CancellationTokenSource();
			queuedFiles.Add(cancellationSource);
			//Task.Factory.StartNew(ParseFile, cancellationSource.Token);
			if (!codeChecker.IsBusy && !runningAssembly)
			{
				runningAssembly = true;
				codeChecker.WorkerSupportsCancellation = true;
				codeChecker.RunWorkerAsync();
			}
		}

		private void UpdateTabText()
		{
			string changedString = DocumentChanged ? "*" : "";
			if (!string.IsNullOrEmpty(FileName))
			{
				TabText = Path.GetFileName(FileName) + changedString;
			}
			else
			{
				TabText = "New Document" + changedString;
			}
		}

		private string FixIncludeFiles(string fileName)
		{
			if (!Path.IsPathRooted(fileName))
			{
				fileName = Path.Combine(Path.GetDirectoryName(editorBox.FileName), fileName);
			}
			if (!File.Exists(fileName))
			{
				return null;
			}
			StreamReader reader = new StreamReader(fileName);
			string fileContents = reader.ReadToEnd();
			int location = 0;
			while (fileContents.Length > location && fileContents.IndexOf("#include", location) != -1)
			{
				int start = fileContents.IndexOf("#include", location);
				int end = fileContents.IndexOf("\n", start);
				if (end == -1)
				{
					end = fileContents.Length;
				}
				string line = fileContents.Substring(start, end - start);
				int firstQuote = line.IndexOf("\"");
				int secondQuote = line.IndexOf("\"", firstQuote + 1);
				//if (firstQuote == -1 || secondQuote == -1)

				string newFile = line.Substring(firstQuote + 1, secondQuote - firstQuote - 1);
				fileContents = fileContents.Replace(line, FixIncludeFiles(newFile));
				location = end;
			}
			return fileContents;
		}

		private void newEditor_DragEnter(object sender, DragEventArgs e)
		{
			mainForm.MainFormRedone_DragEnter(sender, e);
		}

		private void newEditor_DragDrop(object sender, DragEventArgs e)
		{
			mainForm.MainFormRedone_DragDrop(sender, e);
		}

		private void editorBox_DragDrop(object sender, DragEventArgs e)
		{
			mainForm.MainFormRedone_DragDrop(sender, e);
		}

		private void editorBox_DragEnter(object sender, DragEventArgs e)
		{
			mainForm.MainFormRedone_DragEnter(sender, e);
		}

		const int CacheSize = 100;
		private List<IParserData> labelsCache = new List<IParserData>(CacheSize);

		private void bgotoButton_Click(object sender, EventArgs e)
		{
			string text = bgotoButton.Text.Substring(5, bgotoButton.Text.Length - 5);
			bool isMacro = text[text.Length - 1] == '(';
			if (isMacro)
				text = text.Remove(text.Length - 1);
			if (bgotoButton.Text.Substring(0, 4) == "Goto")
			{
				List<IParserData> parserData = new List<IParserData>();
				if (text[0] == '+' || text[0] == '-' || text == "_")
				{
					bool negate = text[0] == '-';
					int steps = text == "_" ? 1 : 0;
					while (text != "_")
					{
						text = text.Remove(0, 1);
						steps++;
					}
					if (negate)
					{
						steps *= -1;
					}
					int i;
					for (i = 0; i < parseInfo.LabelsList.Count; i++)
					{
						if (parseInfo.LabelsList[i].Name == "_")
						{
							parserData.Add(parseInfo.LabelsList[i]);
						}
					}
					i = 0;
					while (i < parserData.Count && parserData[i].Location.Offset < editorBox.ActiveTextAreaControl.Caret.Offset)
					{
						i++;
					}
					if (negate)
					{
						i += steps;
					}
					else
					{
						i += steps - 1;
					}
					IParserData data = parserData[i];
					parserData.Clear();
					parserData.Add(data);                        
				}
				else
				{
					var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
					foreach (ParserInformation info in ProjectService.ParseInfo)
					{
						foreach (IParserData data in info.GeneratedList)
						{
							if (string.Equals(data.Name, text, options))
							{
								parserData.Add(data);
								break;
							}
						}
					}
				}
				if (parserData.Count == 0)
				{
					MessageBox.Show("Unable to locate " + text);
					return;
				}
				else if (parserData.Count == 1)
				{
					DocumentService.GotoLabel(parserData[0]);
					if (!labelsCache.Contains(parserData[0]))
					{
						labelsCache.Add(parserData[0]);
					}
				}
				else
				{
					DockingService.FindResults.NewFindResults(text, ProjectService.ProjectName);
					foreach (IParserData data in parserData)
					{
						string line = String.Empty;
						StreamReader reader = null;
						try
						{
							reader = new StreamReader(data.Parent.SourceFile.ToString());
							line = reader.ReadToEnd().Split('\n')[data.Location.Line];
						}
						catch (Exception)
						{
						}
						finally
						{
							if (reader != null)
							{
								reader.Close();
							}
						}
						DockingService.FindResults.AddFindResult(data.Parent.SourceFile, data.Location.Line, line);
					}
					DockingService.ShowDockPanel(DockingService.FindResults);
				}
			}
			else
			{
				if (Path.IsPathRooted(text))
				{
					DocumentService.GotoFile(text);
				}
				else
				{
					DocumentService.GotoFile(FileOperations.NormalizePath(FindFilePathIncludes(text)));
				}
			}
		}

		private void fixCaseContext_Click(object sender, EventArgs e)
		{
			MenuItem item = sender as MenuItem;
			int offset = editorBox.ActiveTextAreaControl.Caret.Offset;
			if (defaultDelimiters.Contains(editorBox.Document.GetCharAt(offset)))
			{
				offset--;
			}
			while (!defaultDelimiters.Contains(editorBox.Document.GetCharAt(offset)))
			{
				offset--;
			}
			offset++;
			editorBox.Document.Replace(offset, item.Text.Length, item.Text);
		}

		private void findRefContext_Click(object sender, EventArgs e)
		{
			ParserService parserService = new ParserService();
			string word = GetWord();
			DockingService.FindResults.NewFindResults(word, ProjectService.ProjectName);
			var refs = parserService.FindAllReferences(word);
			foreach (var reference in refs)
			{
				foreach (var fileRef in reference)
				{
					DockingService.FindResults.AddFindResult(fileRef);
				}
			}
			DockingService.FindResults.DoneSearching();
			DockingService.ShowDockPanel(DockingService.FindResults);
		}

		private void renameContext_Click(object sender, EventArgs e)
		{
			RefactorForm form = new RefactorForm();
			form.ShowDialog();
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
			if (editorBox.ActiveTextAreaControl.SelectionManager.HasSomethingSelected)
			{
				cutContext.Enabled = true;
				copyContext.Enabled = true;
			}
			else
			{
				cutContext.Enabled = false;
				copyContext.Enabled = false;
			}
			if (!string.IsNullOrEmpty(editorBox.Text))
			{
				var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
				int offset = editorBox.ActiveTextAreaControl.Caret.Offset;
				fixCaseContext.Visible = false;
				string text = editorBox.Document.GetWord(offset);
				if (!string.IsNullOrEmpty(text))
				{
					List<IParserData> parserData = new List<IParserData>();
					foreach (ParserInformation info in ProjectService.ParseInfo)
					{
						foreach (IParserData data in info.GeneratedList)
						{
							if (string.Equals(data.Name, text, options))
							{
								parserData.Add(data);
								break;
							}
						}
					}
					if (parserData.Count > 0)
					{
						fixCaseContext.MenuItems.Clear();
						foreach (IParserData data in parserData)
						{
							if (data.Name == text)
							{
								continue;
							}
							fixCaseContext.Visible = true;
							MenuItem item = new MenuItem(data.Name, new EventHandler(fixCaseContext_Click));
							fixCaseContext.MenuItems.Add(item);
						}
						bgotoButton.Enabled = true;
					}
					else
					{
						bgotoButton.Enabled = false;
					}
				}
				//if the user clicked after the last char we need to catch this
				if (offset == editorBox.Text.Length)
				{
					offset--;
				}
				int startLine = offset;
				while (startLine >= 0 && editorBox.Text[startLine] != '\n')
				{
					startLine--;
				}
				startLine++;
				bool isInclude = false;
				if (string.Compare(editorBox.Text, startLine, "#include", 0, "#include".Length, options) == 0)
				{
					offset = startLine + "#include".Length;
					isInclude = true;
				}
				else
				{
					while (offset >= 0 && (char.IsLetterOrDigit(editorBox.Text[offset]) || editorBox.Text[offset] == '_'))
					{
						offset--;
					}
					offset++;
				}
				int length = 1;
				if (isInclude)
				{
					while (char.IsWhiteSpace(editorBox.Text[offset]))
					{
						offset++;
					}
					while (offset + length < editorBox.Text.Length && (editorBox.Text[offset + length] != ';' && editorBox.Text[offset + length] != '\"' && editorBox.Text[offset + length] != '\r' && editorBox.Text[offset + length] != '\n'))
					{
						length++;
					}
					if (offset + length < editorBox.Text.Length && editorBox.Text[offset + length] == '\"')
					{
						length++;
					}
				}
				else
				{
					while (offset + length < editorBox.Text.Length && (char.IsLetterOrDigit(editorBox.Text[offset + length]) || editorBox.Text[offset + length] == '_'))
					{
						length++;
					}
				}
				string gotoLabel = "";
				try
				{
					gotoLabel = editorBox.Document.GetText(offset, length).Trim();
					if (offset + length < editorBox.Text.Length && editorBox.Text[offset + length] == '(')
						gotoLabel += "(";
					if (gotoLabel == "_")
					{
						if (editorBox.Document.TextContent[offset] == '_')
							offset--;
						else
						{
							while (offset > 0 && editorBox.Document.TextContent[offset] != '_')
								offset--;
							offset--;
						}
						while (offset > 0 && (editorBox.Document.TextContent[offset] == '-' || editorBox.Document.TextContent[offset] == '+'))
						{   offset--; length++; }
						gotoLabel = editorBox.Document.GetText(++offset, length).Trim();
				  }
				}
				catch (Exception ex)
				{
					MessageBox.Show("Somewhere I fucked up the offset:" + ex);
				}
				int num;
				if ("afbcdehlixiypcspnzncpm".IndexOf(gotoLabel) == -1 && !int.TryParse(gotoLabel, out num))
				{
					if (isInclude)
					{
						if (gotoLabel[0] == '\"')
							gotoLabel = gotoLabel.Substring(1, gotoLabel.Length - 2);
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
							bgotoButton.Enabled = string.IsNullOrEmpty(gotoLabel) ? false : true;
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
			List<string> includeDirs = ProjectService.IsInternal ?
						Settings.Default.includeDir.Split('\n').ToList<string>() : ProjectService.IncludeDirs;
			foreach (string dir in includeDirs)
			{
				if (File.Exists(Path.Combine(dir, gotoLabel)))
				{
					return Path.Combine(dir, gotoLabel);
				}
			}
			if (File.Exists(Path.Combine(Path.GetDirectoryName(FileName), gotoLabel)))
			{
				return Path.Combine(Path.GetDirectoryName(FileName), gotoLabel);
			}
			else
			{
				return null;
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
			foreach (Form child in DockingService.Documents)
				if (child != this)
					child.Close();
		}

		private void closeAllMenuItem_Click(object sender, EventArgs e)
		{
			var list = DockingService.Documents.ToList();
			foreach (Form child in list)
				child.Close();
		}

		private void copyPathMenuItem_Click(object sender, EventArgs e)
		{
			Clipboard.SetText(FileName);
		}

		private void openFolderMenuItem_Click(object sender, EventArgs e)
		{
			Process explorer = new Process
								   {
									   StartInfo = {FileName = Path.GetDirectoryName(FileName)}
								   };
			explorer.Start();
		}
		#endregion

		private void GetCodeInfo(string lines)
		{
			if (string.IsNullOrEmpty(lines))
			{
				return;
			}

			if (infoLinesRunning == true)
			{
				infoLinesQueued = true;
				return;
			}

			infoLinesRunning = true;
			CodeCountInfo info = AssemblerService.Instance.CountCode(lines);
			mainForm.Invoke(() => mainForm.UpdateCodeInfo(info));
			if (infoLinesQueued)
			{
				infoLinesQueued = false;
				GetCodeInfo(lines);
			}
			infoLinesRunning = false;
		}

		private void setNextStateMenuItem_Click(object sender, EventArgs e)
		{
			mainForm.SetPC(fileName, editorBox.ActiveTextAreaControl.Caret.Line);
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
			editorBox.ActiveTextAreaControl.Caret.Line = scrollToLine - 1;
		}

		internal void ScrollToOffset(int offset)
		{
			int line = editorBox.Document.GetLineNumberForOffset(offset);
			editorBox.ActiveTextAreaControl.ScrollTo(line);
			editorBox.ActiveTextAreaControl.Caret.Position = new TextLocation(offset - editorBox.Document.GetOffsetForLineNumber(line), line);
		}

		internal void ToggleBreakpoint()
		{
			ToggleBreakpoint(editorBox.ActiveTextAreaControl.Caret.Line);
		}

		internal void ToggleBreakpoint(int lineNum)
		{
			TextEditor.Actions.IEditAction newBreakpoint = new TextEditor.Actions.ToggleBreakpoint();
			newBreakpoint.Execute(editorBox.ActiveTextAreaControl.TextArea);
		}

		internal void RemoveBreakpoint(int lineNum)
		{
			TextEditor.Document.Breakpoint breakpoint = DockingService.ActiveDocument.editorBox.Document.BreakpointManager.GetNextMark(lineNum);
			DockingService.ActiveDocument.editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
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
			switch(type)
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
			string newText = System.Globalization.CultureInfo.CurrentCulture.TextInfo.ToTitleCase(text);
			editorBox.ActiveTextAreaControl.SelectionManager.RemoveSelectedText();
			editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
			editorBox.Document.UndoStack.EndUndoGroup();
		}

		internal void FormatLines()
		{
			string[] lines = editorBox.Text.Split('\n');
			string indent = "\t";
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
			if (settings.autoIndent)
				editorBox.IndentStyle = IndentStyle.Smart;
			else
				editorBox.IndentStyle = IndentStyle.None;
			if (settings.antiAlias)
				editorBox.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
			else
				editorBox.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;
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
			else
			{
				int line =
					editorBox.ActiveTextAreaControl.Caret.Line =
					editorBox.Document.GetLineNumberForOffset(newOffset);
				int col = editorBox.Text.Split('\n')[line].IndexOf(textToFind);
				editorBox.ActiveTextAreaControl.Caret.Column = textToFind.Length + col;
				TextLocation start = new TextLocation(col, line);
				TextLocation end = new TextLocation(textToFind.Length + col, line);
				editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(start, end);
				editorBox.ActiveTextAreaControl.ScrollTo(line);
			}
			return true;
		}

		private void codeCheckAssemblerFinished(object sender, AssemblyFinishEventArgs e)
		{
			try
			{
				RemoveCodeMarkers();
				foreach (var item in e.Output.ParsedErrors)
				{
					if (string.Equals(item.File, fileName, StringComparison.OrdinalIgnoreCase))
					{
						Color underlineColor = item.IsWarning ? Color.Yellow : Color.Red;
						mainForm.Invoke(() => NewEditor.AddSquiggleLine(this, item.LineNum, underlineColor, item.Description));
						mainForm.Invoke(() => UpdateDocument(item.LineNum));
					}
					else
					{
						// because we are not thread safe, its possible that we close the editor as this is going
						var docsList = DockingService.Documents.Select(s => s).ToList();
						foreach (NewEditor doc in docsList)
						{
							if (string.Compare(item.File, doc.FileName, true) == 0)
							{
								Color underlineColor = item.IsWarning ? Color.Yellow : Color.Red;
								mainForm.Invoke(() => NewEditor.AddSquiggleLine(doc, item.LineNum, underlineColor, item.Description));
								mainForm.Invoke(() => doc.UpdateDocument(item.LineNum));
							}
						}
					}
				}
			}
			catch (Exception) { }
		}

		public void UpdateDocument(int line)
		{
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, line));
		}

		private void RemoveCodeMarkers()
		{
			foreach (NewEditor doc in DockingService.Documents)
			{
				doc.EditorBox.Document.MarkerStrategy.RemoveAll(marker => marker != null && marker.Tag == "Code Check");
			}
		}

		public static void AddSquiggleLine(NewEditor doc, int newLineNumber, Color underlineColor, string description)
		{
			var document = doc.EditorBox.Document;
			int start = document.GetOffsetForLineNumber(newLineNumber - 1);
			int length;
			string text;
			if (description.Contains("Can't") || description.Contains("isn't") || description.Contains("Could not"))
			{
				int quote1 = description.IndexOf('\'');
				if (description.Contains("Can't"))
					quote1 = description.IndexOf('\'', 7);
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
			TextMarker highlight = new TextMarker(start, length, TextMarkerType.WaveLine, underlineColor);
			highlight.ToolTip = description;
			highlight.Tag = "Code Check";
			doc.EditorBox.Document.MarkerStrategy.AddMarker(highlight);
		}

		internal void AddSquiggleLine(int newLineNumber, Color underlineColor, string description)
		{
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
			editorBox.ActiveTextAreaControl.ScrollTo(newLineNumber - 1);
			editorBox.ActiveTextAreaControl.Caret.Line = newLineNumber - 1;
			int start = textArea.Caret.Offset;
			int length =
				editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
			while (start > 0 && textArea.Document.TextContent[start] != '\n')
				start--;
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

		internal void HighlightLine(int newLineNumber, Color foregroundColor, string tag)
		{
			//this code highlights the current line
			//I KNOW IT WORKS DONT FUCK WITH IT
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
			int start = textArea.Caret.Offset == editorBox.Text.Length ? textArea.Caret.Offset - 1 : textArea.Caret.Offset;
			int length = editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
			if (editorBox.Document.GetCharAt(start) == '\n')
				start--;
			while (start > 0 && editorBox.Document.GetCharAt(start) != '\n')
				start--;
			start++;
			while (start < textArea.Document.TextContent.Length && char.IsWhiteSpace(editorBox.Document.GetCharAt(start++)))
				length--;
			if (length >= editorBox.Text.Length)
				length += (editorBox.Text.Length - 1) - length;
			if (editorBox.Text.IndexOf(';', start, length) != -1)
				length = editorBox.Text.IndexOf(';', start, length) - start - 1;
			if (editorBox.Text.Length <= start + length)
				length--;
			while (editorBox.Text[start + length] == ' ' || editorBox.Text[start + length] == '\t')
				length--;
			length++;
			TextMarker highlight = new TextMarker(start, length, TextMarkerType.SolidBlock, foregroundColor, Color.Black) 
			{ Tag = DockingService.ActiveDocument.FileName };
			editorBox.Document.MarkerStrategy.AddMarker(highlight);
			editorBox.Refresh();
			DocumentService.ActiveDocument.ScrollToLine(newLineNumber - 1);
		}

		internal void HighlightLine(int newLineNumber, Color foregroundColor)
		{
			//this code highlights the current line
			//I KNOW IT WORKS DONT FUCK WITH IT
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;

			int start = textArea.Caret.Offset == editorBox.Document.TextLength ? textArea.Caret.Offset - 1 : textArea.Caret.Offset;
			int length = editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
			if (editorBox.Document.GetCharAt(start) == '\n')
				start--;
			while (start > 0 && editorBox.Document.GetCharAt(start) != '\n')
				start--;
			start++;
			while (start < editorBox.Document.TextLength && char.IsWhiteSpace(editorBox.Document.GetCharAt(start)))
			{
				start++;
				length--;
			}
			if (length >= editorBox.Document.TextLength)
				length += editorBox.Document.TextLength - 1 - length;
			if (editorBox.Document.TextContent.IndexOf(';', start, length) != -1)
				length = editorBox.Document.TextContent.IndexOf(';', start, length) - start - 1;
			if (editorBox.Document.TextLength <= start + length)
				length--;
			while (char.IsWhiteSpace(editorBox.Document.GetCharAt(start+length)))
				length--;
			length++;
			TextMarker highlight = new TextMarker(start, length, TextMarkerType.SolidBlock, foregroundColor, Color.Black) 
				{ Tag = DockingService.ActiveDocument.FileName };
			editorBox.Document.MarkerStrategy.AddMarker(highlight);
			editorBox.Refresh();
			ScrollToLine(newLineNumber);
		}

		internal void HighlightCall(int lineNumber)
		{
			string line = editorBox.Document.TextContent.Split('\n')[lineNumber];
			if (line.Contains(';'))
			{
				line = line.Remove(line.IndexOf(';'));
			}
			if (line.Contains("call") || line.Contains("bcall") || line.Contains("b_call") || line.Contains("rst"))
			{
				mainForm.AddStackEntry(lineNumber);
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

		internal void AddMarkers(List<TextMarker> markers)
		{
			foreach (TextMarker marker in markers)
			{
				editorBox.Document.MarkerStrategy.AddMarker(marker);
			}
			editorBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
			this.Refresh();
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

	class CodeCompletionProvider : ICompletionDataProvider
	{
		readonly NewEditor mainForm;
		readonly TextEditorControl editorBox;

		public CodeCompletionProvider(NewEditor mainForm)
		{
			this.mainForm = mainForm;
			editorBox = mainForm.EditorBox;
		}

		public ImageList ImageList
		{
			get { return mainForm.imageList1; }
		}       

		public string PreSelection
		{
			get { return null; }
		}

		public int DefaultIndex
		{
			get { return -1; }
		}

		public CompletionDataProviderKeyResult ProcessKey(char key)
		{
			if (char.IsLetterOrDigit(key) || key == '\"' || key == '.' || key == '(' || key == '_' || key == '$')
				return CompletionDataProviderKeyResult.NormalKey;
			// key triggers insertion of selected items
			return CompletionDataProviderKeyResult.InsertionKey;
		}

		/// <summary>
		/// Called when entry should be inserted. Forward to the insertion action of the completion data.
		/// </summary>
		public bool InsertAction(ICompletionData data, TextArea textArea, int insertionOffset, char key)
		{
			textArea.Caret.Position = textArea.Document.OffsetToPosition(insertionOffset);
			bool temp = data.InsertAction(textArea, key);
			if (key == '\n')
			{
				textArea.Caret.Line += 1;
				textArea.Caret.Column += textArea.Document.FormattingStrategy.IndentLine(textArea.Document, textArea.Caret.Line);
			}
			textArea.Refresh();
			return temp;
		}

		private string GetLine(int start)
		{
			if (string.IsNullOrEmpty(editorBox.Text) || start < 0)
			{
				return string.Empty;
			}
			if (start == editorBox.Text.Length)
			{
				start -= 2;
			}
			while (start >= 0 && editorBox.Text[start] != '\n')
			{
				start--;
			}
			start++;
			int end = start;
			//comment effectively ends the line (for our purposes)
			while (end < editorBox.Text.Length && editorBox.Text[end] != '\n' && editorBox.Text[end] != ';')
			{
				end++;
			}
			end--;
			if (end - start == -1)
			{
				return string.Empty;
			}
			string line = editorBox.Text.Substring(start, end - start);
			return line;
		}

		private string GetOpcodeOrMacro(string line)
		{
			int start = 0;
			start = SkipWhitespace(line, start);
			int end = start + 1;
			while (end < line.Length && !char.IsWhiteSpace(line[end]))
			{
				end++;
			}
			if (end > line.Length)
			{
				return string.Empty;
			}
			return line.Substring(start, end - start).Trim();
		}

		private string GetFirstArg(string line, int offset, char charTyped)
		{
			int start = 0;
			start = SkipWhitespace(line, start);
			start += offset;
			int end = start;
			while (end < line.Length && line[end] != ',')
				end++;
			end--;
			return (line[end] != ',' && "(),+-*/\r".IndexOf(charTyped) == -1) || end - start < 1 ? "" : line.Substring(start + 1, end - start).Trim();
		}

		private int SkipWhitespace(string line, int start)
		{
			while (start < line.Length && char.IsWhiteSpace(line[start]))
				start++;
			return start;
		}

		private void Add16BitRegs(ref List<ICompletionData> resultList)
		{
			resultList.Add(new CodeCompletionData("bc", 3));
			resultList.Add(new CodeCompletionData("de", 3));
			resultList.Add(new CodeCompletionData("hl", 3));
			resultList.Add(new CodeCompletionData("ix", 3));
			resultList.Add(new CodeCompletionData("iy", 3));
		}

		private void Add8BitRegs(ref List<ICompletionData> resultList)
		{
			resultList.Add(new CodeCompletionData("a", 3));
			resultList.Add(new CodeCompletionData("b", 3));
			resultList.Add(new CodeCompletionData("d", 3));
			resultList.Add(new CodeCompletionData("h", 3));
			resultList.Add(new CodeCompletionData("ixl", 3));
			resultList.Add(new CodeCompletionData("iyl", 3));
			resultList.Add(new CodeCompletionData("c", 3));
			resultList.Add(new CodeCompletionData("e", 3));
			resultList.Add(new CodeCompletionData("l", 3));
			resultList.Add(new CodeCompletionData("ixh", 3));
			resultList.Add(new CodeCompletionData("iyh", 3));
			resultList.Add(new CodeCompletionData("(hl)", 3));
			resultList.Add(new CodeCompletionData("(ix+", 3));
			resultList.Add(new CodeCompletionData("(iy+", 3));
		}

		private void AddParserData(ref List<ICompletionData> resultList)
		{
			IEnumerable<ParserInformation> includedFiles;
			lock (ProjectService.ParseInfo)
			{
				includedFiles = from info in ProjectService.ParseInfo
								where info.IsIncluded
								select info;
			}
			foreach (var info in includedFiles)
			{
				foreach (IParserData data in info.DefinesList)
					if (!string.IsNullOrEmpty(data.Name))
						resultList.Add(new CodeCompletionData(data.Name, 5, data.Description));
				foreach (IParserData data in info.LabelsList)
				{
					if (data.Name == "_")
						continue;
					resultList.Add(new CodeCompletionData(data.Name, 4, data.Description));
				}
			}
		}

		#region Predefined Data
		ICompletionData[] preprocessors = new ICompletionData[]
							   {
								   new CodeCompletionData("define", 0),
								   new CodeCompletionData("ifdef", 0),
								   new CodeCompletionData("ifndef", 0),
								   new CodeCompletionData("if", 0),
								   new CodeCompletionData("endif", 0),
								   new CodeCompletionData("macro", 0),
								   new CodeCompletionData("endmacro", 0),
								   new CodeCompletionData("comment", 0),
								   new CodeCompletionData("endcomment", 0),
								   new CodeCompletionData("else", 0),
								   new CodeCompletionData("undefine", 0),
								   new CodeCompletionData("include", 0),
								   new CodeCompletionData("region", 0),
								   new CodeCompletionData("endregion", 0)
							   };
		ICompletionData[] directives = new ICompletionData[]
							   {
								   new CodeCompletionData("db", 1),
								   new CodeCompletionData("dw", 1),
								   new CodeCompletionData("end", 1),
								   new CodeCompletionData("or", 1),
								   new CodeCompletionData("byte", 1),
								   new CodeCompletionData("word", 1),
								   new CodeCompletionData("fill", 1),
								   new CodeCompletionData("block", 1),
								   new CodeCompletionData("addinstr", 1),
								   new CodeCompletionData("echo", 1),
								   new CodeCompletionData("error", 1),
								   new CodeCompletionData("list", 1),
								   new CodeCompletionData("nolist", 1),
								   new CodeCompletionData("equ", 1),
								   new CodeCompletionData("option", 1)
							   };
		ICompletionData[] portsList = new ICompletionData[]
							   {
								   new CodeCompletionData("($00)", 6),
								   new CodeCompletionData("($01)", 6),
								   new CodeCompletionData("($02)", 6),
								   new CodeCompletionData("($03)", 6),
								   new CodeCompletionData("($04)", 6),
								   new CodeCompletionData("($05)", 6),
								   new CodeCompletionData("($06)", 6),
								   new CodeCompletionData("($07)", 6),
								   new CodeCompletionData("($08)", 6),
								   new CodeCompletionData("($09)", 6),
								   new CodeCompletionData("($0A)", 6),
								   new CodeCompletionData("($0D)", 6),
								   new CodeCompletionData("($10)", 6),
								   new CodeCompletionData("($11)", 6),
								   new CodeCompletionData("($14)", 6),
								   new CodeCompletionData("($16)", 6),
								   new CodeCompletionData("($18)", 6),
								   new CodeCompletionData("($19)", 6),
								   new CodeCompletionData("($1A)", 6),
								   new CodeCompletionData("($1B)", 6),
								   new CodeCompletionData("($1C)", 6),
								   new CodeCompletionData("($1D)", 6),
								   new CodeCompletionData("($1E)", 6),
								   new CodeCompletionData("($1F)", 6),
								   new CodeCompletionData("($20)", 6),
								   new CodeCompletionData("($21)", 6),
								   new CodeCompletionData("($22)", 6),
								   new CodeCompletionData("($23)", 6),
								   new CodeCompletionData("($27)", 6),
								   new CodeCompletionData("($28)", 6),
								   new CodeCompletionData("($29)", 6),
								   new CodeCompletionData("($2A)", 6),
								   new CodeCompletionData("($2B)", 6),
								   new CodeCompletionData("($2C)", 6),
								   new CodeCompletionData("($2E)", 6),
								   new CodeCompletionData("($2F)", 6),
								   new CodeCompletionData("($30)", 6),
								   new CodeCompletionData("($31)", 6),
								   new CodeCompletionData("($32)", 6),
								   new CodeCompletionData("($33)", 6),
								   new CodeCompletionData("($34)", 6),
								   new CodeCompletionData("($35)", 6),
								   new CodeCompletionData("($36)", 6),
								   new CodeCompletionData("($37)", 6),
								   new CodeCompletionData("($38)", 6),
								   new CodeCompletionData("($40)", 6),
								   new CodeCompletionData("($41)", 6),
								   new CodeCompletionData("($42)", 6),
								   new CodeCompletionData("($43)", 6),
								   new CodeCompletionData("($44)", 6),
								   new CodeCompletionData("($45)", 6),
								   new CodeCompletionData("($46)", 6),
								   new CodeCompletionData("($47)", 6),
								   new CodeCompletionData("($48)", 6),
								   new CodeCompletionData("($4D)", 6),
								   new CodeCompletionData("($55)", 6),
								   new CodeCompletionData("($56)", 6),
								   new CodeCompletionData("($57)", 6),
								   new CodeCompletionData("($5B)", 6),
							   };
		#endregion
		public ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped)
		{
			List<ICompletionData> resultList = new List<ICompletionData>();
			int startOffset = editorBox.ActiveTextAreaControl.Caret.Offset;
			int lineNumber = editorBox.Document.GetLineNumberForOffset(startOffset);
			List<FoldMarker> foldings = editorBox.Document.FoldingManager.GetFoldingsContainsLineNumber(lineNumber);
			bool isInComment = false;
			var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
			foreach (FoldMarker folder in foldings)
				isInComment = folder.InnerText.Contains("#endcomment", options);
			string line = GetLine(startOffset);
			int start = editorBox.Document.OffsetToPosition(startOffset).Column;
			if (line.Length == 0 || line.Length < start || isInComment || char.IsLetterOrDigit(line[0]))
				return resultList.ToArray();
			switch (charTyped)
			{
				case '#':
					return preprocessors;
				case '.':
					return directives;
				default:
					{
						string command = GetOpcodeOrMacro(line);
						string firstArg = GetFirstArg(line, command.Length, charTyped);
						switch (command.ToLower())
						{
							case "ld":
								if (string.IsNullOrEmpty(firstArg))
								{
									Add16BitRegs(ref resultList);
									resultList.Add(new CodeCompletionData("sp", 3));
									Add8BitRegs(ref resultList);
									return resultList.ToArray();
								}
								else
								{
									switch (firstArg.ToLower())
									{
										case "hl":
										case "de":
										case "bc":
										case "iy":
										case "ix":
											AddParserData(ref resultList);
											break;
										case "sp":
											resultList.Add(new CodeCompletionData("hl", 3));
											resultList.Add(new CodeCompletionData("ix", 3));
											resultList.Add(new CodeCompletionData("iy", 3));
											return resultList.ToArray();
										case "a":
										case "b":
										case "c":
										case "d":
										case "e":
										case "h":
										case "l":
										case "(hl)":
											if (firstArg == "a")
											{
												resultList.Add(new CodeCompletionData("i", 3));
												resultList.Add(new CodeCompletionData("r", 3));
											}
											Add8BitRegs(ref resultList);
											break;
										case "i":
										case "r":
											resultList.Add(new CodeCompletionData("a", 3));
											break;
									}
								}
								return resultList.ToArray();
							case "in":
							case "out":
								int temp;
								if (string.IsNullOrEmpty(firstArg))
								{
									resultList.Add(new CodeCompletionData("a", 3));
									resultList.Add(new CodeCompletionData("(C)", 3));
								}
								else if (int.TryParse(firstArg, out temp))
									resultList.Add(new CodeCompletionData("(C)", 3));
								else
									return portsList;
								return resultList.ToArray();
							case "bit":
							case "set":
							case "res":
								if (string.IsNullOrEmpty(firstArg))
								{
									resultList.Add(new CodeCompletionData("0", 6));
									resultList.Add(new CodeCompletionData("1", 6));
									resultList.Add(new CodeCompletionData("2", 6));
									resultList.Add(new CodeCompletionData("3", 6));
									resultList.Add(new CodeCompletionData("4", 6));
									resultList.Add(new CodeCompletionData("5", 6));
									resultList.Add(new CodeCompletionData("6", 6));
									resultList.Add(new CodeCompletionData("7", 6));
								}
								else
									Add8BitRegs(ref resultList);
								return resultList.ToArray();
							case "add":
							case "adc":
							case "sbc":
								if (string.IsNullOrEmpty(firstArg))
								{
									resultList.Add(new CodeCompletionData("a", 3));
									resultList.Add(new CodeCompletionData("hl", 3));
									if (command == "add")
									{
										resultList.Add(new CodeCompletionData("ix", 3));
										resultList.Add(new CodeCompletionData("iy", 3));
									}
								}
								else
								{
									if (firstArg == "hl" || firstArg == "ix" || firstArg == "iy")
									{
										resultList.Add(new CodeCompletionData("bc", 3));
										resultList.Add(new CodeCompletionData("de", 3));
										resultList.Add(new CodeCompletionData("hl", 3));
										resultList.Add(new CodeCompletionData("sp", 3));
									}
									else
										Add8BitRegs(ref resultList);
								}
								return resultList.ToArray();
							case "dec":
							case "inc":
							case "rlc":
							case "rl":
							case "rr":
							case "rrc":
							case "sla":
							case "sll":
							case "sra":
							case "srl":
							// commands that take a register or a number
							case "cp":
							case "or":
							case "xor":
								Add8BitRegs(ref resultList);
								return resultList.ToArray();
							case "sub":
								if (string.IsNullOrEmpty(firstArg))
									resultList.Add(new CodeCompletionData("a", 3));
								else
									Add8BitRegs(ref resultList);
								return resultList.ToArray();
							//16 bit only
							case "push":
							case "pop":
								resultList.Add(new CodeCompletionData("af", 3));
								Add16BitRegs(ref resultList);
								return resultList.ToArray();
							//labels/equates and conditions
							case "call":
							case "jp":
							case "jr":
							case "ret":
							case "djnz":
								//possible to have conditions
								if (command != "djnz" && string.IsNullOrEmpty(firstArg))
								{
									resultList.Add(new CodeCompletionData("z", 2));
									resultList.Add(new CodeCompletionData("nz", 2));
									resultList.Add(new CodeCompletionData("c", 2));
									resultList.Add(new CodeCompletionData("nc", 2));
									if (command != "jr")
									{
										resultList.Add(new CodeCompletionData("p", 2));
										resultList.Add(new CodeCompletionData("m", 2));
										resultList.Add(new CodeCompletionData("po", 2));
										resultList.Add(new CodeCompletionData("pe", 2));
									}
								}
								if (command == "ret")
									return resultList.ToArray();
								AddParserData(ref resultList);
								return resultList.ToArray();
							//special cases
							case "im":
								resultList.Add(new CodeCompletionData("0", 6));
								resultList.Add(new CodeCompletionData("1", 6));
								resultList.Add(new CodeCompletionData("2", 6));
								return resultList.ToArray();
							case "ex":
								resultList.Add(new CodeCompletionData("de,hl", 3));
								resultList.Add(new CodeCompletionData("af,af'", 3));
								resultList.Add(new CodeCompletionData("(sp),hl", 3));
								resultList.Add(new CodeCompletionData("(sp),ix", 3));
								resultList.Add(new CodeCompletionData("(sp),iy", 3));
								return resultList.ToArray();
							case "rst":
								resultList.Add(new CodeCompletionData("08h", 6));
								resultList.Add(new CodeCompletionData("10h", 6));
								resultList.Add(new CodeCompletionData("18h", 6));
								resultList.Add(new CodeCompletionData("20h", 6));
								resultList.Add(new CodeCompletionData("28h", 6));
								resultList.Add(new CodeCompletionData("30h", 6));
								resultList.Add(new CodeCompletionData("38h", 6));
								return resultList.ToArray();
							
							//all the no argument commands
							case "ccf":
							case "cpdr":
							case "cpd":
							case "cpir":
							case "cpi":
							case "cpl":
							case "daa":
							case "di":
							case "ei":
							case "exx":
							case "halt":
							case "indr":
							case "ind":
							case "inir":
							case "ini":
							case "lddr":
							case "ldd":
							case "ldir":
							case "ldi":
							case "neg":
							case "nop":
							case "otdr":
							case "otir":
							case "outd":
							case "outi":
							case "reti":
							case "retn":
							case "rla":
							case "rlca":
							case "rld":
							case "rra":
							case "rrca":
							case "scf":
								return resultList.ToArray();
						}
						break;
					}
			}
			return null;//resultList.ToArray();
		}
	}

	class CodeCompletionData : DefaultCompletionData, ICompletionData
	{
		readonly string member;
		readonly int type;
		public CodeCompletionData(string member, int type): base(member, null, type)
		{
			this.member = member;//.Substring(1, member.Length - 1);
			this.type = type;
		}
		public CodeCompletionData(string member, int type, string description): base(member, description, type)
		{
			this.member = member;//.Substring(1, member.Length - 1);
			this.type = type;
			this.description = description;
		}

		public override string ToString()
		{
			return member;
		}

		int overloads;

		public void AddOverload()
		{
			overloads++;
		}
		string description;

		// DefaultCompletionData.Description is not virtual, but we can reimplement
		// the interface to get the same effect as overriding.
		string ICompletionData.Description
		{
			get
			{
				if (description == null)
				{
					switch (type)
					{
						case 0:
							switch (member)
							{
								case "define":
									description = "Defines a macro or label.";
									break;
								case "ifdef":
									description = "Optionally executes a block of code dependent on if a\nvalue is defined.";
									break;
								case "ifndef":
									description = "Optionally executes a block of code dependent on if a\nvalue is not defined.";
									break;
								case "if":
									description = "Optionally executes a block of code dependent on the\nvalue of a given expression.";
									break;
								case "endif":
									description = "Ends an if statement";
									break;
								case "macro":
									description = "Start of a macro.";
									break;
								case "endmacro":
									description = "Ends a macro.";
									break;
								case "comment":
									description = "Starts a block comment.";
									break;
								case "endcomment":
									description = "Ends a block comment";
									break;
								case "else":
									description = "Executes if the if statement was false.";
									break;
								case "undefine":
									description = "Undefines a label or macro.";
									break;
								case "include":
									description = "Includes the specified file in the assembly.";
									break;
							}
							break;
						case 1:
							switch (member)
							{
								case "db":
									description = "Allows a value assignment to the byte pointed\nto at the current location.";
									break;
								case "dw":
									description = "Allows a value assignment to the word pointed\nto at the current location.";
									break;
								case "end":
									description = "It useless.";
									break;
								case "org":
									description = "Sets the program counter to the desired value.";
									break;
								case "byte":
									description = "Allows a value assignment to the byte pointed\nto at the current location.";
									break;
								case "word":
									description = "Allows a value assignment to the word pointed\nto at the current location.";
									break;
								case "fill":
									description = "Fills a selected number of object bytes with a fixed value.";
									break;
								case "block":
									description = "Advances the specified number of bytes without\nassigning values to the skipped over locations.";
									break;
								case "addinstr":
									description = "Defines additional instructions for use in this assembly.";
									break;
								case "echo":
									description = "Outputs information to the console.";
									break;
								case "error":
									description = "Reports a fatal error.";
									break;
								case "list":
									description = "Turns on writing to the list file.";
									break;
								case "nolist":
									description = "Turns off writing to the list file.";
									break;
								case "equ":
									description = "Used to assign values to labels. The labels can then be used in\nexpressions in place of the literal constant.";
									break;
								case "option":
									description = "The option directive is used to specific how other parts\nof the assembler function. It is possible to specify multiple\noptions with a single directive.";
									break;
							}
							break;
						case 6:
							switch (member)
							{
								case "00h":
									description = "Simulates taking all the batteries out of the calculator";
									break;
								case "08h":
									description = "Execute system routine OP1ToOP2.";
									break;
								case "10h":
									description = "Execute system routine FindSym.";
									break;
								case "18h":
									description = "Execute system routine PushRealO1.";
									break;
								case "20h":
									description = "Execute system routine Mov9ToOP1.";
									break;
								case "28h":
									description = "Part of the b_call() macro.";
									break;
								case "30h":
									description = "Execute system routine FPAdd.";
									break;
								case "38h":
									description = "System interrupt routine.";
									break;
							}
							break;
					}
					//IEntity entity = (IEntity)member ?? c;
					//description = GetText(entity);
					//if (overloads > 1)
					//{
					//    description += " (+" + overloads + " overloads)";
					//}
					//description += Environment.NewLine + XmlDocumentationToText(entity.Documentation);


					//description = " ";
				}
				return description;
			}
		}

		public static string XmlDocumentationToText(string xmlDoc)
		{
			StringReader stringReader = null;
			Debug.WriteLine(xmlDoc);
			StringBuilder b = new StringBuilder();
			try
			{
				stringReader = new StringReader("<root>" + xmlDoc + "</root>");
				using (XmlTextReader reader = new XmlTextReader(stringReader))
				{
					reader.XmlResolver = null;
					while (reader.Read())
					{
						switch (reader.NodeType)
						{
							case XmlNodeType.Text:
								b.Append(reader.Value);
								break;
							case XmlNodeType.Element:
								switch (reader.Name)
								{
									case "filterpriority":
										reader.Skip();
										break;
									case "returns":
										b.AppendLine();
										b.Append("Returns: ");
										break;
									case "param":
										b.AppendLine();
										b.Append(reader.GetAttribute("name") + ": ");
										break;
									case "remarks":
										b.AppendLine();
										b.Append("Remarks: ");
										break;
									case "see":
										if (reader.IsEmptyElement)
										{
											b.Append(reader.GetAttribute("cref"));
										}
										else
										{
											reader.MoveToContent();
											if (reader.HasValue)
											{
												b.Append(reader.Value);
											}
											else
											{
												b.Append(reader.GetAttribute("cref"));
											}
										}
										break;
								}
								break;
						}
					}
				}
				return b.ToString();
			}
			catch (XmlException)
			{
				return xmlDoc;
			}
		}
	}

	class CodeCompletionKeyHandler
	{
		readonly NewEditor mainForm;
		readonly TextEditorControl editor;
		CodeCompletionWindow codeCompletionWindow;

		private CodeCompletionKeyHandler(NewEditor mainForm, TextEditorControl editor)
		{
			this.mainForm = mainForm;
			this.editor = editor;
		}

		public static CodeCompletionKeyHandler Attach(NewEditor mainForm, TextEditorControl editor)
		{
			CodeCompletionKeyHandler h = new CodeCompletionKeyHandler(mainForm, editor);

			editor.ActiveTextAreaControl.TextArea.KeyEventHandler += h.TextAreaKeyEventHandler;

			// When the editor is disposed, close the code completion window
			editor.Disposed += h.CloseCodeCompletionWindow;

			return h;
		}

		/// <summary>
		/// Return true to handle the keypress, return false to let the text area handle the keypress
		/// </summary>
		bool TextAreaKeyEventHandler(char key)
		{
			if (codeCompletionWindow != null)
			{
				// If completion window is open and wants to handle the key, don't let the text area
				// handle it
				if (codeCompletionWindow.ProcessKeyEvent(key))
					return true;
			}
			if (codeCompletionWindow == null && (Settings.Default.enableAutoTrigger && ",( .#\t".IndexOf(key) != -1) || (key == ' ' && Control.ModifierKeys == Keys.Control))
			{
				ICompletionDataProvider completionDataProvider = new CodeCompletionProvider(mainForm);

				if (Control.ModifierKeys == Keys.Control && editor.ActiveTextAreaControl.Caret.Offset != 0)
				{
					//editor.ActiveTextAreaControl.Caret.Column -= 1;
					int startOffset = editor.ActiveTextAreaControl.Caret.Offset;
					if (startOffset == editor.Text.Length)
						startOffset--;
					while (startOffset >= 0 && ",( .#\t\n\r+-*/".IndexOf(editor.Text[startOffset]) == -1)
						startOffset--;
					if (startOffset == -1)
					{
						return false;
					}
					key = editor.Document.TextContent[startOffset];
				}
				codeCompletionWindow = CodeCompletionWindow.ShowCompletionWindow(
					mainForm,					// The parent window for the completion window
					editor, 					// The text editor to show the window for
					mainForm.Text,		        // Filename - will be passed back to the provider
					completionDataProvider,		// Provider to get the list of possible completions
					key							// Key pressed - will be passed to the provider
				);
				if (codeCompletionWindow != null)
				{
					// ShowCompletionWindow can return null when the provider returns an empty list
					codeCompletionWindow.Closed += CloseCodeCompletionWindow;
					//codeCompletionWindow.
				}
			}
			return Control.ModifierKeys == Keys.Control;
		}

		void CloseCodeCompletionWindow(object sender, EventArgs e)
		{
			if (codeCompletionWindow == null)
				return;
			codeCompletionWindow.Closed -= CloseCodeCompletionWindow;
			codeCompletionWindow.Dispose();
			codeCompletionWindow = null;
		}
	}

	public class RegionFoldingStrategy : IFoldingStrategy
	{
		/// <summary>
		/// Generates the foldings for our document.
		/// </summary>
		/// <param name="document">The current document.</param>
		/// <param name="fileName">The filename of the document.</param>
		/// <param name="parseInformation">Extra parse information, not used in this sample.</param>
		/// <returns>A list of FoldMarkers.</returns>
		public List<FoldMarker> GenerateFoldMarkers(IDocument document, string fileName, object parseInformation)
		{
			List<FoldMarker> list = new List<FoldMarker>();

			Stack<FoldingItem> regionLines = new Stack<FoldingItem>();
			Stack<FoldingItem> ifLines = new Stack<FoldingItem>();
			Stack<FoldingItem> macroLines = new Stack<FoldingItem>();
			Stack<int> commentLines = new Stack<int>();
			// Create foldmarkers for the whole document, enumerate through every line.
			for (int i = 0; i < document.TotalNumberOfLines; i++)
			{
				var seg = document.GetLineSegment(i);
				int offs, end = document.TextLength;
				char c;
				for (offs = seg.Offset; offs < end && ((c = document.GetCharAt(offs)) == ' ' || c == '\t'); offs++)
				{ }
				if (offs == end)
					break;
				int spaceCount = offs - seg.Offset;

				// now offs points to the first non-whitespace char on the line
				if (
					document.GetCharAt(offs) != '#') 
					continue;
				var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
				string normalText = document.GetText(offs, seg.Length - spaceCount);
				if (normalText.StartsWith("#region", options))
					regionLines.Push(new FoldingItem(i, normalText.Remove(0, "#region".Length).Trim()));
				if (normalText.StartsWith("#ifdef", options) || normalText.StartsWith("#if", options) || normalText.StartsWith("#ifndef", options))
					ifLines.Push(new FoldingItem(i, normalText));
				if (normalText.StartsWith("#macro", options))
				{
					int paren = normalText.IndexOf('(');
					if (paren == -1)
						paren = normalText.Length - 1;
					string substring = normalText.Substring(0, paren);
					macroLines.Push(new FoldingItem(i, substring));
				}
				if (normalText.StartsWith("#comment", options))
					commentLines.Push(i);
				if (normalText.StartsWith("#endregion", options) && regionLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = regionLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0, i, spaceCount + "#endregion".Length, FoldType.Region, start.Text));
				}
				if (normalText.StartsWith("#else", options) && ifLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = ifLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0, i - 1, document.GetLineSegment(i-1).Length, FoldType.TypeBody, start.Text));
					int offset = spaceCount + "#else".Length;
					ifLines.Push(new FoldingItem(i, "#else"));
				}
				if (normalText.StartsWith("#endif", options) && ifLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = ifLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0 /*document.GetLineSegment(start).Length*/,
											i, spaceCount + "#endif".Length, FoldType.TypeBody, start.Text));
				}
				if (normalText.StartsWith("#endmacro", options) && macroLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = macroLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0,
											i, spaceCount + "#endmacro".Length, FoldType.MemberBody, start.Text));
				}
				if (normalText.StartsWith("#endcomment", options) && commentLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					int start = commentLines.Pop();
					list.Add(new FoldMarker(document, start, 0 /*document.GetLineSegment(start).Length*/,
											i, spaceCount + "#endcomment".Length, FoldType.Region, "#comment"));
				}
			}
			return list;
		}
	}

	public class FoldingItem
	{
		int offset;
		public int Offset
		{
			get { return offset; }
		}
		string text;
		public string Text
		{
			get { return text; }
		}
		public FoldingItem(int offset, string text)
		{
			this.offset = offset;
			this.text = text;
		}
	}
}
