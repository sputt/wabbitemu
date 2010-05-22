//#define USE_DLL
//#define NEW_DEBUGGING
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Net;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;
using System.Xml;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
    public partial class MainFormRedone : Form
    {
        //private ProjectClass project;
        
        private readonly ToolbarManager.ToolBarManager toolBarManager;

        //[DllImport("libWabbitemu.dll")]
        //public static extern int ShowMessage();

        public MainFormRedone(string[] args)
        {
            InitializeComponent();
			if (Settings.Default.firstRun)
			{
				Settings.Default.Upgrade();
				Settings.Default.firstRun = false;
				Settings.Default.Save();
			}
			//ShowMessage();

			toolBarManager = new ToolbarManager.ToolBarManager(this, this);
			if (Settings.Default.mainToolBar)
				toolBarManager.AddControl(mainToolBar);
			if (Settings.Default.debugToolbar)
				toolBarManager.AddControl(debugToolStrip);
			GlobalClass.errorsInFiles = new List<Errors>();
			if (ProjectService.IsInternal)
				ProjectService.CreateInternalProject();

			DockingService.InitDocking(dockPanel);
			DockingService.InitPanels();
			HighlightingClass highlighting = new HighlightingClass();
			highlighting.makeHighlightingFile();
			DockingService.LoadConfig();
			HandleArgs(args);
			UpdateMenus(false);
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.spasm.exe", "spasm.exe");
            //MessageBox.Show("There are " + calcCount() + " calcs running.");
            //newCalc();
            //romLoad(0, @"C:\Documents and Settings\LOCAL-ADMIN\Desktop\ti84pse.rom");
            //MessageBox.Show("There are " + calcCount() + " calcs running.");
            //drawCalc(0, pictureBox1.Handle);
            //ShowMessage();
            //removeCalc(0);

            DocumentService.GetRecentFiles();
            UpdateChecks();
			Settings.Default.firstRun = false;
        }

		private void HandleArgs(string[] args)
		{
			if (args.Length != 0)
			{
				foreach (string file in args)
					if (File.Exists(file))
						if (file.EndsWith(".wcodeproj"))
						{
							ProjectService.OpenProject(file);
							return;
						}
						else
							foreach (string arg in args)
								try
								{
									newEditor doc = new newEditor();
									if (string.IsNullOrEmpty(arg))
										break;
									DocumentService.OpenDocument(doc, arg);
								}
								catch (FileNotFoundException)
								{
									MessageBox.Show("Error: File not found!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
								}
								catch (Exception ex)
								{
									MessageBox.Show("Error: " + ex);
								}
			} else if (Settings.Default.startupProject != "")
				if (File.Exists(Settings.Default.startupProject))
				{
					ProjectService.OpenProject(Settings.Default.startupProject);
				}
				else
				{
					Settings.Default.startupProject = "";
					MessageBox.Show("Error: Project file not found!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				}
		}
        //[DllImport("libWabbitemu.dll")]
        //private static extern void ShowMessage();
        
        /*[DllImport("libWabbitemu.dll")]
        private static extern int newCalc();
        [DllImport("libWabbitemu.dll")]
        private static extern int calcCount();
        [DllImport("libWabbitemu.dll")]
        private static extern int romLoad(int slot, string FileName);
        [DllImport("libWabbitemu.dll")]
        private static extern int removeCalc(int slot);
        [DllImport("libWabbitemu.dll")]
        private static extern int resetCalc(int slot);
        [DllImport("libWabbitemu.dll")]
        private static extern int drawCalc(int slot, IntPtr handle);*/
        /// <summary>
        /// Updates all the menu items that depend on if there is an active child open. 
        /// </summary>
        /// <param name="enabled">Whether items should be enabled or disabled.</param>
		private void UpdateMenus(bool enabled)
		{
			//Main Toolbar
			saveToolStripButton.Enabled = enabled;
			saveAllToolButton.Enabled = enabled;
			cutToolStripButton.Enabled = enabled;
			copyToolStripButton.Enabled = enabled;
			pasteToolStripButton.Enabled = enabled;
			findBox.Enabled = enabled;
			if (ProjectService.IsInternal)
				runToolButton.Enabled = enabled;
			else
				runToolButton.Enabled = true;

			//File Menu
			saveMenuItem.Enabled = enabled;
			saveAsMenuItem.Enabled = enabled;
			saveAllMenuItem.Enabled = enabled;
			closeMenuItem.Enabled = enabled;

			//Edit Menu
			undoMenuItem.Enabled = enabled;
			redoMenuItem.Enabled = enabled;
			cutMenuItem.Enabled = enabled;
			copyMenuItem.Enabled = enabled;
			pasteMenuItem.Enabled = enabled;
			selectAllMenuItem.Enabled = enabled;
			findMenuItem.Enabled = enabled;
			replaceMenuItem.Enabled = enabled;
			makeLowerMenuItem.Enabled = enabled;
			makeUpperMenuItem.Enabled = enabled;
			invertCaseMenuItem.Enabled = enabled;
			sentenceCaseMenuItem.Enabled = enabled;
			toggleBookmarkMenuItem.Enabled = enabled;
			nextBookmarkMenuItem.Enabled = enabled;
			prevBookmarkMenuItem.Enabled = enabled;
			
			//View Menu
			lineNumMenuItem.Enabled = enabled;
			iconBarMenuItem.Enabled = enabled;

			//Assemble Menu
			if (ProjectService.IsInternal)
				assembleMenuItem.Enabled = enabled;
			else
				assembleMenuItem.Enabled = true;
			symTableMenuItem.Enabled = enabled;
			projStatsMenuItem.Enabled = enabled;
			listFileMenuItem.Enabled = enabled;

			//Debug Menu
			if (!enabled)
				UpdateDebugStuff();
			toggleBreakpointMenuItem.Enabled = enabled;
			if (ProjectService.IsInternal)
				startDebugMenuItem.Enabled = enabled;
			else
				startDebugMenuItem.Enabled = true;
		}

        /// <summary>
        /// Updates the code info with the latest size, min, and max string in the status bar.
        /// </summary>
        /// <param name="size"></param>
        /// <param name="min"></param>
        /// <param name="max"></param>
        public void UpdateCodeInfo(string size, string min, string max)
        {
            lineCodeInfo.Text = "Min:" + min + " Max:" + max + " Size:" + size;
        }

        /// <summary>
        /// Updates the title of the app with the filename.
        /// </summary>
        public void UpdateTitle()
        {
			string debugString = DebuggerService.IsDebugging ? " (Debugging)" : "";
            if (DocumentService.ActiveFileName != null)
                Text = Path.GetFileName(DocumentService.ActiveFileName) + debugString + " - Wabbitcode";
            else
                Text = "Wabbitcode" + debugString;
        }

        private void MainFormRedone_FormClosing(object sender, FormClosingEventArgs e)
        {
#if NEW_DEBUGGING

#else
			if (DebuggerService.IsDebugging)
				DebuggerService.CancelDebug();
#endif
            if (!ProjectService.IsInternal)
                closeProjMenuItem_Click(null, null);
			//HACK: I is creating a security flaw here. Probably should move settings elsewhere where you actually have permissions, but meh w/e
			/*try
			{
				DirectorySecurity security = Directory.GetAccessControl(Path.GetDirectoryName(configFile));
				FileSystemAccessRule rule = new FileSystemAccessRule(@"BUILTIN\Users", FileSystemRights.FullControl, AccessControlType.Allow);
				security.AddAccessRule(rule);
				Directory.SetAccessControl(Path.GetDirectoryName(configFile), security);
			}
			catch (Exception) { }*/
			DockingService.Destroy();
			/*try
			{
				DirectorySecurity security = Directory.GetAccessControl(configFile);
				FileSystemAccessRule rule = new FileSystemAccessRule(@"BUILTIN\Users", FileSystemRights.FullControl, AccessControlType.Allow);
				security.RemoveAccessRule(rule);
				Directory.SetAccessControl(configFile, security);
			}
			catch (Exception) { }*/
            Settings.Default.Save();
        }

        private void dockPanel_ActiveDocumentChanged(object sender, EventArgs e)
        {
            if (Disposing)
                return;
            if (ActiveMdiChild != null)
            {
				UpdateMenus(true);
				DockingService.ActiveDocument.UpdateLabelBox();
            }
            else
            {
				UpdateMenus(false);
				DockingService.LabelList.ClearLabels();
            }
            UpdateTitle();
        }

        public void MainFormRedone_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Copy : DragDropEffects.None;
        }

        public void MainFormRedone_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop) == false)
                return;
            string[] files = (string[]) e.Data.GetData(DataFormats.FileDrop);
            foreach (string file in files)
            {
                newEditor doc = DocumentService.CreateNewDocument();
                doc.Text = Path.GetFileName(file);
                doc.TabText = Path.GetFileName(file);
                doc.ToolTipText = file;
				DockingService.ShowDockPanel(doc);
                doc.OpenFile(file);
            }
        }

        private void findBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != (char) Keys.Enter) 
                return;
            if (ActiveMdiChild == null)
                return;
            if (!findBox.Items.Contains(findBox.Text))
                findBox.Items.Add(findBox.Text);
            TextEditorControl editorBox = ((newEditor) ActiveMdiChild).editorBox;
			int startOffset = editorBox.ActiveTextAreaControl.Caret.Offset + findBox.Text.Length;
			if (startOffset > editorBox.Text.Length)
				startOffset = 0;
			int newOffset = editorBox.Text.IndexOf(findBox.Text, startOffset);
            if (newOffset == -1)
                MessageBox.Show("Text Not Found");
            else
            {
                int line =
                    editorBox.ActiveTextAreaControl.Caret.Line =
                    editorBox.Document.GetLineNumberForOffset(newOffset);
                int col = editorBox.Text.Split('\n')[line].IndexOf(findBox.Text);
                editorBox.ActiveTextAreaControl.Caret.Column = findBox.Text.Length + col;
                TextLocation start = new TextLocation(col, line);
                TextLocation end = new TextLocation(findBox.Text.Length + col, line);
                editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(start, end);
                editorBox.ActiveTextAreaControl.ScrollTo(line);
            }
        }

        private void addNewFileMenuItem_Click(object sender, EventArgs e)
        {
        }

        public void existingFileMenuItem_Click(object sender, EventArgs e)
        {
			OpenFileDialog openFileDialog = new OpenFileDialog()
			{
				CheckFileExists = true,
				DefaultExt = "*.asm",
				Filter = "All Know File Types | *.asm; *.z80;| Assembly Files (*.asm)|*.asm|Z80" +
						   " Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|All Files(*.*)|*.*",
				FilterIndex = 0,
				Multiselect = true,
				RestoreDirectory = true,
				Title = "Add Existing File",
			};
			DialogResult result = openFileDialog.ShowDialog();
			if (result != DialogResult.OK)
				return;
            TreeNode parent = DockingService.ProjectViewer.projViewer.SelectedNodes[0];
            if (parent == null)
                parent = DockingService.ProjectViewer.projViewer.Nodes[0];
            else if (parent.Tag.ToString().StartsWith("File"))
                parent = parent.Parent;
			foreach (string file in openFileDialog.FileNames)
			{
				TreeNode node = new TreeNode
									{
										Tag = "File|" + file.Remove(0, ProjectService.ProjectDirectory.Length) + "|",
										Text = Path.GetFileName(file),
										ImageIndex = 4,
										SelectedImageIndex = 5
									};
				parent.Nodes.Add(node);
			}
        }

        private void includeDirButton_Click(object sender, EventArgs e)
        {
            IncludeDir includes = new IncludeDir();
            includes.ShowDialog();
        }

        #region FileMenu

        private void newFileMenuItem_Click(object sender, EventArgs e)
        {
            newEditor doc = DocumentService.CreateNewDocument();
            doc.TabText = "New Document";
            doc.Show(dockPanel);
        }

        private void newProjectMenuItem_Click(object sender, EventArgs e)
        {
            templateForm template = new templateForm();
            if (template.ShowDialog() != DialogResult.OK) 
                return;
            ProjectService.OpenProject(template.projectFile);
        }

        private void openFileMenuItem_Click(object sender, EventArgs e)
        {
			DocumentService.OpenDocument();
        }

        private void openProjectMenuItem_Click(object sender, EventArgs e)
        {
			OpenFileDialog openFileDialog = new OpenFileDialog()
			{
				CheckFileExists = true,
				DefaultExt = "*.wcodeproj",
				Filter = "Project Files (*.wcodeproj)|*.wcodeproj|All Files(*.*)|*.*",
				FilterIndex = 0,
				RestoreDirectory = true,
				Title = "Open Project File",
			};
            if (openFileDialog.ShowDialog() == DialogResult.OK)
                ProjectService.OpenProject(openFileDialog.FileName);
        }

        private void saveMenuItem_Click(object sender, EventArgs e)
        {
            DocumentService.SaveDocument();
        }

        private void saveAsMenuItem_Click(object sender, EventArgs e)
        {
			DocumentService.SaveDocumentAs();
        }

        private void closeMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild != null)
                ActiveMdiChild.Close();
        }

        private void exitMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

		internal void IncrementProgress(int progress)
		{
			progressBar.Visible = true;
			progressBar.Increment(progress);
		}

        #endregion

        #region RecentItems

        /// <summary>
        /// This opens the recend document clicked in the file menu.
        /// </summary>
        /// <param name="sender">This is the button object. This is casted to get which button was clicked.</param>
        /// <param name="e">Nobody cares about this arg.</param>
        private void openRecentDoc(object sender, EventArgs e)
        {
            var button = (MenuItem) sender;
            var doc = new newEditor();
            String fileName = button.Text;
            doc.Text = Path.GetFileName(fileName);
            doc.TabText = Path.GetFileName(fileName);
            doc.ToolTipText = fileName;
            doc.MdiParent = this;
            doc.editorBox.ShowLineNumbers = Settings.Default.lineNumbers;
            doc.OpenFile(fileName);
            doc.editorBox.Font = Settings.Default.editorFont;
            doc.Show(dockPanel);
            //doc.highlight();
        }

		internal void AddRecentItem(string file)
		{
			MenuItem button = new MenuItem(file, openRecentDoc);
			recentFilesMenuItem.MenuItems.Add(button);
		}

        #endregion

        #region Edit Menu

        private void undoMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.editorBox.Undo();
        }

        private void redoMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.editorBox.Redo();
        }

        private void cutMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.editorBox.Cut();
        }

        private void copyMenuItem_Click(object sender, EventArgs e)
        {
			Copy();
        }

		private void Copy()
		{
			if (DockingService.ActiveContent == DockingService.OutputWindow)
				DockingService.OutputWindow.Copy();
			else if (DockingService.ActiveContent == DockingService.DebugPanel)
				DockingService.DebugPanel.Copy();
			else if (DockingService.ActiveContent == DockingService.ActiveDocument)
				DockingService.ActiveDocument.Copy();
			else if (dockPanel.ActiveContent == DockingService.FindResults)
				DockingService.FindResults.Copy();
			else if (DockingService.ActiveContent == DockingService.ErrorList)
				DockingService.ErrorList.Copy();
			else if (DockingService.ActiveContent == DockingService.TrackWindow)
				DockingService.TrackWindow.Copy();
			else if (DockingService.ActiveContent == DockingService.LabelList)
				DockingService.LabelList.Copy();
		}

        private void pasteMenuItem_Click(object sender, EventArgs e)
        {
			if (DockingService.ActiveContent == DockingService.DebugPanel)
				DockingService.DebugPanel.Paste();
			else if (DockingService.ActiveContent == DockingService.ActiveDocument)
                DockingService.ActiveDocument.editorBox.Paste();
			else if (DockingService.ActiveContent == DockingService.TrackWindow)
				DockingService.TrackWindow.Paste();
        }

        private void selectAllMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            TextEditorControl editorBox = DockingService.ActiveDocument.editorBox;
            var selectStart = new TextLocation(0, 0);
            var selectEnd =
                new TextLocation(editorBox.Text.Split('\n')[editorBox.Document.TotalNumberOfLines - 1].Length,
                                 editorBox.Document.TotalNumberOfLines - 1);
            editorBox.ActiveTextAreaControl.SelectionManager.SetSelection(selectStart, selectEnd);
        }

        private void findMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.editorBox, false, false);
        }

        private void findInFilesMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.editorBox, false, true);
        }

        private void replaceMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.editorBox, true, false);
        }

        private void replaceInFilesMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.editorBox, true, true);
        }

        private void makeUpperMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.ActiveDocument.editorBox.Document.UndoStack.StartUndoGroup();
            string newText = DockingService.ActiveDocument.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.SelectedText.ToUpper();
            DockingService.ActiveDocument.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.RemoveSelectedText();
            DockingService.ActiveDocument.editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
            DockingService.ActiveDocument.editorBox.Document.UndoStack.EndUndoGroup();
        }

        private void makeLowerMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild == null)
                return;
            newEditor child = ((newEditor) (ActiveMdiChild));
            child.editorBox.Document.UndoStack.StartUndoGroup();
            string newText = child.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.SelectedText.ToLower();
            child.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.RemoveSelectedText();
            child.editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
            child.editorBox.Document.UndoStack.EndUndoGroup();
        }

        private void invertCaseMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild == null)
                return;
            newEditor child = ((newEditor)(ActiveMdiChild));
            child.editorBox.Document.UndoStack.StartUndoGroup();
            string text = child.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.SelectedText;
            char[] textarray = text.ToCharArray();
            for (int i = 0; i < textarray.Length; i++)
            {
                if (textarray[i] >= 65 && textarray[i] <= 90)
                    textarray[i] = (char) (textarray[i] + 32);
                else if (textarray[i] >= 97 && textarray[i] <= 122)
                    textarray[i] = (char) (textarray[i] - 32);
            }
            child.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.RemoveSelectedText();
            child.editorBox.ActiveTextAreaControl.TextArea.InsertString(new string(textarray));
            child.editorBox.Document.UndoStack.EndUndoGroup();
        }

        private void sentenceCaseMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild == null)
                return;
            newEditor child = ((newEditor)(ActiveMdiChild));
            child.editorBox.Document.UndoStack.StartUndoGroup();
            string text = child.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.SelectedText;
            string newText = CultureInfo.CurrentCulture.TextInfo.ToTitleCase(text);
            child.editorBox.ActiveTextAreaControl.TextArea.SelectionManager.RemoveSelectedText();
            child.editorBox.ActiveTextAreaControl.TextArea.InsertString(newText);
            child.editorBox.Document.UndoStack.EndUndoGroup();
        }

        private void toggleBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild == null)
                return;
            ToggleBookmark toggle = new ToggleBookmark();
            toggle.Execute(((newEditor) ActiveMdiChild).editorBox.ActiveTextAreaControl.TextArea);
        }

        private void prevBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild == null)
                return;
			GotoPrevBookmark next = new GotoPrevBookmark(bookmark => true);
			next.Execute(((newEditor)ActiveMdiChild).editorBox.ActiveTextAreaControl.TextArea);
        }

        private void nextBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild == null)
                return;
            GotoNextBookmark next = new GotoNextBookmark(bookmark => true);
            next.Execute(((newEditor) ActiveMdiChild).editorBox.ActiveTextAreaControl.TextArea);
        }

        private void gLineMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            GotoLine gotoBox = new GotoLine(DockingService.ActiveDocument.editorBox.Document.TotalNumberOfLines);
            DialogResult gotoResult = gotoBox.ShowDialog();
            if (gotoResult != DialogResult.OK) 
                return;
            int line = Convert.ToInt32(gotoBox.inputBox.Text);
			DocumentService.GotoLine(line);
        }

        private void gLabelMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void prefsMenuItem_Click(object sender, EventArgs e)
        {
            var prefs = new Preferences();
            prefs.ShowDialog();
        }

        #endregion

        #region View Menu

        /// <summary>
        /// This handles all things relating to the view menu. Just does a switch based
        /// on the tag, and does the appropriate action based on the check mark state
        /// this probably isnt a great way to handle it, but it works
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void viewMenuItem_Click(object sender, EventArgs e)
        {
            MenuItem item = (MenuItem) sender;
            item.Checked = !item.Checked;
            switch (item.Tag.ToString())
            {
                case "iconBar":
                    if (ActiveMdiChild != null)
                        ((newEditor) ActiveMdiChild).editorBox.IsIconBarVisible = item.Checked;
                    break;
                case "lineNumbers":
                    if (ActiveMdiChild != null)
                        ((newEditor) ActiveMdiChild).editorBox.ShowLineNumbers = item.Checked;
                    break;
                case "labelsList":
					if (item.Checked)
						DockingService.ShowDockPanel(DockingService.LabelList);
					else
						DockingService.HideDockPanel(DockingService.LabelList);
                    break;
                case "mainToolBar":
                    if (item.Checked)
                        toolBarManager.AddControl(mainToolBar);
                    else
                        toolBarManager.RemoveControl(mainToolBar);
                    break;
				case "editorToolBar":
					if (item.Checked)
						toolBarManager.AddControl(editorToolStrip);
					else
						toolBarManager.RemoveControl(editorToolStrip);
					break;
                case "outputWindow":
					if (item.Checked)
						DockingService.ShowDockPanel(DockingService.OutputWindow);
					else
						DockingService.HideDockPanel(DockingService.OutputWindow);
                    break;
                case "FindResults":
					if (item.Checked)
						DockingService.ShowDockPanel(DockingService.FindResults);
					else
						DockingService.HideDockPanel(DockingService.FindResults);
                    break;
                case "statusBar":
                    statusBar.Visible = item.Checked;
                    break;
                case "debugPanel":
                    if (item.Checked)
						DockingService.ShowDockPanel(DockingService.DebugPanel);
                    else
						DockingService.HideDockPanel(DockingService.DebugPanel);
                    break;
                case "breakManager":
                    if (item.Checked)
						DockingService.ShowDockPanel(DockingService.BreakManager);
                    else
						DockingService.HideDockPanel(DockingService.BreakManager);
                    break;
                case "projectViewer":
                    if (item.Checked)
						DockingService.ShowDockPanel(DockingService.ProjectViewer);
                    else
						DockingService.HideDockPanel(DockingService.ProjectViewer);
                    break;
                case "directoryViewer":
                    if (item.Checked)
						DockingService.ShowDockPanel(DockingService.DirectoryViewer);
                    else
						DockingService.HideDockPanel(DockingService.DirectoryViewer);
                    break;
                case "debugToolBar":
					if (item.Checked)
					{
						if (!toolBarManager.ContainsControl(debugToolStrip))
							toolBarManager.AddControl(debugToolStrip);
					}
					else
						toolBarManager.RemoveControl(debugToolStrip);
                    break;
                case "errorList":
                    if (item.Checked)
						DockingService.ShowDockPanel(DockingService.ErrorList);
                    else
						DockingService.HideDockPanel(DockingService.ErrorList);
                    break;
				case "macroManager":
					if (item.Checked)
						DockingService.ShowDockPanel(DockingService.MacroManager);
					else
						DockingService.HideDockPanel(DockingService.MacroManager);
					break;
            }
            Settings.Default[item.Tag.ToString()] = item.Checked;
            debugToolStrip.Height = 25;
        }

        public void UpdateChecks()
        {
            mainToolMenuItem.Checked = Settings.Default.mainToolBar;
            labelListMenuItem.Checked = Settings.Default.labelsList;
            debugToolMenuItem.Checked = Settings.Default.debugToolbar;
            projViewMenuItem.Checked = Settings.Default.projectViewer;
            dirViewMenuItem.Checked = Settings.Default.directoryViewer;
            outWinMenuItem.Checked = Settings.Default.outputWindow;
            statusBarMenuItem.Checked = Settings.Default.statusBar;
            lineNumMenuItem.Checked = Settings.Default.lineNumbers;
            iconBarMenuItem.Checked = Settings.Default.iconBar;
            debugPanelMenuItem.Checked = Settings.Default.debugPanel;
            errListMenuItem.Checked = Settings.Default.errorList;
            findResultsMenuItem.Checked = Settings.Default.findResults;
            debugPanelMenuItem.Checked = Settings.Default.debugPanel;
        }

        #endregion

        #region Project Menu

        private void closeProjMenuItem_Click(object sender, EventArgs e)
        {
            ProjectService.CloseProject();
            DockingService.DirectoryViewer.dirViewer.Nodes.Clear();
            DockingService.ProjectViewer.projViewer.Nodes.Clear();
            projMenuItem.Visible = false;
            includeDirButton.Visible = true;
        }

        private void buildOrderButton_Click(object sender, EventArgs e)
        {
            BuildSteps build = new BuildSteps(ProjectService.ProjectFile);
            build.ShowDialog();
        }

        #endregion

        #region Assembly Code

        public enum MODE
        {
            MODE_NORMAL = 1,
            MODE_CODE_COUNTER = 2,
            MODE_SYMTABLE = 4,
            MODE_STATS = 8,
            MODE_LIST = 16,
            MODE_COMMANDLINE = 32
        }

        private readonly Process emulator = new Process();
        public string originaldir = "";

        private void assembleMenuItem_Click(object sender, EventArgs e)
        {
            if (!ProjectService.IsInternal)
                assembleProject();
            else if (DockingService.ActiveDocument != null)
            {
				bool saved = DockingService.ActiveDocument.SaveFile();
                if (saved)
                {
					string text = DockingService.ActiveDocument.FileName;
                    assembleCode(text, true, Path.ChangeExtension(text, GetExtension(Settings.Default.outputFile)));
                }
            }
        }

        private void symTableMenuItem_Click(object sender, EventArgs e)
        {
			if (DockingService.ActiveDocument == null)
                return;
			DockingService.ActiveDocument.SaveFile();
			String text = DockingService.ActiveDocument.editorBox.FileName;
            createSymTable(text, Path.ChangeExtension(text, "lab"));
        }

        private void countCodeMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void projStatsMenuItem_Click(object sender, EventArgs e)
        {
			if (DockingService.ActiveDocument == null)
                return;
			DockingService.ActiveDocument.SaveFile();
			String text = DockingService.ActiveDocument.FileName;
            createStatsFile(text);
        }

        private void listFileMenuItem_Click(object sender, EventArgs e)
        {
			if (DockingService.ActiveDocument == null)
                return;
			DockingService.ActiveDocument.SaveFile();
			String text = DockingService.ActiveDocument.FileName;
            createListing(text, Path.ChangeExtension(text, "lst"));
        }

        private void hexFileMenuItem_Click(object sender, EventArgs e)
        {
        }

