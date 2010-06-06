//#define CHECK_CODE
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Drawing;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using System.Drawing.Text;
using System.Threading;
using Revsoft.Wabbitcode.Services;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Services.Parser;
using System.Reflection;

namespace Revsoft.Wabbitcode
{
    /// <summary>
    /// Summary description for frmDocument.
    /// </summary>
    public partial class newEditor
    {
		bool docChanged;
		public bool DocumentChanged
		{
			get { return docChanged; }
		}
        public int[] labelsLoc = new int[1];
        public int debugEditAndCont;

		public string FileName
		{
			get
			{
				if (this.IsDisposed || this.Disposing)
					return null;
				if (editorBox.InvokeRequired)
					return editorBox.Invoke(new GetFileNameDelegate(GetFileName)).ToString();
				else
					return editorBox.FileName; }
			set { editorBox.FileName = value; }
		}

        public newEditor()
        {
            InitializeComponent();

			//fix this stuff so settings are applied on opening...what a concept :P
			if (Settings.Default.antiAlias)
				editorBox.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
			else
				editorBox.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;
			editorBox.TextEditorProperties.MouseWheelScrollDown = !Settings.Default.inverseScrolling;
			editorBox.ShowLineNumbers = Settings.Default.lineNumbers;
			editorBox.Font = Settings.Default.editorFont;
			editorBox.LineViewerStyle = Settings.Default.lineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None;
            
            CodeCompletionKeyHandler.Attach(this, editorBox);

			parserThread = new Thread(ParseFile);
			parserThread.Priority = ThreadPriority.BelowNormal;
			parserThread.Start();

			codeInfoThread = new Thread(GetCodeInfo);
			codeInfoThread.Priority = ThreadPriority.BelowNormal;
			codeInfoThread.Start();
        }
		Thread parserThread;
		Thread codeInfoThread;

		private string EditorText
		{
			get
			{
				if (editorBox.InvokeRequired)
					return Invoke(new GetTextDelegate(GetText)).ToString();
				else
					return editorBox.Text;
			}
		}

		delegate string GetTextDelegate();
		private string GetText()
		{
			return editorBox.Text;
		}

		delegate string GetFileNameDelegate();
		private string GetFileName()
		{
			return editorBox.FileName;
		}


		EditorUpdateRequest RequestUpdate = new EditorUpdateRequest();
		delegate void UpdateLabelBoxDelegate();
		private void ParseFile()
		{
			while (true)
			{
				Thread.Sleep(200);
				if (RequestUpdate.UpdateLabels && DocumentService.ActiveFileName != null &&
								FileName.ToLower() == DocumentService.ActiveFileName.ToLower())
				{
					parseInfo = ParserService.ParseFile(FileName, EditorText);
					UpdateLabelBoxDelegate updateLabelDelegate = new UpdateLabelBoxDelegate(UpdateLabelBox);
					BeginInvoke(updateLabelDelegate, null);
					RequestUpdate.UpdateLabels = false;
				}
			}
		}

		private void GetCodeInfo()
		{
			while (true)
			{
				if (RequestUpdate.UpdateCodeInfo)
				{
					GetCodeInfo(codeInfoLines);
				}
				Thread.Sleep(100);
			}
		}

        protected override string GetPersistString()
        {
            // Add extra information into the persist string for this document
            // so that it is available when deserialized.
            return GetType() +  ";" + editorBox.FileName + ";" + editorBox.ActiveTextAreaControl.HorizontalScroll.Value + ";" +
                   editorBox.ActiveTextAreaControl.VerticalScroll.Value + ";" +
                   editorBox.ActiveTextAreaControl.Caret.Column + ";" +
                   editorBox.ActiveTextAreaControl.Caret.Line;
        }

		void TextArea_ToolTipRequest(object sender, TextEditor.ToolTipRequestEventArgs e)
		{
			
		}

        void BreakpointManager_Removed(object sender, BreakpointEventArgs e)
        {
            if (e.Breakpoint.Anchor.IsDeleted == false)
                DebuggerService.RemoveBreakpoint(e.Breakpoint.LineNumber, editorBox.FileName);
            if (DockingService.BreakManager != null)
                DockingService.BreakManager.UpdateManager();
        }

        void BreakpointManager_Added(object sender, BreakpointEventArgs e)
        {
            DebuggerService.AddBreakpoint(e.Breakpoint.LineNumber, editorBox.FileName);
			if (DockingService.BreakManager != null)
				DockingService.BreakManager.UpdateManager();
        }

		string codeInfoLines = "";
        void SelectionManager_SelectionChanged(object sender, EventArgs e)
        {
            if (editorBox.Text != "")
            {
				RequestUpdate.UpdateCodeInfo = true;
				ListFileValue label = DebuggerService.GetListValue(editorBox.FileName, editorBox.ActiveTextAreaControl.Caret.Line);
				if(label != null)
					DockingService.MainForm.SetToolStripText("Page: " + label.Page.ToString() + " Address: " + label.Address.ToString("X4")); 
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
                    start--;
                while (start >= 0 && editorBox.Text[start] != '\n')
                    start--;
                start++;
                while (end < editorBox.Text.Length && editorBox.Text[end] != '\n')
                    end++;
                end--;
                if (start > end)
                    start = end;
                codeInfoLines = editorBox.Text.Substring(start, end - start);
            }

            ReadOnlyCollection<TextMarker> markers = (ReadOnlyCollection<TextMarker>)editorBox.Document.MarkerStrategy.TextMarker;
            int i = 0;
            while (i < markers.Count)
            {
                if (markers[i].Tag == "Code Check")
                    editorBox.Document.MarkerStrategy.RemoveMarker(markers[i]);
                markers = (ReadOnlyCollection<TextMarker>)editorBox.Document.MarkerStrategy.TextMarker;
                i++;
            }
#if CHECK_CODE
            if (!codeChecker.IsBusy)// && !ProjectService.IsInternal)
                codeChecker.RunWorkerAsync(editorBox.Text.Split('\n')[editorBox.ActiveTextAreaControl.Caret.Line]);
#endif
        }

