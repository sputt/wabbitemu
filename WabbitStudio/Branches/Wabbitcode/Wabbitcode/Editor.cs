using System.Configuration;
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
	public partial class Editor : IClipboardOperation, IUndoable, ISelectable
    {
		#region Private Memebers
		private const int LabelsCacheSize = 100;
	    private static readonly Regex LineRegex = new Regex(@"^\s*(?<line>[\w|\s|,|\(|\)|:|\*|/|\+|\-|\$|\%|'|\\|\<|\>]*?)\s*(;.*)?$", RegexOptions.Compiled);

	    private readonly Bitmap _warningBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.Warning16.png"));
		private readonly Bitmap _errorBitmap = new Bitmap(Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.Wabbitcode.Resources.PNG.error.png"));
		private readonly List<CancellationTokenSource> _queuedFiles = new List<CancellationTokenSource>();
		private CancellationTokenSource _highlightRefsCancellationTokenSource;
		private readonly IBackgroundAssemblerService _backgroundAssemblerService;
		private readonly IDockingService _dockingService;
		private readonly IDocumentService _documentService;
		private readonly IParserService _parserService;
		private readonly IProjectService _projectService;
		private readonly ISymbolService _symbolService;
        private readonly IFileReaderService _fileReaderService;
	    private IWabbitcodeDebugger _debugger;
		private string _fileName;
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


	    public bool ReadOnly
	    {
	        get
	        {
	            return editorBox.Document.ReadOnly;
	        }
	        set
	        {
	            editorBox.Document.ReadOnly = value;
	        }
	    }

	    public string DocumentFoldings
	    {
	        get
	        {
	            return editorBox.Document.FoldingManager.SerializeToString();
	        }
	        set
	        {
                editorBox.Document.FoldingManager.DeserializeFromString(value);
	        }
	    }

	    #endregion

		#region Static Events

		public delegate void EditorEventHandler(object sender, EditorEventArgs e);
        public delegate void EditorToolTipHandler(object sender, EditorToolTipRequestEventArgs e);
		public delegate void EditorSelectionHandler(object sender, EditorSelectionEventArgs e);

        public static event EditorEventHandler OnEditorUpdated;
        public static event EditorEventHandler OnEditorOpened;
        public static event EditorEventHandler OnEditorClosing;
        public static event EditorToolTipHandler OnEditorToolTipRequested;
        public static event EditorSelectionHandler OnEditorSelectionChanged;
        public static event DragEventHandler OnEditorDragEnter;
        public static event DragEventHandler OnEditorDragDrop;

		#endregion

		public Editor(IBackgroundAssemblerService backgroundAssemblerService, IDockingService dockingService,
			IDocumentService documentService, IFileReaderService fileReaderService, IParserService parserService,
            IProjectService projectService, ISymbolService symbolService)
		{
			InitializeComponent();

			_backgroundAssemblerService = backgroundAssemblerService;
			_dockingService = dockingService;
			_documentService = documentService;
			_parserService = parserService;
			_projectService = projectService;
			_symbolService = symbolService;
		    _fileReaderService = fileReaderService;

			_textChangedTimer.Tick += textChangedTimer_Tick;

			editorBox.TextRenderingHint = Settings.Default.AntiAlias ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
			editorBox.TextEditorProperties.MouseWheelScrollDown = !Settings.Default.InverseScrolling;
			editorBox.ShowLineNumbers = Settings.Default.LineNumbers;
			editorBox.Font = Settings.Default.EditorFont;
			editorBox.LineViewerStyle = Settings.Default.LineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None;
			editorBox.ActiveTextAreaControl.TextArea.ToolTipRequest += TextArea_ToolTipRequest;
			editorBox.Document.FormattingStrategy = new AsmFormattingStrategy();

			WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
			WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnBreakpointRemoved;
            WabbitcodeDebugger.OnDebuggingStarted += (sender, e) =>
		    {
		        _debugger = e.Debugger;
		        setNextStateMenuItem.Visible = true;
		        editorBox.Document.ReadOnly = true;
		    };
            WabbitcodeDebugger.OnDebuggingEnded += (sender, e) =>
		    {
		        _debugger = null;
                setNextStateMenuItem.Visible = false;
                editorBox.Document.ReadOnly = false;
		    };

		    _debugger = WabbitcodeDebugger.Instance;
		    if (_debugger != null)
		    {
                setNextStateMenuItem.Visible = true;
                editorBox.Document.ReadOnly = true;
		    }

			CodeCompletionKeyHandler.Attach(this, editorBox, parserService);
            Settings.Default.SettingChanging += Default_SettingChanging;
		}

        void Default_SettingChanging(object sender, SettingChangingEventArgs e)
        {
            if (e.NewValue == null)
            {
                return;
            }
            
            switch (e.SettingName)
            {
                case "MouseWheelScrollDown":
                    editorBox.TextEditorProperties.MouseWheelScrollDown = !((bool)e.NewValue);
                    break;
                case "EnableFolding":
                    if ((bool)e.NewValue)
                    {
                        editorBox.Document.FoldingManager.FoldingStrategy = new RegionFoldingStrategy();
                        editorBox.Document.FoldingManager.UpdateFoldings(null, null);
                    }
                    else
                    {
                        editorBox.Document.FoldingManager.FoldingStrategy = null;
                        editorBox.Document.FoldingManager.UpdateFoldings(new List<FoldMarker>());
                    }
                    break;
                case "AutoIndent":
                    editorBox.IndentStyle = (bool)e.NewValue ? IndentStyle.Smart : IndentStyle.None;
                    break;
                case "AntiAlias":
                    editorBox.TextRenderingHint = (bool)e.NewValue ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
                    break;
                case "EditorFont":
                    editorBox.Font = (Font) e.NewValue;
                    break;
                case "TabSize":
                    editorBox.TabIndent = (int) e.NewValue;
                    break;
                case "ConvertTabs":
                    editorBox.ConvertTabsToSpaces = (bool)e.NewValue;
                    break;
                case "ExternalHighlight":
                    UpdateHighlighting();
                    break;
                case "IconBar":
                    editorBox.IsIconBarVisible = (bool) e.NewValue;
                    break;
                case "LineNumbers":
                    editorBox.ShowLineNumbers = (bool) e.NewValue;
                    break;
            }
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
				IParserData data = _parserService.GetParserData(text, Settings.Default.CaseSensitive).FirstOrDefault();
				tooltip = data == null ? _symbolService.SymbolTable.GetAddressFromLabel(text) : data.Description;
			}
			catch (Exception)
			{
				return;
			}

		    if (string.IsNullOrEmpty(tooltip))
		    {
		        var editorEventArgs = new EditorToolTipRequestEventArgs(this, text);
		        if (OnEditorToolTipRequested != null)
		        {
		            OnEditorToolTipRequested(this, editorEventArgs);
		        }

		        tooltip = editorEventArgs.Tooltip;
		        if (string.IsNullOrEmpty(tooltip))
		        {
		            return;
		        }
		    }

            e.ShowToolTip(tooltip);
		}

		void SelectionManager_SelectionChanged(object sender, EventArgs e)
		{
			if (string.IsNullOrEmpty(editorBox.Text))
			{
				return;
			}

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
				OnEditorSelectionChanged(this, new EditorSelectionEventArgs(this, editorBox.ActiveTextAreaControl.Caret, codeInfoLines));
			}
		}

		void Caret_PositionChanged(object sender, EventArgs e)
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
			var options = Settings.Default.CaseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
			if (string.IsNullOrEmpty(word) || !Settings.Default.ReferencesHighlighter)
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
			_dockingService.Invoke(() => AddMarkers(references));
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
				OnEditorOpened(this, new EditorEventArgs(this));
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

	    internal void SaveFile(string fileName)
	    {
	        if (string.IsNullOrEmpty(fileName))
	        {
	            return;
	        }

	        FileName = fileName;
	        SaveFile();
	    }

		internal void SaveFile()
		{
			if (_projectService.Project.ProjectWatcher != null)
			{
				_projectService.Project.ProjectWatcher.EnableRaisingEvents = false;
			}
			if (string.IsNullOrEmpty(FileName))
			{
                new SaveAsCommand(this).Execute();
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

			if (_projectService.Project.ProjectWatcher != null)
			{
				_projectService.Project.ProjectWatcher.EnableRaisingEvents = true;
			}

			editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
			TabText = Path.GetFileName(FileName);
			DocumentChanged = false;
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
				OnEditorClosing(this, new EditorEventArgs(this));
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
            _debugger.SetPCToSelect(_fileName, editorBox.ActiveTextAreaControl.Caret.Line + 1);
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
					parserData = _parserService.GetParserData(text, Settings.Default.CaseSensitive).ToList(); 
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
				string fileFullPath = Path.IsPathRooted(text) ? text : FileOperations.NormalizePath(
                    _projectService.Project.GetFilePathFromRelativePath(text));
				_documentService.GotoFile(fileFullPath);
			}
		}
		#endregion

		#region Drag and Drop

		private void editor_DragEnter(object sender, DragEventArgs e)
		{
		    if (OnEditorDragEnter != null)
		    {
		        OnEditorDragEnter(sender, e);
		    }
		}

		private void editor_DragDrop(object sender, DragEventArgs e)
		{
            if (OnEditorDragDrop != null)
            {
                OnEditorDragDrop(sender, e);
            }
		}

		private void editorBox_DragDrop(object sender, DragEventArgs e)
		{
            if (OnEditorDragDrop != null)
            {
                OnEditorDragDrop(sender, e);
            }
		}

		private void editorBox_DragEnter(object sender, DragEventArgs e)
		{
            if (OnEditorDragEnter != null)
            {
                OnEditorDragEnter(sender, e);
            }
		}

		#endregion

        public void SelectAll()
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
					OnEditorUpdated(this, new EditorEventArgs(this));
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
            var refs = _projectService.FindAllReferences(word);
			_dockingService.FindResults.NewFindResults(word, _projectService.Project.ProjectName);
			foreach (var fileRef in refs.SelectMany(r => r))
			{
				_dockingService.FindResults.AddFindResult(fileRef);
			}
			_dockingService.FindResults.DoneSearching();
			_dockingService.ShowDockPanel(_dockingService.FindResults);
		}

        private void renameContext_Click(object sender, EventArgs e)
		{
			RefactorForm refactorForm = new RefactorForm(_dockingService, _projectService);
            refactorForm.ShowDialog();
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
					IEnumerable<IParserData> parserData =  _parserService.GetParserData(text, Settings.Default.CaseSensitive).ToList();
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
			contextMenu.Show(editorBox, editorBox.PointToClient(MousePosition));
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

		internal void ScrollToLine(int line)
		{
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

        private void UpdateHighlighting()
        {
            if (!string.IsNullOrEmpty(FileName))
            {
                editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
            }
            else
            {
                SetHighlighting("Z80 Assembly");
            }
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

		    Match match = LineRegex.Match(line);
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
			for (int i = 0; i < Settings.Default.TabSize; i++)
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

	    public void ShowFindForm(Form owner, SearchMode mode)
	    {
	        _dockingService.FindForm.ShowFor(owner, editorBox, mode);
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