#if USE_DLL
        //[DllImport("SPASM.dll")]
        //public static extern int ShowMessage();

        [DllImport("SPASM.dll")]
        public static extern int SetInputFileA([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll")]
        public static extern int SetOutputFileA([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll")]
        public static extern int RunAssembly();

        [DllImport("SPASM.dll")]
        public static extern int ClearDefines();

        [DllImport("SPASM.dll")]
        public static extern int AddDefine([In, MarshalAs(UnmanagedType.LPStr)] string name,
                                           [In, MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport("SPASM.dll")]
        public static extern int ClearIncludes();

        [DllImport("SPASM.dll")]
        public static extern int AddInclude([In, MarshalAs(UnmanagedType.LPStr)] string directory);

        [DllImport("SPASM.dll")]
        public static extern int SetMode(int mode);

        [DllImport("kernel32.dll")]
        public static extern IntPtr GetStdHandle(uint nStdHandle);

        [DllImport("Kernel32.dll")]//SetLastError = true
        public static extern bool SetStdHandle(uint device, IntPtr handle);
        //[DllImport("Kernel32.dll")]//SetLastError = true
        //public static extern uint GetStdHandle(uint device);
#endif
        private Process wabbitspasm;
        public bool assembleCode(string filePath, bool sendFile, string assembledName)
        {
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.spasm.exe", "spasm.exe");
            //Clear any other assemblings
            //outputWindow.outputWindowBox.Text = "";
            //Get our emulator
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.Wabbitemu.exe", "Wabbitemu.exe");
#if USE_DLL == false
            //create two new processes to run
            //setup wabbitspasm to run silently
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

            //some strings we'll need to build 
            originaldir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            string includedir = "-I \"" + Application.StartupPath + "\"";
            if (Settings.Default.includeDir != "" || !ProjectService.IsInternal)
            {
				string[] dirs = ProjectService.IsInternal ? Settings.Default.includeDir.Split('\n') : ProjectService.Project.getIncludeDirs();
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        includedir += ";\"" + dir + "\"";
                }
            }
            string fileName = Path.GetFileName(filePath);
                // filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            wabbitspasm.StartInfo.Arguments = includedir + " -T -L " + quote +  filePath + quote + " " + quote + assembledName + quote;
            wabbitspasm.StartInfo.WorkingDirectory = originaldir;
            wabbitspasm.Start();

#else
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            //ShowMessage();
            ClearIncludes();
            ClearDefines();
            AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                    if (dir != "")
                        AddInclude(dir);
            }
            //get the file name we'll use and use it to create the assembled name
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //assembledName = Path.ChangeExtension(fileName, getExtension(fileName));
            //now we can set the args for spasm
            int error = 0;
            //SetMode((int)MODE.MODE_SYMTABLE);
            error |= SetInputFileA(Path.Combine(originalDir, fileName));
            error |= SetOutputFileA(Path.Combine(originalDir, assembledName));
            //emulator setup
            //emulator.StartInfo.FileName = emuLoc;
            //assemble that fucker
            uint STD_ERROR_HANDLE = 0xFFFFFFF4;
            uint STD_OUTPUT_HANDLE = 0xFFFFFFF5;

            StreamWriter test = new StreamWriter(Application.StartupPath + "\\test.txt");
            //FileStream test = new FileStream(Application.StartupPath + "\\test.txt", FileMode.Create);
            //SetStdHandle(STD_ERROR_HANDLE, test.Handle);
            //IntPtr test2 = GetStdHandle(STD_OUTPUT_HANDLE);
            //SetStdHandle(STD_OUTPUT_HANDLE, test.Handle);
            //test2 = GetStdHandle(STD_OUTPUT_HANDLE);
            //Console.SetOut(test);
            //Console.SetError(test);
            //StreamReader reader = myprocess.StandardOutput;
            //Console.WriteLine("test line1");
            try
            {
                RunAssembly();
            }
            catch (Exception)
            {

            }
            //Console.WriteLine("test line2");
            //string tryread = reader.ReadToEnd();
            //string output = myprocess.StandardOutput.ReadToEnd();
            //test2 = GetStdHandle(STD_OUTPUT_HANDLE);
            test.Flush();
            test.Close();            
#endif
            //lets write it to the output window so the user knows whats happening
            string outputText = wabbitspasm.StartInfo.Arguments + "\n==================" +
                                                            fileName + "==================\r\n" +
                                                            "Assembling " + originaldir + "\\" + fileName + "\r\n" +
#if USE_DLL == false
                                                wabbitspasm.StandardOutput.ReadToEnd();
#endif
			DockingService.OutputWindow.SetText(outputText);
			DockingService.OutputWindow.HighlightOutput();
            bool errors = outputText.Contains("error");
            if (errors)
                DockingService.ShowDockPanel(DockingService.OutputWindow);
            //its more fun with colors
			DockingService.ErrorList.ParseOutput(outputText, originaldir);
            if (errors)
				DockingService.ShowDockPanel(DockingService.ErrorList);
            //we need to check for errors
            if (Settings.Default.sendFileEmu && sendFile && !errors)
                SendFileEmu(assembledName);
            //tell if the assembly was successful
            //if (error != 0)
            //    return false;
            //else
            //    return true;
            return !errors;
        }

        private bool error = true;
        public string assembleProject()
        {
            string outputText = ProjectService.ProjectDirectory;
			DockingService.OutputWindow.SetText(outputText);
            /*ArrayList*/ XmlNodeList buildConfigs = ProjectService.Project.getBuildConfigs();
            //buildConfigs.IndexOf(Settings.Default.buildConfig);
            //some strings we'll need to build 
            string sendToEmu = "";
            //string includeDir = "-I " + quote + ProjectService.ProjectDirectory + quote + " ";
#if USE_DLL
            /*ClearIncludes();
            ClearDefines();
            AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Properties.Settings.Default.includeDir != "")
            {
                string[] dirs = Properties.Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        AddInclude(dir);
                }
            }*/
#endif
            XmlNodeList steps = buildConfigs[Settings.Default.buildConfig].ChildNodes;
            progressBar.Visible = true;
            progressBar.Value = 0;
            int counter = 1;
            progressBar.Step = counter * 100 / (steps.Count + 1);
            foreach (XmlElement step in steps)
            {
                //int error = 0;
				//string fileName;
                //if (step.Attributes["action"].Value[0] != 'C')
                string originalDir = Path.GetDirectoryName(step.InnerText);
                string assembledName;
                string filePath;
                switch (step.Attributes["action"].Value[0])
                {
                    case 'C':
                        try
                        {
#if USE_DLL
                            filePath = step.InnerText;
                            //string ext = getExtension(step.Attributes["type"].Value[7] - 48);
                            fileName = Path.GetFileName(filePath);
                            assembledName = Path.Combine(originalDir, GlobalClass.project.getProjectName() + "." + getExtension(Convert.ToInt32(step.Attributes["type"].Value)));
                            //now we can set the args for spasm
                            error |= SetInputFileA(filePath);//Path.Combine(originalDir, fileName));
                            error |= SetOutputFileA(assembledName);//Path.Combine(originalDir, assembledName));
                            error |= RunAssembly();
                            GlobalClass.outputWindow.outputWindowBox.Text = "==================" + fileName +
                                                                 "==================\r\n" + "Assembling " + originalDir +
                                                                 "\\" + fileName + "\r\n";
                                // +wabbitspasm.StandardOutput.ReadToEnd();
#else
                            filePath = step.InnerText;
                            assembledName = Path.Combine(originalDir, ProjectService.ProjectName + "." + GetExtension(Convert.ToInt32(step.Attributes["type"].Value)));
                            error &= assembleCode(filePath, false, assembledName);
                            //sendFileEmu(assembledName);
#endif
                            sendToEmu = assembledName;
                        }
                        catch (Exception ex)
                        {
                            //errorConsoleBox.Text += ex.ToString() + '\n';
                            MessageBox.Show("Error: " + ex);
                        }
                        progressBar.PerformStep();
                        counter++;
                        break;
                    case 'T':
                        try
                        {
#if USE_DLL
                            fileName = Path.GetFileName(step.InnerText);
                            assembledName = Path.ChangeExtension(fileName, ".inc");
                            error |= SetInputFileA(Path.Combine(originalDir, fileName));
                            error |= SetOutputFileA(Path.Combine(originalDir, assembledName));
                            RunAssembly();
#else
                            filePath = step.InnerText;
                            assembledName = Path.Combine(originalDir, ProjectService.ProjectName + ".lab");
                            error &= createSymTable(filePath, assembledName);
                            /*wabbitspasm.StartInfo.Arguments = includeDir + "-L " + quote + originalDir + "\\" + fileName + quote;
                            wabbitspasm.Start();
                            
                            outputWindow.outputWindowBox.Text += "==================" + fileName +
                                                                 "==================\r\n" + "Creating Symbol Table for " +
                                                                 originalDir + "\\" + fileName + "\r\n" +wabbitspasm.StandardOutput.ReadToEnd();
                            outputWindow.outputWindowBox.SelectionStart = outputWindow.outputWindowBox.Text.Length;
                            outputWindow.outputWindowBox.ScrollToCaret();*/
#endif
                        }
                        catch (Exception ex)
                        {
                            //errorConsoleBox.Text += ex.ToString() + '\n';
                            MessageBox.Show("Error: " + ex);
                        }
                        progressBar.PerformStep();
                        counter++;
                        break;
                    case 'L':
                        filePath = step.InnerText;
                        assembledName = Path.Combine(originalDir, ProjectService.ProjectName + ".lst");
                        //error &= createListing(filePath, assembledName);
                        progressBar.PerformStep();
                        counter++;
                        break;
                    case 'E':
                        try
                        {
                            Process cmd = new Process
                                          {
                                              StartInfo =
                                                  {
                                                      FileName = Path.Combine(ProjectService.ProjectDirectory, step.InnerText),
                                                      WorkingDirectory = ProjectService.ProjectDirectory,
                                                      UseShellExecute = false,
                                                      CreateNoWindow = true,
                                                      RedirectStandardOutput = true,
                                                      RedirectStandardError = true
                                                  }
                                          };
                            cmd.Exited += new EventHandler(externalProgramExit);
                            cmd.Start();
                        }
                        catch (Exception ex)
                        {
                            //errorConsoleBox.Text += ex.ToString() + '\n';
                            MessageBox.Show("Error: " + ex);
                        }
                        progressBar.Step = counter * 100 / (steps.Count + 1);
                        counter++;
                        break;
                }
            }
            DockingService.OutputWindow.HighlightOutput();
            progressBar.Visible = false;
            return sendToEmu;
            //try
            //{
            //    getEmulator();
            //    if (Properties.Settings.Default.sendFileEmu && outputWindow.outputWindowBox.Text.Contains("error") == false && outputWindow.outputWindowBox.Text.Contains("Couldn't") == false)
            //    {
            //        emulator.StartInfo.FileName = Application.UserAppDataPath + "\\wabbitemu.exe";
            //        emulator.StartInfo.CreateNoWindow = true;
            //        emulator.Start();
            //    }
            //}
            //catch (Exception ex)
            //{
            //    //errorConsoleBox.Text += ex.ToString() + '\n';
            //    MessageBox.Show("Error: " + ex.ToString());
            //}
        }

        private void externalProgramExit(object sender, EventArgs e)
        {
            Process cmd = (Process)sender;
			DockingService.OutputWindow.AddText(cmd.StandardOutput.ReadToEnd());
        }

        private void SendFileEmu(string assembledName)
        {
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.wabbitemu.exe", "Wabbitemu.exe");
            emulator.StartInfo.Arguments = quote + assembledName + quote;
            emulator.StartInfo.FileName = "Wabbitemu.exe";
            emulator.Start();
			//switch to the emulator
            IntPtr calculatorHandle = NativeMethods.FindWindow("z80", "Wabbitemu");
            NativeMethods.SetForegroundWindow(calculatorHandle);
        }

        private bool createSymTable(string filePath, string assembledName)
        {
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.spasm.exe", "spasm.exe");
            //Clear any other assemblings
            //outputWindow.outputWindowBox.Text = "";
            //Get our emulator
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.Wabbitemu.exe", "Wabbitemu.exe");
#if USE_DLL == false
            //create two new processes to run
            wabbitspasm = new Process
            {
                StartInfo =
                {
                    FileName = Path.Combine(Application.UserAppDataPath, "spasm.exe"),
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            //setup wabbitspasm to run silently

            //some strings we'll need to build 
            originaldir = Path.GetDirectoryName(filePath);// filePath.Substring(0, filePath.LastIndexOf('\\'));
            string includedir = "-I \"" + Application.StartupPath + "\"";
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        includedir += ";\"" + dir + "\"";
                }
            }
            string fileName = Path.GetFileName(filePath);
            // filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            wabbitspasm.StartInfo.Arguments = includedir + " -L " + fileName + " " + quote + assembledName + quote;
            wabbitspasm.StartInfo.WorkingDirectory = originaldir;
            wabbitspasm.Start();

#else
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            ClearIncludes();
            ClearDefines();
            AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        AddInclude(dir);
                }
            }
            //get the file name we'll use and use it to create the assembled name
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            //now we can set the args for spasm
            int error = 0;
            error |= SetInputFileA(Path.Combine(originalDir, fileName));
            error |= SetOutputFileA(Path.Combine(originalDir, assembledName));
            //emulator setup
            //emulator.StartInfo.FileName = emuLoc;
            //assemble that fucker
            uint STD_ERROR_HANDLE = 0xfffffff4;
            uint STD_OUTPUT_HANDLE = 0xfffffff5;
            StreamWriter test = new StreamWriter(Application.StartupPath + "\\test.txt");
            //FileStream test = new FileStream(Application.StartupPath + "\\test.txt", FileMode.Create);
            //SetStdHandle(STD_ERROR_HANDLE, test.Handle);
            //SetStdHandle(STD_OUTPUT_HANDLE, test.Handle);
            Console.SetOut(test);
            Console.SetError(test);
            //StreamReader reader = myprocess.StandardOutput;
            RunAssembly();
            Console.WriteLine("test line");
            //string tryread = reader.ReadToEnd();
            //string output = myprocess.StandardOutput.ReadToEnd();
            test.Flush();
            test.Close();
#endif
			DockingService.ShowDockPanel(DockingService.OutputWindow);
            //lets write it to the output window so the user knows whats happening
			string outputText = "==================" + fileName + "==================\r\n" +
												"SymTable for " + originaldir + "\\" + fileName + "\r\n" +
												wabbitspasm.StandardOutput.ReadToEnd();
			DockingService.OutputWindow.SetText(outputText);
            //its more fun with colors
			DockingService.OutputWindow.HighlightOutput();
            
            bool errors = outputText.Contains("error");
            if (errors)
				DockingService.ShowDockPanel(DockingService.OutputWindow);
            //its more fun with colors
			DockingService.ErrorList.ParseOutput(outputText, originaldir);
            if (errors)
				DockingService.ShowDockPanel(DockingService.ErrorList); 
            //tell if the assembly was successful
            //if (error != 0)
            //    return false;
            //else
            //    return true;
            return !errors;
        }

        const string quote = "\"";
        /// <summary>
        /// Creates a listing file for the currently open file. Very useful for debugging.
        /// </summary>
        /// <param name="filePath">File to create list file of.</param>
        /// <param name="assembledName">Name of the file to create.</param>
        private bool createListing(string filePath, string assembledName)
        {
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.spasm.exe", "spasm.exe");
            //Clear any other assemblings
            //outputWindow.outputWindowBox.Text = "";
            //Get our emulator
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.Wabbitemu.exe", "Wabbitemu.exe");
#if USE_DLL == false
            //create two new processes to run
            wabbitspasm = new Process
            {
                StartInfo =
                {
                    FileName = Path.Combine(Application.UserAppDataPath, "spasm.exe"),
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
            //setup wabbitspasm to run silently

            //some strings we'll need to build 
            originaldir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            string includedir = "-I \"" + Application.StartupPath + "\"";
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        includedir += ";\"" + dir + "\"";
                }
            }
            string fileName = Path.GetFileName(filePath);
            // filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            wabbitspasm.StartInfo.Arguments = includedir + " -T " + fileName + " " + quote + assembledName + quote;
            wabbitspasm.StartInfo.WorkingDirectory = originaldir;
            wabbitspasm.Start();

#else
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            ClearIncludes();
            ClearDefines();
            AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        AddInclude(dir);
                }
            }
            //get the file name we'll use and use it to create the assembled name
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            //now we can set the args for spasm
            int error = 0;
            error |= SetInputFileA(Path.Combine(originalDir, fileName));
            error |= SetOutputFileA(Path.Combine(originalDir, assembledName));
            //emulator setup
            //emulator.StartInfo.FileName = emuLoc;
            //assemble that fucker
            uint STD_ERROR_HANDLE = 0xfffffff4;
            uint STD_OUTPUT_HANDLE = 0xfffffff5;
            StreamWriter test = new StreamWriter(Application.StartupPath + "\\test.txt");
            //FileStream test = new FileStream(Application.StartupPath + "\\test.txt", FileMode.Create);
            //SetStdHandle(STD_ERROR_HANDLE, test.Handle);
            //SetStdHandle(STD_OUTPUT_HANDLE, test.Handle);
            Console.SetOut(test);
            Console.SetError(test);
            //StreamReader reader = myprocess.StandardOutput;
            RunAssembly();
            Console.WriteLine("test line");
            //string tryread = reader.ReadToEnd();
            //string output = myprocess.StandardOutput.ReadToEnd();
            test.Flush();
            test.Close();