        void Caret_PositionChanged(object sender, EventArgs e)
        {
            DockingService.MainForm.SetLineAndColStatus(editorBox.ActiveTextAreaControl.Caret.Line.ToString(),
														editorBox.ActiveTextAreaControl.Caret.Column.ToString());
            //update code info
            SelectionManager_SelectionChanged(sender, e);
        }

        public void UpdateIcons()
        {
            editorBox.ActiveTextAreaControl.TextArea.Document.IconManager.ClearIcons();
            foreach (Errors errorWarning in AssemblerService.ErrorsInFiles)
            {
                if (errorWarning.file != editorBox.FileName)
                    continue;
                Bitmap newIcon;
                if (errorWarning.isWarning)
                    newIcon = new Bitmap(Assembly.GetExecutingAssembly().
								GetManifestResourceStream("Revsoft.Wabbitcode.Resources.Warning16.png"));
                else
                    newIcon = new Bitmap(Assembly.GetExecutingAssembly().
								GetManifestResourceStream("Revsoft.Wabbitcode.Resources.error.png"));
                MarginIcon marginIcon = new MarginIcon(newIcon, errorWarning.lineNum - 1, errorWarning.toolTip);
                editorBox.Document.IconManager.AddIcon(marginIcon);
            }
        }      

		private ParserInformation parseInfo;
        public void OpenFile(string filename)
        {
            editorBox.FileName = filename.ToLower();
            editorBox.LoadFile(filename, true, true);
            editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(editorBox.FileName);
            Text = filename;
            /*if (!GlobalClass.mainForm.staticLabelsParser.IsBusy && GlobalClass.mainForm.debugging && !GlobalClass.mainForm.IsDisposed && !GlobalClass.mainForm.Disposing)
                GlobalClass.mainForm.staticLabelsParser.RunWorkerAsync(editorBox);*/
            docChanged = false;
			UpdateTabText();
            UpdateIcons();
			RequestUpdate.UpdateAll();
			if (!ProjectService.IsInternal)
				ProjectService.ActiveFileChanged();
			if (ProjectService.CurrentFile != null)
				editorBox.Document.FoldingManager.DeserializeFromString(ProjectService.CurrentFile.FileFoldings);
        }

        public bool SaveFile()
        {
            bool saved = true;
            string fileName = editorBox.FileName;
            stackTop = editorBox.Document.UndoStack.UndoItemCount;
            try
            {
				//TODO: Fix spasm to allow for UTF-8
                editorBox.Encoding = System.Text.Encoding.ASCII;
                editorBox.SaveFile(fileName);
            }
            catch (Exception ex)
            {
                saved = false;
                //((Wabbitcode.frmMain)(MdiParent)).errorConsoleBox.Text = ex.ToString() + '\n';
                MessageBox.Show("Error: " + ex);
            }
            editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(editorBox.FileName);
            TabText = Path.GetFileName(fileName);
            docChanged = false;
            DockingService.MainForm.UpdateTitle();
            return saved;
        }

        private new void FormClosed(object sender, EventArgs e)
        {
            editorBox.Dispose();
        }

        private new void FormClosing(object sender, CancelEventArgs e)
        {
			parserThread.Abort();
			codeInfoThread.Abort();
			string fileName = editorBox.FileName;
            if (!ProjectService.IsInternal)
            {
                if (ProjectService.ContainsFile(fileName))
                {
					Revsoft.Wabbitcode.Services.Project.ProjectFile file = ProjectService.FindFile(fileName);
					file.FileFoldings = editorBox.Document.FoldingManager.SerializeToString();
                }
            }
            if (!DocumentChanged) 
                return;
			if (string.IsNullOrEmpty(editorBox.FileName))
				fileName = "New Document";
            DialogResult dlg = MessageBox.Show(this, "Document '" + fileName + "' has changed. Save changes?", "Wabbitcode", MessageBoxButtons.YesNoCancel);
            switch (dlg)
            {
                case DialogResult.Cancel:
                    e.Cancel = true;
                    break;
                case DialogResult.Yes:
                    if (string.IsNullOrEmpty(editorBox.FileName))
						DocumentService.SaveDocument(this);
                    else
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
			editorBox.ActiveTextAreaControl.Document.FormattingStrategy.FormatLine(editorBox.ActiveTextAreaControl.TextArea, editorBox.ActiveTextAreaControl.Caret.Line, 0, e.KeyChar);
		}

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (keyData == Keys.F3)
                DockingService.FindForm.FindNext(true, false, "Text not found");
			if (MacroService.IsRecording)
				MacroService.RecordKeyData(keyData);
            return base.ProcessCmdKey(ref msg, keyData);
        }

        public void UpdateLabelBox()
        {
			if (parseInfo != null)
				DockingService.LabelList.AddLabels(parseInfo.LabelsList);
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
            TextLocation selectStart = new TextLocation(0, 0);
            TextLocation selectEnd = new TextLocation(editorBox.Text.Split('\n')[editorBox.Document.TotalNumberOfLines - 1].Length, editorBox.Document.TotalNumberOfLines - 1);
            editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(selectStart, selectEnd);
        }

#if CHECK_CODE
        private List<TextMarker> codeCheckMarkers = new List<TextMarker>();
#endif
        //private static Process wabbitspasm;
        private int stackTop;
        private void editorBox_TextChanged(object sender, EventArgs e)
        {
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
			RequestUpdate.UpdateAll();
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
			DockingService.MainForm.UpdateTitle();
        }

		private void UpdateTabText()
		{
			string changedString = DocumentChanged ? "*" : "";
			if (!string.IsNullOrEmpty(editorBox.FileName))
				TabText = Path.GetFileName(editorBox.FileName) + changedString;
			else
				TabText = "New Document" + changedString;
		}

#if CHECK_CODE
        private void codeCheck_DoWork(object sender, DoWorkEventArgs e)
        {
            foreach (TextMarker marker in codeCheckMarkers)
                editorBox.Document.MarkerStrategy.RemoveMarker(marker);
            codeCheckMarkers.Clear();

			wabbitspasm = new Process
			{
				StartInfo =
				{
					FileName = Path.Combine(Application.StartupPath, "spasm.exe"),
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					UseShellExecute = false,
					CreateNoWindow = true
				}
			};
			string filePath = editorBox.FileName;
            /*XmlNodeList buildConfigs = GlobalClass.project.getBuildConfigs();
            XmlNodeList steps = buildConfigs[Settings.Default.buildConfig].ChildNodes;
            string assembledName = "";
            foreach (XmlElement step in steps)
            {
                //int error = 0;
                //if (step.Attributes["action"].Value[0] != 'C')
                string originalDir = Path.GetDirectoryName(step.InnerText);
                //string fileName;
                if (step.Attributes["action"].Value[0] != 'C') 
                    continue;
                filePath = step.InnerText;
                assembledName = Path.Combine(originalDir, GlobalClass.project.getProjectName() + "." +
                                                          MainFormRedone.getExtension(Convert.ToInt32(step.Attributes["type"].Value)));
            }

            GlobalClass.getResource("Revsoft.Wabbitcode.Resources.spasm.exe", "spasm.exe");            
            //Get our emulator
            GlobalClass.getResource("Revsoft.Wabbitcode.Resources.Wabbitemu.exe", "Wabbitemu.exe");
            //create two new processes to run
            //setup wabbitspasm to run silently
            //some strings we'll need to build 
            string includedir = "-I \"" + Application.StartupPath + "\"";
            if (Settings.Default.includeDir != "" || !ProjectService.IsInternal)
            {
                string[] dirs = ProjectService.IsInternal ? Settings.Default.includeDir.Split('\n') : GlobalClass.project.getIncludeDirs();
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        includedir += ";\"" + dir + "\"";
                }
            }
                // filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);

            string fileContents = fixIncludeFiles(filePath);*/
			string originaldir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            const string quote = "\"";
			wabbitspasm.StartInfo.Arguments = "-V " + quote + e.Argument.ToString().TrimEnd() +quote;
            wabbitspasm.StartInfo.WorkingDirectory = originaldir;
            wabbitspasm.Start();
            string output = wabbitspasm.StandardOutput.ReadToEnd();
            foreach (string line in output.Split('\n'))
            {
                addSquiggle squiggleDelegate = addSquiggleLine;
                int firstColon;
                string file;
                string description;
                int lineNum;
                int secondColon;
                int thirdColon;
                if (line.Contains("error"))
                {
                    firstColon = line.IndexOf(':', 3);
                    file = line.Substring(0, firstColon);
                    if (editorBox.FileName != file)
                        continue;
                    secondColon = line.IndexOf(':', firstColon + 1);
                    thirdColon = line.IndexOf(':', secondColon + 1);
                    lineNum = Convert.ToInt32(line.Substring(firstColon + 1, secondColon - firstColon - 1));
                    description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                    if (description == "Couldn't open output file\r")
                        continue;
                    Invoke(squiggleDelegate, new object[] { lineNum - 2, Color.Red, description });
                }
                if (!line.Contains("warning"))
                    continue;
                firstColon = line.IndexOf(':', 3);
                file = line.Substring(0, firstColon);
                if (editorBox.FileName != file)
                    continue;
                secondColon = line.IndexOf(':', firstColon + 1);
                thirdColon = line.IndexOf(':', secondColon + 1);
                lineNum = Convert.ToInt32(line.Substring(firstColon + 1, secondColon - firstColon - 1));
                description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                if (description == "Couldn't open output file\r")
                    continue;
                Invoke(squiggleDelegate, new object[] { lineNum - 1, Color.Gold, description });
            }
        }

        private string fixIncludeFiles(string fileName)
        {
            if (!Path.IsPathRooted(fileName))
                fileName = Path.Combine(Path.GetDirectoryName(editorBox.FileName), fileName);
            if (!File.Exists(fileName))
                return null;
            StreamReader reader = new StreamReader(fileName);
            string fileContents = reader.ReadToEnd();
            int location = 0;
            while (fileContents.Length > location && fileContents.IndexOf("#include", location) != -1)
            {
                int start = fileContents.IndexOf("#include", location);
                int end = fileContents.IndexOf("\n", start);
                if (end == -1)
                    end = fileContents.Length;
                string line = fileContents.Substring(start, end - start);
                int firstQuote = line.IndexOf("\"");
                int secondQuote = line.IndexOf("\"", firstQuote + 1);
                //if (firstQuote == -1 || secondQuote == -1)

                string newFile = line.Substring(firstQuote + 1, secondQuote - firstQuote - 1);
                fileContents = fileContents.Replace(line, fixIncludeFiles(newFile));
                location = end;
            }
            return fileContents;
        }