#endif
			DockingService.ShowDockPanel(DockingService.OutputWindow); 
            //lets write it to the output window so the user knows whats happening
            string outputText = "==================" + fileName + "==================\r\n" +
                                                "Listing " + originaldir + "\\" + fileName + "\r\n" +
                                                wabbitspasm.StandardOutput.ReadToEnd();
			DockingService.OutputWindow.SetText(outputText);
            //its more fun with colors
			DockingService.OutputWindow.HighlightOutput();
            bool errors = outputText.Contains("error");
            if (errors)
				DockingService.ShowDockPanel(DockingService.OutputWindow); 
            //its more fun with colors
            DockingService.ErrorList.ParseOutput(outputText, originaldir);
            if (errors)
				DockingService.ShowDockPanel(DockingService.ErrorList); 
            //tell if the assembly was successful
            //if (error != 0)
            //    return false;
            //else
            //    return true;
            return !errors;
        }

        private void createStatsFile(String filePath)
        {
            GlobalClass.GetResource("Revsoft.Wabbitcode.Resources.spasm.exe", "spasm.exe");
            wabbitspasm = new Process
                              {
                                  StartInfo =
                                      {
                                          FileName = Path.Combine(Application.UserAppDataPath, "spasm.exe"),
                                          RedirectStandardOutput = true,
                                          RedirectStandardError = true,
                                          UseShellExecute = false,
                                          CreateNoWindow = true
                                      }
                              };
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1,
                                                 filePath.Length - filePath.LastIndexOf('\\') - 1);
            string includePath = "-I " + quote + originalDir + quote;
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                    includePath += " -I " + quote + dir + quote;
            }
            wabbitspasm.StartInfo.Arguments = includePath + " -S " + quote + originalDir + "\\" + fileName + quote + " " +
                                              quote + originalDir + "\\" + fileName + ".txt" + quote;
            wabbitspasm.Start();
            string outputText = "==================" + fileName + "==================\r\n" +
                                                "Creating Stats File for " + originalDir + "\\" + fileName + "\r\n" +
                                                wabbitspasm.StandardOutput.ReadToEnd();
			DockingService.OutputWindow.SetText(outputText);
			DockingService.OutputWindow.HighlightOutput();
        }

        protected internal string GetExtension(int outputFile)
        {
            string outputFileExt = "bin";
            switch (outputFile)
            {
                case 1:
                    outputFileExt = "73p";
                    break;
                case 2:
                    outputFileExt = "82p";
                    break;
                case 3:
                    outputFileExt = "83p";
                    break;
                case 4:
                    outputFileExt = "8xp";
                    break;
                case 5:
                    outputFileExt = "8xk";
                    break;
                case 6:
                    outputFileExt = "85p";
                    break;
                case 7:
                    outputFileExt = "85s";
                    break;
                case 8:
                    outputFileExt = "86p";
                    break;
                case 9:
                    outputFileExt = "86s";
                    break;
            }
            return outputFileExt;
        }

        #endregion

        #region Debugging

        private TextMarker highlight;
        bool showToolbar = true;

        private void startDebugMenuItem_Click(object sender, EventArgs e)
        {
			if (DebuggerService.IsDebugging)
				DebuggerService.Run();
			else
				DebuggerService.StartDebug();
				#region OldDebug
				//Kept for sentimental reasons
				//((Wabbitcode.newEditor)(ActiveMdiChild)).editorBox.ActiveTextAreaControl.Enabled = false;

				//old stuff?
				//string locInfo = debugTable[page + ":" + startAddress].ToString();
				//string file = locInfo.Substring(0, locInfo.LastIndexOf(':'));
				//string line = locInfo.Substring(locInfo.LastIndexOf(':') + 1, locInfo.Length - locInfo.LastIndexOf(':') - 1);
				/*
				if (Path.GetExtension(createdName) == ".8xk")
				{
					isAnApp = true;
					Wabbitemu.AppEntry[] appList = debugger.getAppList();
					apppage = appList[0].page;
					debugger.setBreakpoint(false, apppage, 0x4080);
				}
				else
				{
					debugger.sendKeyPress((int)Keys.F12);
					debugger.releaseKeyPress((int)Keys.F12);
					System.Threading.Thread.Sleep(2000);
					debugger.setBreakpoint(true, 1, 0x9D95);
				}*/
				/*try
				{
					while (debugging)
					{
						Application.DoEvents();
					}
                
						//calcScreen.Image = debugger.DrawScreen();
						var currentLoc = new Wabbitemu.breakpoint();
						currentLoc.Address = debugger.getState().PC;
						currentLoc.Page = byte.Parse(getPageNum(currentLoc.Address.ToString("X")), NumberStyles.HexNumber);
						currentLoc.IsRam = getRamState(currentLoc.Address.ToString("X"));
						bool breakpointed = false;
						foreach (Wabbitemu.breakpoint breakpoint in breakpoints)
						{
							if (breakpoint.Page == currentLoc.Page && breakpoint.Address == currentLoc.Address)
								breakpointed = true;
						}
						while (breakpointed)
						{
							//updateRegisters();
							//updateFlags();
							//updateCPUStatus();
							//updateInterrupts();
							if (stepOverClicked)
								step(debugTable);
							Application.DoEvents();
						}
						debugger.step();
						//System.Threading.Thread.Sleep(2000);
						Application.DoEvents();
					}
				}
				catch (COMException ex)
				{
					if (ex.ErrorCode != -2147023174)
						MessageBox.Show(ex.ToString());
				}*/
				#endregion
        }

        /*private void staticLabelsParser_DoWork(object sender, DoWorkEventArgs e)
        {
			if (GlobalClass.mainForm.IsDisposed || GlobalClass.mainForm.Disposing)
				return;
            if (e.Argument == null)
            {
                foreach (newEditor child in GlobalClass.mainForm.MdiChildren)
                {
                    currentEditorBox = child.editorBox;
                    getEditorBoxText d = getTextForStaticLabels;
                    string[] text = (string[])Invoke(d, new[] { currentEditorBox });
                    addStaticLabels(text);
                }
            }
            else
            {
                currentEditorBox = (TextEditorControl) e.Argument;
                getEditorBoxText d = getTextForStaticLabels;
                string[] text = (string[])Invoke(d, new[] { currentEditorBox });
                addStaticLabels(text);
            }
        }

        private delegate string[] getEditorBoxText(TextEditorControl editorBox);
        private string[] getTextForStaticLabels(TextEditorControl editorBox)
        {
            return currentEditorBox.Text.ToUpper().Split('\n');
        }

        private TextEditorControl currentEditorBox;
        private void addStaticLabel(int lineNum, int location, string label, string line, DictionaryEntry keyword)
        {
            TextMarker newMarker = new TextMarker(currentEditorBox.Document.GetOffsetForLineNumber(lineNum) + line.IndexOf(label, location), label.Length, TextMarkerType.Invisible) 
                        { ToolTip = keyword.Value.ToString() };
            staticLabelMarkers.Add(newMarker);
            currentEditorBox.Document.MarkerStrategy.AddMarker(newMarker);
        }

        private delegate void AddStaticLabel(
            int lineNum, int location, string label, string line, DictionaryEntry keyword);
        private void addStaticLabels(string[] editorText)
        {
            string line;
            foreach (DictionaryEntry keyword in symTable.staticLabels)
            {
                int lineNum = 0;
                string label = keyword.Key.ToString();
                while (lineNum < editorText.Length)
                {
                    int location = 0;
                    int commentChar = editorText[lineNum].IndexOf(';');
                    line = editorText[lineNum].Contains(";")
                               ? editorText[lineNum].Remove(commentChar, editorText[lineNum].Length - commentChar)
                               : editorText[lineNum];
                    if (line.Length == 0 || !char.IsWhiteSpace(line[0]) || line.Contains("\""))
                    {
                        lineNum++;
                        continue;
                    }

                    while (location < line.Length && Char.IsWhiteSpace(line[location]))
                        location++;
                    while (location < line.Length && Char.IsLetter(line[location]))
                        location++;
                    if (line.IndexOf(label, location) != -1)
                    {
                        int index = line.IndexOf(label, location);
                        if (" \t\n\r,();+-".IndexOf(line[index - 1]) != -1 && index + label.Length < line.Length && " \t\n\r,();+-".IndexOf(line[index + label.Length]) != -1)
                        {
							try
							{
								AddStaticLabel d = addStaticLabel;
								Invoke(d, new object[] { lineNum, location, label, line, keyword });
							}
							catch (Exception)
							{ }
                        }
                    }
                    lineNum++;
                }
            }
        }*/

		private const int WM_KEYDOWN = 0x0100;
        private const int VK_SHIFT = 16;
        private const int VK_LSHIFT = 0xA0;
        private const int VK_RSHIFT = 0xA1;
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            switch (msg.Msg)
            {
                case WM_KEYDOWN:
                    if ((int)msg.WParam == VK_SHIFT)
                    {
                        if ((NativeMethods.GetKeyState(VK_LSHIFT) & 0xFF00) != 0)
                            msg.WParam = (IntPtr)VK_LSHIFT;
                        else
                            msg.WParam = (IntPtr)VK_RSHIFT;
                    }
                    break;
            }
            return base.ProcessCmdKey(ref msg, keyData);
        }

        public void UpdateDebugStuff()
        {
			DockingService.DebugPanel.Enabled = DebuggerService.IsBreakpointed;
			DockingService.CallStack.Enabled = DebuggerService.IsBreakpointed;
			DockingService.TrackWindow.Enabled = DebuggerService.IsBreakpointed;
            runMenuItem.Enabled = true;
            runDebuggerToolButton.Enabled = true;
			stepMenuItem.Enabled = DebuggerService.IsBreakpointed && DebuggerService.IsDebugging;
			stepToolButton.Enabled = DebuggerService.IsBreakpointed && DebuggerService.IsDebugging;
			stepOverMenuItem.Enabled = DebuggerService.IsBreakpointed && DebuggerService.IsDebugging;
			stepOverToolButton.Enabled = DebuggerService.IsBreakpointed && DebuggerService.IsDebugging;
			stepOutMenuItem.Enabled = DebuggerService.IsBreakpointed && DebuggerService.IsDebugging;
			stepOutToolButton.Enabled = DebuggerService.IsBreakpointed && DebuggerService.IsDebugging;
			stopDebugMenuItem.Enabled = DebuggerService.IsDebugging;
			stopToolButton.Enabled = DebuggerService.IsDebugging;
			pauseToolButton.Enabled = !DebuggerService.IsBreakpointed && DebuggerService.IsDebugging;
        }

        private const int WM_USER = 0x0400;
        protected override void WndProc(ref Message m)
        {
            // Listen for operating system messages.
            switch (m.Msg)
            {
                case WM_USER:
					DebuggerService.BreakpointHit();
                    break;
            }
            base.WndProc(ref m);
        }

        private void stepButton_Click(object sender, EventArgs e)
        {
			DebuggerService.Step();
        }

		private void stepOverMenuItem_Click(object sender, EventArgs e)
		{
			DebuggerService.StepOver();
		}

		private void stepOutMenuItem_Click(object sender, EventArgs e)
		{
			DebuggerService.StepOut();
		}

		private void stepOutToolButton_Click(object sender, EventArgs e)
		{
			DebuggerService.StepOut();
		}

		/*private bool editAndContinue()
		{
			DialogResult continueResult = MessageBox.Show("Changes have been made, would you like to try and continue debugging?", "Edit and Continue", MessageBoxButtons.YesNo, MessageBoxIcon.Information);
			if (continueResult == DialogResult.No)
				return false;
			string createdName = "", fileName = "", symName, listName;
			if (GlobalClass.project.projectOpen)
			{
				listName = Path.Combine(ProjectService.ProjectDirectory, ProjectService.ProjectName + ".lst");
				symName = Path.Combine(ProjectService.ProjectDirectory, ProjectService.ProjectName + ".lab");
				fileName = Path.Combine(ProjectService.ProjectDirectory, ProjectService.ProjectName + ".asm");
				int outputType = GlobalClass.project.getOutputType();
				XmlDocument doc = new XmlDocument();
				doc.Load(ProjectService.ProjectFile);
				XmlNodeList buildConfigs = doc.ChildNodes[1].ChildNodes[1].ChildNodes;
				int counter = 0;
				foreach (XmlNode config in buildConfigs)
				{
					if (config.Name.ToLower() == "debug")
					{
						Settings.Default.buildConfig = counter;
						counter = -1;
						break;
					}
					counter++;
				}
				if (counter == -1)
					createdName = assembleProject();
			}
			else
			{
				if (ActiveMdiChild != null)
					fileName = GlobalClass.activeChild.editorBox.FileName;
				error &= assembleCode(fileName, false,
									  Path.ChangeExtension(fileName, getExtension(Settings.Default.outputFile)));
				listName = Path.ChangeExtension(fileName, ".lst");
				symName = Path.ChangeExtension(fileName, ".lab");
				switch (Settings.Default.outputFile)
				{
					case 4:
						createdName = Path.ChangeExtension(fileName, "8xp");
						break;
					case 5:
						createdName = Path.ChangeExtension(fileName, "8xk");
						break;
				}
			}
			if (!error && MessageBox.Show("Errors assembling, would you like to continue?", "Edit and Continue", MessageBoxButtons.YesNo, MessageBoxIcon.Information) != DialogResult.Yes)
				return false;
			debugger = new CWabbitemu(createdName);
			debugTable = new Hashtable();
			debugTableReverse = new Hashtable();
			TextEditorControl editorBox;
			StreamReader reader = new StreamReader(listName);
			//StreamReader breakReader = new StreamReader(fileName.Remove(fileName.Length - 3) + "brk");
			string listFileText = reader.ReadToEnd();
			reader = new StreamReader(symName);
			string symFileText = reader.ReadToEnd();
			parseListFile(listFileText, fileName, Path.GetDirectoryName(fileName), ref debugTable,
						  ref debugTableReverse);
			symTable = new SymbolTableClass();
			symTable.parseSymFile(symFileText);
			foreach (newEditor child in MdiChildren)
			{
				editorBox = child.editorBox;
				string breakLocInfo;
				ReadOnlyCollection<Breakpoint> marks = editorBox.Document.BreakpointManager.Marks;
				foreach (Breakpoint breakpoint in marks)
				{
					int breakNum = GlobalClass.FindBreakpoint(editorBox.FileName, breakpoint.LineNumber);
					if (debugTable.ContainsValue(editorBox.FileName.ToLower() + ":" + (breakpoint.LineNumber + 1)) && breakNum != -1)
					{
						GlobalClass.WabbitcodeBreakpoint newBreakpoint = GlobalClass.breakpoints[breakNum];
						breakLocInfo =
							debugTableReverse[editorBox.FileName.ToLower() + ":" + (breakpoint.LineNumber + 1)].ToString();
						newBreakpoint.Address = UInt16.Parse(breakLocInfo.Substring(3, 4), NumberStyles.HexNumber);
						if (isAnApp)
							newBreakpoint.Page = (byte)(apppage - byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber));
						else
							newBreakpoint.Page = byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber);
						newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
						newBreakpoint.file = editorBox.FileName;
						newBreakpoint.lineNumber = breakpoint.LineNumber;
						GlobalClass.breakpoints[breakNum] = newBreakpoint;
#if NEW_DEBUGGING
                            SetBreakpoint(0, Handle, newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#else
						debugger.setBreakpoint(Handle, newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#endif
					}
					else
					{
						editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
						break;
					}
				}
				child.setNextStateMenuItem.Visible = true;
			}
			if (!staticLabelsParser.IsBusy && !Disposing && !IsDisposed)
				staticLabelsParser.RunWorkerAsync();
			return true;
		}*/

        private ushort oldSP = 0xFFFF;
        public void UpdateStack()
        {
#if NEW_DEBUGGING
            int currentSP = GetState(currentSlot).SP;
#else
			int currentSP = DebuggerService.Debugger.getState().SP;
#endif
            if (currentSP < 0xFE66)
                return;
            while (oldSP != currentSP - 2)
            {
                if (oldSP > currentSP - 2)
                {
#if NEW_DEBUGGING
                    callStack.addStackData(oldSP, ReadMem(currentSlot, oldSP) + ReadMem(currentSlot, (ushort)(oldSP + 1)) * 256);
#else
					DockingService.CallStack.addStackData(oldSP, DebuggerService.Debugger.readMem(oldSP) + DebuggerService.Debugger.readMem((ushort)(oldSP + 1)) * 256);
#endif
                    oldSP -= 2;
                }
                else
                {
					DockingService.CallStack.removeLastRow();
                    oldSP += 2;
                }
            }
        }

        

        public void cancelDebug_Click(object sender, EventArgs e)
        {
			DebuggerService.CancelDebug();
        }

        private static bool InvisibleMarkers(TextMarker marker)
        {
            return marker.TextMarkerType == TextMarkerType.Invisible;
        }

        #endregion

        #region Main ToolBar

        private void newToolButton_Click(object sender, EventArgs e)
        {
            newEditor doc = DocumentService.CreateNewDocument();
            doc.TabText = "New Document";
            doc.Show(dockPanel);
        }

        private void openToolButton_Click(object sender, EventArgs e)
        {
			DocumentService.OpenDocument();
        }

        private void saveToolButton_Click(object sender, EventArgs e)
        {
			DocumentService.SaveDocument();
        }

        private void cutToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.editorBox.Cut();
        }

        private void copyToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.editorBox.Copy();
        }

        private void pasteToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.editorBox.Paste();
        }

        #endregion

        #region ProjectStuff

		

        #endregion


        private void runMenuItem_Click(object sender, EventArgs e)
        {
			DebuggerService.Run();
        }

        private void pauseToolButton_Click(object sender, EventArgs e)
        {
			DebuggerService.Pause();
        }

        private void documentParser_DoWork(object sender, DoWorkEventArgs e)
        {
            ArrayList arguments = (ArrayList)e.Argument;
            TextEditorControl editorBox = (TextEditorControl)arguments[0];
            string text = arguments[1].ToString();
            foreach (TextMarker marker in editorBox.Document.MarkerStrategy.TextMarker)
            {
                if (marker.Tag == "Code Check")
                    editorBox.Document.MarkerStrategy.RemoveMarker(marker);
            }
            string filePath = editorBox.FileName;
            //setup wabbitspasm to run silently
            wabbitspasm = new Process
                              {
                                  StartInfo =
                                      {
                                          FileName = "spasm.exe",
                                          RedirectStandardOutput = true,
                                          RedirectStandardError = true,
                                          UseShellExecute = false,
                                          CreateNoWindow = true
                                      }
                              };

            //some strings we'll need to build 
            originaldir = Path.GetDirectoryName(filePath); //filePath.Substring(0, filePath.LastIndexOf('\\'));
            string includedir = "-I \"" + Application.StartupPath + "\"";
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        includedir += ";\"" + dir + "\"";
                }
            }
            //string fileName = Path.GetFileName(filePath);
            // filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            wabbitspasm.StartInfo.Arguments = "-V " + includedir + " " + text;
            wabbitspasm.StartInfo.WorkingDirectory = originaldir == "" ? Application.StartupPath : originaldir;
            wabbitspasm.Start();
            string output = wabbitspasm.StandardOutput.ReadToEnd();
            errorsToAdd.Clear();
            foreach (string line in output.Split('\n'))
            {
                if (!line.Contains("error")) 
                    continue;
                int firstColon = line.IndexOf(':');
                int secondColon = line.IndexOf(':', firstColon + 1);
                int thirdColon = line.IndexOf(':', secondColon + 1);
                int lineNum = Convert.ToInt32(line.Substring(firstColon + 1, secondColon - firstColon - 1));
                string description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                ArrayList listofattributes = new ArrayList {lineNum, description};
                errorsToAdd.Add(listofattributes);
            }
        }

        private readonly ArrayList errorsToAdd = new ArrayList();
        private void addSquiggleLine(int newLineNumber, Color underlineColor, string description)
        {
            //this code highlights the current line
            //I KNOW IT WORKS DONT FUCK WITH IT
            if (ActiveMdiChild == null) 
                return;
            TextEditorControl editorBox = ((newEditor)(ActiveMdiChild)).editorBox;
            TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
            editorBox.LineViewerStyle = LineViewerStyle.None;
            editorBox.ActiveTextAreaControl.ScrollTo(newLineNumber - 1);
            editorBox.ActiveTextAreaControl.Caret.Line = newLineNumber - 1;
            int start = textArea.Caret.Offset;
            int length =
                editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
            while (start > 0 && textArea.Document.TextContent[start] != '\n')
                start--;
            start++;
            length--;
            while (textArea.Document.TextContent[start] == ' ' || textArea.Document.TextContent[start] == '\t')
            {
                start++;
                length--;
            }
            highlight = new TextMarker(start, length, TextMarkerType.WaveLine, underlineColor)
                            {
                                ToolTip = description,
                                Tag = "Code Check"
                            };
            editorBox.Document.MarkerStrategy.AddMarker(highlight);
        }

        private void documentParser_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            foreach (ArrayList attributes in errorsToAdd)
                addSquiggleLine((int)attributes[0], Color.Red, attributes[1].ToString());
            if (ActiveMdiChild != null) 
                ((newEditor)ActiveMdiChild).editorBox.Refresh();
        }

        private void refreshViewMenuItem_Click(object sender, EventArgs e)
        {
            DockingService.ProjectViewer.buildProjTree(ProjectService.ProjectFile);
        }

        private void saveAllToolButton_Click(object sender, EventArgs e)
        {
            foreach(newEditor child in MdiChildren)
                DocumentService.SaveDocument(child);
        }

        private Thread updateThread;
        private void updateMenuItem_Click(object sender, EventArgs e)
        {
            if (updateThread != null && updateThread.IsAlive)
                return;
            updateThread = new Thread(updateApp);
            updateThread.Start();
        }

        private Version curVersion =  Assembly.GetExecutingAssembly().GetName().Version;
        private delegate bool OurDelegate(bool newVersion);
        private void updateApp()
        {
            bool newVersion = CheckForNewVersion();
            bool downloadIt = (bool) Invoke(new OurDelegate(OnAskUser), new Object[] {newVersion});
            if (!downloadIt)
                return;
            Application.Exit();
            Process updater = new Process
                                  {
                                      StartInfo = {FileName = "Revsoft.Autoupdater.exe"}
                                  };
            updater.Start();
        }

        private bool CheckForNewVersion()
        {
            try
            {
                WebClient Client = new WebClient();
                Stream strm = Client.OpenRead("http://group.revsoft.org/Wabbitcode/WabbitcodeVersion.txt");
                StreamReader sr = new StreamReader(strm);
                Version newVer = new Version(sr.ReadLine());
                return curVersion.CompareTo(newVer) < 0;
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            return false;
        }

        private static bool OnAskUser(bool newVersion)   
        {   
            if (!newVersion)   
            {   
                MessageBox.Show("No updates available", "Check for updates");   
                return false;   
            }   
            return DialogResult.Yes ==  MessageBox.Show("Download new version?", "Check for updates", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
        }

        private void aboutMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox box = new AboutBox();
            box.ShowDialog();
        }

        private void MainFormRedone_KeyPress(object sender, KeyPressEventArgs e)
        {

		}

        private void newBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            NewBreakpointForm form = new NewBreakpointForm();
            form.ShowDialog();
        }

		private void toggleBreakpointMenuItem_Click(object sender, EventArgs e)
		{
			if (DockingService.ActiveDocument == null)
				return;
			DockingService.ActiveDocument.editorBox.Document.BreakpointManager.ToggleMarkAt(DockingService.ActiveDocument.editorBox.ActiveTextAreaControl.Caret.Position);
			DockingService.ActiveDocument.Refresh();

		}

		private void formatDocMenuItem_Click(object sender, EventArgs e)
		{
			if (DockingService.ActiveDocument == null)
				return;
			string[] lines = DockingService.ActiveDocument.editorBox.Text.Split('\n');
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
				bool islabel = line != "" && (!char.IsWhiteSpace(line[0]) || line[0] == '_');
				line = line.Trim();
				if (line.StartsWith("push"))
					currentIndent += indent;
				if ((line.StartsWith("pop") || line.StartsWith("ret")) && currentIndent.Length > 1)
					currentIndent = currentIndent.Remove(currentIndent.Length - 1);
				if (!islabel)
					line = currentIndent + line;
				lines[i] = line + comment;
			}
			string newText = "";
			foreach (string line in lines)
				newText += line + '\n';
			DockingService.ActiveDocument.editorBox.Text = newText;
		}

		internal void StartDebug()
		{
			showToolbar = Settings.Default.debugToolbar;
			Settings.Default.debugToolbar = true;
			UpdateDebugStuff();
			if (!showToolbar)
				toolBarManager.AddControl(debugToolStrip, DockStyle.Top, mainToolBar, DockStyle.Right);
			debugToolStrip.Height = mainToolBar.Height;
			UpdateChecks();
			DockingService.ShowDockPanel(DockingService.DebugPanel);
			DockingService.ShowDockPanel(DockingService.TrackWindow);
			DockingService.ShowDockPanel(DockingService.CallStack);
			UpdateTitle();
		}

		internal void UpdateBreakpoints()
		{
			TextEditorControl editorBox;
			foreach (newEditor child in MdiChildren)
			{
				editorBox = child.editorBox;
				Breakpoint[] marks = new Breakpoint[editorBox.Document.BreakpointManager.Marks.Count];
				editorBox.Document.BreakpointManager.Marks.CopyTo(marks, 0);
				foreach (Breakpoint breakpoint in marks)
				{
					WabbitcodeBreakpoint newBreakpoint = DebuggerService.FindBreakpoint(editorBox.FileName, breakpoint.LineNumber);
					//ListFileKey key = new ListFileKey(editorBox.FileName.ToLower(), breakpoint.LineNumber + 1);
					ListFileValue value = DebuggerService.GetListValue(editorBox.FileName.ToLower(), breakpoint.LineNumber + 1);
					if (value != null && newBreakpoint != null)
					{
						newBreakpoint.Address = value.Address;
						if (DebuggerService.IsAnApp)
							newBreakpoint.Page = (byte)(DebuggerService.AppPage - value.Page);
						else
							newBreakpoint.Page = value.Page;
						newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
						newBreakpoint.file = editorBox.FileName;
						newBreakpoint.lineNumber = breakpoint.LineNumber;
#if NEW_DEBUGGING
                        SetBreakpoint(0, Handle, newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#else
						DebuggerService.Debugger.setBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#endif
					}
					else
					{
						editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
					}
				}
				child.setNextStateMenuItem.Visible = true;
			}
		}

		internal void DoneStep(ListFileKey newKey)
		{
			UpdateStepOut();
			DocumentService.RemoveDebugHighlight();
			DockingService.MainForm.UpdateStack();
			DocumentService.GotoLine(newKey.FileName, newKey.LineNumber);
			DocumentService.HighlightDebugLine(newKey.LineNumber);
			DockingService.MainForm.UpdateTrackPanel();
			DockingService.MainForm.UpdateDebugPanel();
		}


		private void UpdateStepOut()
		{
			if (DebuggerService.StepStack.Count > 0)
			{
				stepOutMenuItem.Enabled = true;
				stepOutToolButton.Enabled = true;
			}
			else
			{
				stepOutMenuItem.Enabled = false;
				stepOutToolButton.Enabled = false;
			}
		}

		internal void CancelDebug()
		{
			UpdateTitle();
			UpdateDebugStuff();
			if (DockingService.DebugPanel != null)
				DockingService.HideDockPanel(DockingService.DebugPanel);
			if (DockingService.TrackWindow != null)
				DockingService.HideDockPanel(DockingService.TrackWindow);
			if (DockingService.CallStack != null)
				DockingService.HideDockPanel(DockingService.CallStack);
			Settings.Default.debugToolbar = showToolbar;
			if (!showToolbar)
				toolBarManager.RemoveControl(debugToolStrip);
			UpdateChecks();
			DocumentService.RemoveDebugHighlight();
			foreach (newEditor child in MdiChildren)
			{
				TextEditorControl editorBox = child.editorBox;
				editorBox.Document.MarkerStrategy.RemoveAll(InvisibleMarkers);
				child.setNextStateMenuItem.Visible = false;
			}
		}

		internal void UpdateDebugPanel()
		{
			DockingService.DebugPanel.UpdateFlags();
			DockingService.DebugPanel.UpdateRegisters();
			DockingService.DebugPanel.updateScreen();
		}

		internal void UpdateTrackPanel()
		{
			DockingService.TrackWindow.updateVars();
		}

		internal void HideProgressBar()
		{
			progressBar.Visible = false;
		}

		internal void SetToolStripText(string text)
		{
			toolStripStatusLabel.Text = text;
		}

		internal void SetLineAndColStatus(string line, string col)
		{
			lineStatusLabel.Text = "Ln: " + line;
			colStatusLabel.Text = "Col: " + col;
		}

		internal void UpdateProjectMenu(bool projectOpen)
		{
			projMenuItem.Visible = projectOpen;
			includeDirButton.Visible = !projectOpen;
		}
	}
}