        private delegate void addSquiggle(int newLineNumber, Color underlineColor, string description);
        private void addSquiggleLine(int newLineNumber, Color underlineColor, string description)
        {
            TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
            int start = textArea.Document.GetOffsetForLineNumber(newLineNumber);
            int length = editorBox.Text.Split('\n')[newLineNumber].Length - 1;
            string text;
            if (description.Contains("Can't") || description.Contains("isn't"))
            {
                int quote1 = description.IndexOf('\'');
                if (description.Contains("Can't"))
                    quote1 = description.IndexOf('\'', 7);
                int quote2 = description.IndexOf('\'', quote1 + 1);
                length = quote2 - quote1 - 1;
                text = description.Substring(quote1 + 1, length);
                start = textArea.Document.TextContent.IndexOf(text, start);
            }
            else if (description.Contains("No such file or directory"))
            {
                text = description.Substring(0, description.IndexOf(':', 4));
                length = text.Length;
                start = textArea.Document.TextContent.IndexOf(text, start);
            }
            else
            {
                while (textArea.Document.TextContent[start] == ' ' || textArea.Document.TextContent[start] == '\t')
                {
                    start++;
                    length--;
                }
            }
            TextMarker highlight = new TextMarker(start, length, TextMarkerType.WaveLine, underlineColor);
            highlight.ToolTip = description;
            highlight.Tag = "Code Check";
            editorBox.Document.MarkerStrategy.AddMarker(highlight);
        }
#endif

        private void newEditor_DragEnter(object sender, DragEventArgs e)
        {
			DockingService.MainForm.MainFormRedone_DragEnter(sender, e);
        }

        private void newEditor_DragDrop(object sender, DragEventArgs e)
        {
			DockingService.MainForm.MainFormRedone_DragDrop(sender, e);
        }

        private void editorBox_DragDrop(object sender, DragEventArgs e)
        {
			DockingService.MainForm.MainFormRedone_DragDrop(sender, e);
        }

        private void editorBox_DragEnter(object sender, DragEventArgs e)
        {
			DockingService.MainForm.MainFormRedone_DragEnter(sender, e);
        }

		const int CacheSize = 100;
		private List<IParserData> labelsCache = new List<IParserData>(CacheSize);

        private void bgotoButton_Click(object sender, EventArgs e)
        {
            string text = bgotoButton.Text.Substring(5, bgotoButton.Text.Length - 5);
            bool isMacro = text[text.Length - 1] == '(';
            if (isMacro)
                text.Remove(text.Length - 2);
            //if (!ProjectService.IsInternal)
            //    GlobalClass.outputWindow.outputWindowBox.Text = text + "\nProjectLabels:" + GlobalClass.project.projectLabels.Count +
            //                                "\nProjectLabels[1]:" + ((ArrayList)GlobalClass.project.projectLabels[1]).Count;
            if (bgotoButton.Text.Substring(0, 4) == "Goto")
            {
				IParserData parserData = null;
				foreach (IParserData data in labelsCache)
					if (data.Name.ToLower() == text.ToLower())
					{
						parserData = data;
						break;
					}

				if (parserData == null)
				{
					foreach (ParserInformation info in ProjectService.ParseInfo)
						foreach (IParserData data in info)
							if (data.Name.ToLower() == text.ToLower())
							{
								parserData = data;
								break;
							}
				}
				if (parserData == null)
				{
					MessageBox.Show("Unable to locate " + text);
					return;
				}
				DocumentService.GotoLabel(parserData);
				if (!labelsCache.Contains(parserData))
					labelsCache.Add(parserData);
            }
            else
            {
                if (Path.IsPathRooted(text))
                    DocumentService.GotoFile(text);
                else
                    DocumentService.GotoFile(Path.Combine(Path.GetDirectoryName(editorBox.FileName), text));
            }
        }

        private void editorBox_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right)
                return;
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
            if (editorBox.Text != "")
            {
                int offset = editorBox.ActiveTextAreaControl.Caret.Offset;
                //if the user clicked after the last char we need to catch this
                if (offset == editorBox.Text.Length)
                    offset--;
                while (" \t\r\n()+-*/,;".IndexOf(editorBox.Text.Substring(offset, 1)) == -1 && offset != 0)
                    offset--;
                offset++;
                int length = 1;
                while (offset + length != editorBox.Text.Length && " \t\r\n()+-*/,;".IndexOf(editorBox.Text.Substring(offset + length, 1)) == -1)
                    length++;
                string gotoLabel = "";
                try
                {
                    gotoLabel = editorBox.Document.GetText(offset, length);
                    if (offset + length < editorBox.Text.Length && editorBox.Text[offset + length + 1] == '(')
                        gotoLabel += "(";
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Somewhere I fucked up the offset:" + ex);
                }
                if (!Settings.Default.caseSensitive)
                    gotoLabel = gotoLabel.ToLower();
                int num;
                if ("afbcdehlixiypcspnzncpm".IndexOf(gotoLabel) == -1 && !int.TryParse(gotoLabel, out num))
                {
                    if (gotoLabel[0] == '\"' && gotoLabel[gotoLabel.Length - 1] == '\"')
                    {
                        gotoLabel = gotoLabel.Substring(1, gotoLabel.Length - 2);
                        bool exists = Path.IsPathRooted(gotoLabel) ? File.Exists(gotoLabel) : File.Exists(Path.Combine(Path.GetDirectoryName(editorBox.FileName), gotoLabel));
                        if (exists)
                        {
                            bgotoButton.Text = "Open " + gotoLabel;
                            bgotoButton.Enabled = true;
                        }
                    }
                    else
                    {
                        bgotoButton.Text = "Goto " + gotoLabel;
                        bgotoButton.Enabled = true;
                    }
                }
                else
                    bgotoButton.Text = "Goto ";
            }
            contextMenu.Show(editorBox, editorBox.PointToClient(MousePosition));
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

        private void closeAllMenuItem_Click(object sender, EventArgs e)
        {
            foreach (Form child in DockingService.Documents)
            {
                if (child != this)
                    child.Close();
            }
        }

        private void copyPathMenuItem_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(editorBox.FileName);
        }

        private void openFolderMenuItem_Click(object sender, EventArgs e)
        {
            Process explorer = new Process
                                   {
                                       StartInfo = {FileName = Path.GetDirectoryName(editorBox.FileName)}
                                   };
            explorer.Start();
        }
        #endregion

        private delegate void updateCodeInfo(string size, string min, string max);
        private void GetCodeInfo(string lines)
        {
            try
            {
                Process wabbitspasm = new Process
                {
                    StartInfo =
                    {
                        FileName = "spasm.exe",
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        UseShellExecute = false,
                        CreateNoWindow = true,
                    }
                };
				string [] outputlines = null;
				string size = "0", min = "0", max = "0";
                //setup wabbitspasm to run silently
				if (!string.IsNullOrEmpty(lines))
				{
					//lines = lines.Replace('\n', '\\');
					//lines = lines.Replace('\r', ' ');
					wabbitspasm.StartInfo.Arguments = "-C -O -V \"" + lines + "\"";
					wabbitspasm.Start();
					while (!wabbitspasm.StartInfo.RedirectStandardError)
					{
						System.Threading.Thread.Sleep(500);
						wabbitspasm.StartInfo.RedirectStandardError = true;
					}
					outputlines = wabbitspasm.StandardError.ReadToEnd().Split('\n');
				}

				if (outputlines != null && !string.IsNullOrEmpty(outputlines[0]))
				{
					size = outputlines[0].Substring(6, outputlines[0].Length - 7);
					min = outputlines[1].Substring(20, outputlines[1].Length - 21);
					max = outputlines[2].Substring(20, outputlines[2].Length - 21);
				}

				updateCodeInfo update = DockingService.MainForm.UpdateCodeInfo;
				DockingService.MainForm.Invoke(update, new[] { size, min, max });
				RequestUpdate.UpdateCodeInfo = false;
            }
            catch (Exception) { }
        }

        private void setNextStateMenuItem_Click(object sender, EventArgs e)
        {
            DebuggerService.SetPCToSelect(editorBox.FileName, editorBox.ActiveTextAreaControl.Caret.Line);
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
	}

    class CodeCompletionProvider : ICompletionDataProvider
    {
        readonly newEditor mainForm;
        readonly TextEditorControl editorBox;

        public CodeCompletionProvider(newEditor mainForm)
        {
            this.mainForm = mainForm;
            editorBox = mainForm.editorBox;
        }

        public ImageList ImageList
        {
            get
            {
                return mainForm.imageList1;
            }
        }       

        public string PreSelection
        {
            get
            {
                return null;
            }
        }

        public int DefaultIndex
        {
            get
            {
                return -1;
            }
        }

        public CompletionDataProviderKeyResult ProcessKey(char key)
        {
            if (char.IsLetterOrDigit(key) || key == '\"' || key == '.' || key == '(' || key == '_')
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

        private string getLine(int start)
        {
            if (editorBox.Text == "" || start < 0)
                return "";
            if (start == editorBox.Text.Length)
                start-=2;
            while (start >= 0 && editorBox.Text[start] != '\n')
                start--;
            start++;
            int end = start;
            //comment effectively ends the line (for our purposes)
            while (end < editorBox.Text.Length && editorBox.Text[end] != '\n' && editorBox.Text[end] != ';')
                end++;
            end--;
			if (end - start == -1)
				return "";
            string line = editorBox.Text.Substring(start, end - start);
            return line;
        }

        private string getOpcodeOrMacro(string line)
        {
            int start = 0;
            while (start < line.Length && char.IsWhiteSpace(line[start]))
                start++;
            int end = start + 1;
            while(end < line.Length && !char.IsWhiteSpace(line[end]))
                end++;
            if (end > line.Length)
                return "";
            return line.Substring(start, end - start);
        }

        private string getFirstArg(string line, int offset, char charTyped)
        {
            int start = 0;
            while (start < line.Length && char.IsWhiteSpace(line[start]))
                start++;
            start += offset;
            int end = start;
            while (end < line.Length && line[end] != ',')
                end++;
            end--;
            return (line[end] != ',' && "(),+-*/".IndexOf(charTyped) == -1) || end - start < 1 ? "" : line.Substring(start + 1, end - start);
        }

        public ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped)
        {
            // We can return code-completion items like this:

            //return new ICompletionData[] {
            //	new DefaultCompletionData("Text", "Description", 1)
            //};
            /*List<ICompletionData> resultList = new List<ICompletionData>();
            int startOffset = editorBox.ActiveTextAreaControl.Caret.Offset;
            int lineNumber = editorBox.Document.GetLineNumberForOffset(startOffset);
            List<FoldMarker> foldings = editorBox.Document.FoldingManager.GetFoldingsContainsLineNumber(lineNumber);
            bool isInComment = false;
            foreach (FoldMarker folder in foldings)
                isInComment = folder.InnerText.ToLower().Contains("#endcomment");
            string line = getLine(startOffset);
            int start = editorBox.Document.OffsetToPosition(startOffset).Column;
            if (line.Length == 0 || line.Length < start || isInComment || char.IsLetterOrDigit(line[0]))
                return resultList.ToArray();
            switch (charTyped)
            {
                case '#':
                    return new List<ICompletionData>
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
                               }.ToArray();
                case '.':
                    return new List<ICompletionData>
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
                               }.ToArray();
                default:
                    {
                        string command = getOpcodeOrMacro(line);
                        string firstArg = getFirstArg(line, command.Length, charTyped);
                        switch (command.ToLower())
                        {
                            case "ld":
                                if (firstArg == "")
                                {
                                    resultList.Add(new CodeCompletionData("bc", 3));
                                    resultList.Add(new CodeCompletionData("de", 3));
                                    resultList.Add(new CodeCompletionData("hl", 3));
                                    resultList.Add(new CodeCompletionData("ix", 3));
                                    resultList.Add(new CodeCompletionData("iy", 3));
                                    resultList.Add(new CodeCompletionData("sp", 3));
                                    goto registers8Bit;
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
                                        case "sp":
                                            if (firstArg == "sp")
                                                resultList.Add(new CodeCompletionData("hl", 3));
											foreach (ArrayList file in ProjectService.Project.projectLabels)
                                            {
                                                ArrayList labels = (ArrayList)file[0];
                                                ArrayList props = (ArrayList)file[1];
                                                ArrayList description = (ArrayList)file[2];
                                                int counter = -1;
                                                foreach (string label in labels)
                                                {
                                                    counter++;
                                                    if (label == "_")
                                                        continue;
                                                    if (props[counter].ToString().Contains("|label|"))
                                                        resultList.Add(new CodeCompletionData(label, 4,
                                                                                              description[labels.IndexOf(label)].
                                                                                                  ToString()));
                                                    else if (props[counter].ToString().Contains("|equate|"))
                                                        resultList.Add(new CodeCompletionData(label, 5,
                                                                                              description[labels.IndexOf(label)].
                                                                                                  ToString()));
                                                }
                                            }
                                            break;
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
											foreach (ArrayList file in ProjectService.Project.projectLabels)
                                            {
                                                ArrayList labels = (ArrayList)file[0];
                                                ArrayList props = (ArrayList)file[1];
                                                ArrayList description = (ArrayList)file[2];
                                                int counter = -1;
                                                foreach (string label in labels)
                                                {
                                                    counter++;
                                                    if (label == "_")
                                                        continue;
                                                    if (props[counter].ToString().Contains("|equate|"))
                                                        resultList.Add(new CodeCompletionData(label, 5, description[labels.IndexOf(label)].ToString()));
                                                }
                                            }
                                            goto registers8Bit;
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
                                if (firstArg == "")
                                {
                                    resultList.Add(new CodeCompletionData("a", 3));
                                    resultList.Add(new CodeCompletionData("(C)", 3));
                                }
                                else if (int.TryParse(firstArg, out temp))
                                    resultList.Add(new CodeCompletionData("(C)", 3));
                                return resultList.ToArray();
                            case "bit":
                            case "set":
                            case "res":
                                if (firstArg == "")
                                {
									foreach (ArrayList file in ProjectService.Project.projectLabels)
                                    {
                                        ArrayList labels = (ArrayList)file[0];
                                        ArrayList props = (ArrayList)file[1];
                                        ArrayList description = (ArrayList)file[2];
                                        int counter = -1;
                                        foreach (string label in labels)
                                        {
                                            counter++;
                                            if (label == "_")
                                                continue;
                                            if (props[counter].ToString().Contains("|equate|"))
                                                resultList.Add(new CodeCompletionData(label, 5, description[labels.IndexOf(label)].ToString()));
                                        }
                                    }
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
                                    goto registers8Bit;
                                return resultList.ToArray();
                            case "add":
                            case "adc":
                            case "sbc":
                                if (firstArg == "")
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
                                    if (!(firstArg == "hl" || ((firstArg == "ix" || firstArg == "iy") && command == "add")))
                                        goto registers8Bit;
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
                            case "sub":
                            case "xor":
                        registers8Bit:
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
                                return resultList.ToArray();
                            //16 bit only
                            case "push":
                            case "pop":
                                resultList.Add(new CodeCompletionData("af", 3));
                                resultList.Add(new CodeCompletionData("bc", 3));
                                resultList.Add(new CodeCompletionData("de", 3));
                                resultList.Add(new CodeCompletionData("hl", 3));
                                resultList.Add(new CodeCompletionData("ix", 3));
                                resultList.Add(new CodeCompletionData("iy", 3));
                                return resultList.ToArray();
                            //labels/equates and conditions
                            case "call":
                            case "jp":
                            case "jr":
                            case "ret":
                            case "djnz":
                                //possible to have conditions
                                if (command != "djnz" && firstArg == "")
                                {
                                    resultList.Add(new CodeCompletionData("z", 2));
                                    resultList.Add(new CodeCompletionData("nz", 2));
                                    resultList.Add(new CodeCompletionData("c", 2));
                                    resultList.Add(new CodeCompletionData("nc", 2));
                                    if (command != "jr")
                                    {
                                        resultList.Add(new CodeCompletionData("p", 2));
                                        resultList.Add(new CodeCompletionData("m", 2));
                                        resultList.Add(new CodeCompletionData("n", 2));
                                        resultList.Add(new CodeCompletionData("po", 2));
                                        resultList.Add(new CodeCompletionData("pe", 2));
                                        resultList.Add(new CodeCompletionData("v", 2));
                                        resultList.Add(new CodeCompletionData("nv", 2));
                                    }
                                }
                                if (command == "ret")
                                    return resultList.ToArray();

								foreach (ArrayList file in ProjectService.Project.projectLabels)
                                {
                                    ArrayList labels = (ArrayList) file[0];
                                    ArrayList props = (ArrayList) file[1];
                                    ArrayList description = (ArrayList) file[2];
                                    int counter = -1;
                                    foreach (string label in labels)
                                    {
                                        counter++;
                                        if (label == "_")
                                            continue;
                                        if (props[counter].ToString().Contains("|label|"))
                                            resultList.Add(new CodeCompletionData(label, 4,
                                                                                  description[labels.IndexOf(label)].
                                                                                      ToString()));
                                        else if (props[counter].ToString().Contains("|equate|"))
                                            resultList.Add(new CodeCompletionData(label, 5,
                                                                                  description[labels.IndexOf(label)].
                                                                                      ToString()));
                                    }
                                }
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
						#region OldAutoComplete
						/*
                        if (startOffset == editorBox.Text.Length)
                            startOffset--;
                        if (Control.ModifierKeys == Keys.Control)
                        {
                            while (",( .#\t".IndexOf(editorBox.Text[startOffset]) == -1)
                                startOffset--;
                            startOffset--;
                        }
                        while (startOffset == editorBox.Text.Length || char.IsWhiteSpace(editorBox.Text[startOffset]))
                            startOffset--;
                        string word1 = getPrevWord(startOffset);
                        // string word2 = getPrevWord(startOffset - word1.Length - 1);
                        if (newline && word1 == "")
                            return new List<ICompletionData>
                                       {
                                           new CodeCompletionData("adc", 7),
                                           new CodeCompletionData("add", 7),
                                           new CodeCompletionData("bit", 7),
                                           new CodeCompletionData("call", 7),
                                           new CodeCompletionData("ccf", 7),
                                           new CodeCompletionData("cpdr", 7),
                                           new CodeCompletionData("cpd", 7),
                                           new CodeCompletionData("cpir", 7),
                                           new CodeCompletionData("cpi", 7),
                                           new CodeCompletionData("cpl", 7),
                                           new CodeCompletionData("cp", 7),
                                           new CodeCompletionData("daa", 7),
                                           new CodeCompletionData("dec", 7),
                                           new CodeCompletionData("di", 7),
                                           new CodeCompletionData("djnz", 7),
                                           new CodeCompletionData("ei", 7),
                                           new CodeCompletionData("exx", 7),
                                           new CodeCompletionData("ex", 7),
                                           new CodeCompletionData("halt", 7),
                                           new CodeCompletionData("im", 7),
                                           new CodeCompletionData("inc", 7),
                                           new CodeCompletionData("indr", 7),
                                           new CodeCompletionData("ind", 7),
                                           new CodeCompletionData("inir", 7),
                                           new CodeCompletionData("ini", 7),
                                           new CodeCompletionData("in", 7),
                                           new CodeCompletionData("jp", 7),
                                           new CodeCompletionData("jr", 7),
                                           new CodeCompletionData("lddr", 7),
                                           new CodeCompletionData("ldd", 7),
                                           new CodeCompletionData("ldir", 7),
                                           new CodeCompletionData("ldi", 7),
                                           new CodeCompletionData("ld", 7),
                                           new CodeCompletionData("neg", 7),
                                           new CodeCompletionData("nop", 7),
                                           new CodeCompletionData("or", 7),
                                           new CodeCompletionData("otdr", 7),
                                           new CodeCompletionData("otir", 7),
                                           new CodeCompletionData("outd", 7),
                                           new CodeCompletionData("outi", 7),
                                           new CodeCompletionData("out", 7),
                                           new CodeCompletionData("pop", 7),
                                           new CodeCompletionData("push", 7),
                                           new CodeCompletionData("res", 7),
                                           new CodeCompletionData("reti", 7),
                                           new CodeCompletionData("retn", 7),
                                           new CodeCompletionData("ret", 7),
                                           new CodeCompletionData("rla", 7),
                                           new CodeCompletionData("rlca", 7),
                                           new CodeCompletionData("rlc", 7),
                                           new CodeCompletionData("rld", 7),
                                           new CodeCompletionData("rl", 7),
                                           new CodeCompletionData("rra", 7),
                                           new CodeCompletionData("rrca", 7),
                                           new CodeCompletionData("rrc", 7),
                                           new CodeCompletionData("rrd", 7),
                                           new CodeCompletionData("rr", 7),
                                           new CodeCompletionData("rst", 7),
                                           new CodeCompletionData("sbc", 7),
                                           new CodeCompletionData("scf", 7),
                                           new CodeCompletionData("set", 7),
                                           new CodeCompletionData("sla", 7),
                                           new CodeCompletionData("sll", 7),
                                           new CodeCompletionData("sra", 7),
                                           new CodeCompletionData("srl", 7),
                                           new CodeCompletionData("sub", 7),
                                           new CodeCompletionData("xor", 7)
                                       }.ToArray();
                        if (word1[word1.Length - 1] == ',')
                            word1 = word1.Remove(word1.Length - 1);
                        if (word1.ToLower() == "#include")
                        {
                            if (!File.Exists(editorBox.FileName))
                                return null;
                            string[] dirs = (Settings.Default.includeDir + '\n' + Path.GetDirectoryName(editorBox.FileName)).Split('\n');
                            foreach (string dir in dirs)
                            {
                                if (dir == "")
                                    continue;
                                string[] files = Directory.GetFiles(dir);
                                foreach (string file in files)
                                {
                                    if (((newEditor)mainForm).names == null)
                                        continue;
                                    foreach (string test in ((newEditor)mainForm).names)
                                        if (test == file || test == file.Remove(0, dir.Length + 1))
                                            continue;
                                    if ((file.EndsWith(".asm") || file.EndsWith(".inc") || file.EndsWith(".bmp") ||
                                         file.EndsWith(".z80")))
                                        resultList.Add(new CodeCompletionData("\"" + file.Remove(0, dir.Length + 1) + "\"", 0));
                                }
                            }
                            return resultList.ToArray();
                        }
                        //Array sixteenbit = {"djnz", "call", "jp", "jr", "hl", "bc", "de", "iy", "ix"};

                        string[] regsOps = {
                                               "adc", "add", "and", "bit", "cp", "dec", "inc", "in", "ld", "or",
                                               "out", "pop", "push", "res", "rlc", "rl", "rrca", "rrc", "rr", "sbc",
                                               "set", "sla", "sll", "sra", "srl", "sub", "xor", "af", "bc", "de", "hl",
                                               "ix", "iy", "a", "b", "c", "d", "e", "h", "l"
                                           };
                        if (Array.IndexOf(regsOps, word1) != -1)
                        {
                            string[] registers = {
                                                     "af", "bc", "de", "hl", "ixh", "ixl", "iyh", "iyl", "ix",
                                                     "iy", "a", "f", "b", "c", "d", "e", "h", "l", "r"
                                                 };
                            foreach (string register in registers)
                            {
                                resultList.Add(new CodeCompletionData(register, 2));
                            }
                        }
                    }
						#endregion
						break;
                    }
            }*/
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
            Debug.WriteLine(xmlDoc);
            StringBuilder b = new StringBuilder();
            try
            {
                using (XmlTextReader reader = new XmlTextReader(new StringReader("<root>" + xmlDoc + "</root>")))
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
        readonly newEditor mainForm;
        readonly TextEditorControl editor;
        CodeCompletionWindow codeCompletionWindow;

        private CodeCompletionKeyHandler(newEditor mainForm, TextEditorControl editor)
        {
            this.mainForm = mainForm;
            this.editor = editor;
        }

        public static CodeCompletionKeyHandler Attach(newEditor mainForm, TextEditorControl editor)
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

            Stack<int> regionLines = new Stack<int>();
            Stack<int> ifLines = new Stack<int>();
            Stack<int> macroLines = new Stack<int>();
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
                string text = document.GetText(offs, seg.Length - spaceCount).ToLower();
                if (text.StartsWith("#region"))
                    regionLines.Push(i);
                if (text.StartsWith("#ifdef") || text.StartsWith("#if") || text.StartsWith("#ifndef"))
                    ifLines.Push(i);
                if (text.StartsWith("#macro"))
                    macroLines.Push(i);
                if (text.StartsWith("#comment"))
                    commentLines.Push(i);
                if (text.StartsWith("#endregion") && regionLines.Count > 0)
                {
                    // Add a new FoldMarker to the list.
                    int start = regionLines.Pop();
                    list.Add(new FoldMarker(document, start, document.GetLineSegment(start).Length, i,
                                            spaceCount + "#endregion".Length));
                }
                if (text.StartsWith("#else") && ifLines.Count > 0)
                {
                    // Add a new FoldMarker to the list.
                    int start = ifLines.Pop();
                    list.Add(new FoldMarker(document, start,
                                            document.GetLineSegment(start).Length,
                                            i-1, spaceCount + "#else".Length));
                    ifLines.Push(i);
                }
                if (text.StartsWith("#endif") && ifLines.Count > 0)
                {
                    // Add a new FoldMarker to the list.
                    int start = ifLines.Pop();
                    list.Add(new FoldMarker(document, start,
                                            document.GetLineSegment(start).Length,
                                            i, spaceCount + "#endif".Length));
                }
                if (text.StartsWith("#endmacro") && macroLines.Count > 0)
                {
                    // Add a new FoldMarker to the list.
                    int start = macroLines.Pop();
                    list.Add(new FoldMarker(document, start,
                                            document.GetLineSegment(start).Length,
                                            i, spaceCount + "#endregion".Length));
                }
                if (text.StartsWith("#endcomment") && commentLines.Count > 0)
                {
                    // Add a new FoldMarker to the list.
                    int start = commentLines.Pop();
                    list.Add(new FoldMarker(document, start,
                                            document.GetLineSegment(start).Length,
                                            i, spaceCount + "#endcomment".Length));
                }
            }
            return list;
        }
    }
}
