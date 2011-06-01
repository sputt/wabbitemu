using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

#if NEW_DEBUGGING
using Revsoft.Wabbitcode.Services.Debugger;
#endif

namespace Revsoft.Wabbitcode
{
    public partial class MainFormRedone : Form
    {        
        //TODO: remove this from here
        private readonly ToolbarManager.ToolBarManager toolBarManager;

        public MainFormRedone(string[] args)
        {
            InitializeComponent();
			RestoreWindow();

			toolBarManager = new ToolbarManager.ToolBarManager(this, this);
			if (Settings.Default.mainToolBar)
				toolBarManager.AddControl(mainToolBar);
			if (Settings.Default.debugToolbar)
				toolBarManager.AddControl(debugToolStrip);
            AssemblerService.InitAssembler();

			DockingService.InitDocking(dockPanel);
			DockingService.InitPanels();
            if (args.Length == 0)
                LoadStartupProject();
			DockingService.LoadConfig();
            if (!ProjectService.IsInternal)
                DockingService.ProjectViewer.BuildProjTree();
			HandleArgs(args);
			UpdateMenus(DockingService.ActiveDocument != null);
			UpdateChecks();

            DocumentService.GetRecentFiles();
        }

        public delegate void ProcessParametersDelegate(object sender, string[] args);
        public void ProcessParameters(object sender, string[] args)
        {
            // The form has loaded, and initialization will have been be done.
            HandleArgs(args);
            NativeMethods.SetForegroundWindow(this.Handle);
        }

		private void RestoreWindow()
		{
            try
            {
                this.WindowState = Settings.Default.WindowState;
                this.Size = Settings.Default.WindowSize;
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error restoring the window size", ex);
            }
		}

        private void LoadStartupProject()
        {
            if (string.IsNullOrEmpty(Settings.Default.startupProject))
                return;
            try
            {
				bool valid = false;
                if (File.Exists(Settings.Default.startupProject))
                    valid = ProjectService.OpenProject(Settings.Default.startupProject);
                else
                {
                    Settings.Default.startupProject = "";
                    DockingService.ShowError("Error: Project file not found");
                }
                if (ProjectService.IsInternal || !valid)
                    ProjectService.CreateInternalProject();
            }
            catch (Exception ex)
            {
				ProjectService.CreateInternalProject();
				var result = MessageBox.Show("There was an error loading the startup project, would you like to remove it?\n" + ex.ToString(),
									"Error", MessageBoxButtons.YesNo, MessageBoxIcon.Error);
				if (result == DialogResult.Yes)
					Settings.Default.startupProject = "";
            }
        }

		private void HandleArgs(string[] args)
		{
            if (args.Length == 0)
                return;
			foreach (string file in args)
				if (File.Exists(file))
					if (Path.GetExtension(file) == ".wcodeproj")
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
                                DockingService.ShowError("Error: File not found");
							}
							catch (Exception ex)
							{
                                DockingService.ShowError("Error in loading startup args", ex);
							}
		}
        
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
			gLineMenuItem.Enabled = enabled;
			gLabelMenuItem.Enabled = enabled;
			
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
			{
				startDebugMenuItem.Enabled = enabled;
				startWithoutDebugMenuItem.Enabled = enabled;
				runMenuItem.Enabled = enabled;
				runDebuggerToolButton.Enabled = enabled;
			}
			else
			{
				runMenuItem.Enabled = true;
				runDebuggerToolButton.Enabled = true;
				startDebugMenuItem.Enabled = true;
				startWithoutDebugMenuItem.Enabled = true;
			}

            //Window Menu
            windowMenuItem.Enabled = enabled;
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
			if (DebuggerService.IsDebugging)
				DebuggerService.CancelDebug();
			if (!ProjectService.IsInternal)
				CloseProject();
			SaveWindow();
			DockingService.Destroy();
            Settings.Default.Save();
        }

		private void SaveWindow()
		{
			if (this.WindowState != FormWindowState.Normal)
				Settings.Default.WindowSize = new Size(this.RestoreBounds.Width, this.RestoreBounds.Height);
			else
				Settings.Default.WindowSize = this.Size;
			Settings.Default.WindowState = this.WindowState;
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
                DocumentService.OpenDocument(file);
        }

        private void findBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != (char) Keys.Enter) 
                return;
            if (ActiveMdiChild == null)
                return;
            if (!findBox.Items.Contains(findBox.Text))
                findBox.Items.Add(findBox.Text);
            DocumentService.ActiveDocument.Find(findBox.Text);
        }

        private void addNewFileMenuItem_Click(object sender, EventArgs e)
        {
            RenameForm newNameForm = new RenameForm();
            newNameForm.Text = "New File";
            var result = newNameForm.ShowDialog() != DialogResult.OK;
			newNameForm.Dispose();
			if (result)
                return;
            string name = newNameForm.NewText;
            DockingService.ProjectViewer.AddNewFile(name);
        }

        private void existingFileMenuItem_Click(object sender, EventArgs e)
        {
			OpenFileDialog openFileDialog = new OpenFileDialog()
			{
				CheckFileExists = true,
				DefaultExt = "*.asm",
				Filter = "All Know File Types | *.asm; *.z80; *.inc; |Assembly Files (*.asm)|*.asm|*.z80" +
						   " Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|All Files(*.*)|*.*",
				FilterIndex = 0,
				Multiselect = true,
				RestoreDirectory = true,
				Title = "Add Existing File",
			};
			DialogResult result = openFileDialog.ShowDialog();
			if (result != DialogResult.OK)
				return;
            foreach (string file in openFileDialog.FileNames)
                DockingService.ProjectViewer.AddExistingFile(file);
        }

        private void includeDirButton_Click(object sender, EventArgs e)
        {
            IncludeDir includes = new IncludeDir();
            includes.ShowDialog();
			includes.Dispose();
        }

        #region FileMenu

        private void newFileMenuItem_Click(object sender, EventArgs e)
        {
            newEditor doc = DocumentService.CreateNewDocument();
            DockingService.ShowDockPanel(doc);
        }

        private void newProjectMenuItem_Click(object sender, EventArgs e)
        {
            NewProjectDialog template = new NewProjectDialog();
            if (template.ShowDialog() != DialogResult.OK) 
                return;
            UpdateProjectMenu(true);
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
				if (!ProjectService.OpenProject(openFileDialog.FileName))
					ProjectService.CreateInternalProject();
			UpdateMenus(DockingService.Documents.Count() > 0);
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

		internal void SetProgress(int percent)
		{
			progressBar.Visible = true;
			progressBar.Value = percent;
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
            MenuItem button = (MenuItem) sender;
            DocumentService.OpenDocument(button.Text);
        }

        internal void ClearRecentItems()
        {
            recentFilesMenuItem.MenuItems.Clear();
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
                DockingService.ActiveDocument.Undo();
        }

        private void redoMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.Redo();
        }

        private void cutMenuItem_Click(object sender, EventArgs e)
        {
			Cut();
        }

        private void copyMenuItem_Click(object sender, EventArgs e)
        {
			Copy();
        }

		private void pasteMenuItem_Click(object sender, EventArgs e)
		{
			Paste();
		}

		private void Cut()
		{
			if (DockingService.ActiveContent.GetType() == typeof(ToolWindow))
				((ToolWindow)DockingService.ActiveContent).Cut();
			else if (DockingService.ActiveDocument != null)
				DockingService.ActiveDocument.Cut();
		}

		private void Copy()
		{
			if (DockingService.ActiveContent.GetType() == typeof(ToolWindow))
				((ToolWindow)DockingService.ActiveContent).Copy();
			else if (DockingService.ActiveDocument != null)
				DockingService.ActiveDocument.Copy();
		}

		private void Paste()
		{
			if (DockingService.ActiveContent.GetType().BaseType == typeof(ToolWindow))
				((ToolWindow)DockingService.ActiveContent).Paste();
			else if (DockingService.ActiveDocument != null)
				DockingService.ActiveDocument.Paste();
		}

        private void selectAllMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DocumentService.ActiveDocument.SelectAll();
        }

        private void findMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, false, false);
        }

        private void findInFilesMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, false, true);
        }

        private void replaceMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, true, false);
        }

        private void replaceInFilesMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, true, true);
        }

        private void makeUpperMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.ActiveDocument.SelectedTextToUpper();
        }

        private void makeLowerMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.ActiveDocument.SelectedTextToLower();
        }

        private void invertCaseMenuItem_Click(object sender, EventArgs e)
        {
			if (DockingService.ActiveDocument == null)
				return;
            DockingService.ActiveDocument.SelectedTextInvertCase();            
        }

        private void sentenceCaseMenuItem_Click(object sender, EventArgs e)
        {
			if (DockingService.ActiveDocument == null)
				return;
			newEditor child = DockingService.ActiveDocument;
            DocumentService.ActiveDocument.SelectedTextToSentenceCase();
        }

        private void toggleBookmarkMenuItem_Click(object sender, EventArgs e)
        {
			if (DocumentService.ActiveDocument == null)
				return;
            DocumentService.ActiveDocument.ToggleBookmark();
        }

        private void prevBookmarkMenuItem_Click(object sender, EventArgs e)
        {
			if (DocumentService.ActiveDocument == null)
				return;
            DocumentService.ActiveDocument.GotoPrevBookmark();
        }

        private void nextBookmarkMenuItem_Click(object sender, EventArgs e)
        {
			if (DocumentService.ActiveDocument == null)
				return;
            DocumentService.ActiveDocument.GotoNextBookmark();
        }

        private void gLineMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            GotoLine gotoBox = new GotoLine(DockingService.ActiveDocument.TotalNumberOfLines);
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
            Preferences prefs = new Preferences();
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
                        DocumentService.ActiveDocument.IsIconBarVisible = item.Checked;
                    break;
                case "lineNumbers":
                    if (ActiveMdiChild != null)
                        DocumentService.ActiveDocument.ShowLineNumbers = item.Checked;
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
                    Settings.Default.mainToolBar = item.Checked;
                    break;
				case "editorToolBar":
					if (item.Checked)
						toolBarManager.AddControl(editorToolStrip);
					else
						toolBarManager.RemoveControl(editorToolStrip);
                    Settings.Default.editorToolbar = item.Checked;
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
				case "callStack":
					if (item.Checked)
						DockingService.ShowDockPanel(DockingService.CallStack);
					else
						DockingService.HideDockPanel(DockingService.CallStack);
					break;
				case "stackViewer":
					if (item.Checked)
						DockingService.ShowDockPanel(DockingService.StackViewer);
					else
						DockingService.HideDockPanel(DockingService.StackViewer);
					break;
				case "varTrack":
					if (item.Checked)
						DockingService.ShowDockPanel(DockingService.TrackWindow);
					else
						DockingService.HideDockPanel(DockingService.TrackWindow);
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
                    Settings.Default.debugToolbar = item.Checked;
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
            debugToolStrip.Height = 25;
        }

        public void UpdateChecks()
        {
			mainToolMenuItem.Checked = toolBarManager.ContainsControl(mainToolBar);
			debugToolMenuItem.Checked = toolBarManager.ContainsControl(debugToolStrip);
			labelListMenuItem.Checked = DockingService.LabelList.Visible;
            projViewMenuItem.Checked = DockingService.ProjectViewer.Visible;
			dirViewMenuItem.Checked = DockingService.DirectoryViewer.Visible;
			findResultsMenuItem.Checked = DockingService.FindResults.Visible;
			//output stuff
			outWinMenuItem.Checked = DockingService.OutputWindow.Visible;
			errListMenuItem.Checked = DockingService.ErrorList.Visible;
			//debug stuff
			breakManagerMenuItem.Checked = DockingService.BreakManager.Visible;
			debugPanelMenuItem.Checked = DockingService.DebugPanel.Visible;
			callStackMenuItem.Checked = DockingService.CallStack.Visible;
			stackViewerMenuItem.Checked = DockingService.StackViewer.Visible;
			varTrackMenuItem.Checked = DockingService.TrackWindow.Visible;
            //misc stuff
			statusBarMenuItem.Checked = this.statusBar.Visible;
			lineNumMenuItem.Checked = Settings.Default.lineNumbers;
			iconBarMenuItem.Checked = Settings.Default.iconBar;
        }

        #endregion

        #region Project Menu

        private void closeProjMenuItem_Click(object sender, EventArgs e)
        {
			CloseProject();
        }

		private void CloseProject()
		{
			ProjectService.CloseProject();
			DockingService.DirectoryViewer.CloseProject();
			DockingService.ProjectViewer.CloseProject();
			UpdateProjectMenu(false);
		}

        private void buildOrderButton_Click(object sender, EventArgs e)
        {
            BuildSteps build = new BuildSteps();
            build.ShowDialog();
			build.Dispose();
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
			AssemblerService.AssembleCurrentFile();
        }

        private void symTableMenuItem_Click(object sender, EventArgs e)
        {
			if (DockingService.ActiveDocument == null)
                return;
			DockingService.ActiveDocument.SaveFile();
			AssemblerService.CreateSymTable(DocumentService.ActiveFileName,
											Path.ChangeExtension(DocumentService.ActiveFileName, "lab"));
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
        private Process wabbitspasm;


        private void SendFileEmu(string assembledName)
        {
            Classes.Resources.GetResource("wabbitemu.exe", "Wabbitemu.exe");
            emulator.StartInfo.Arguments = quote + assembledName + quote;
            emulator.StartInfo.FileName = "Wabbitemu.exe";
            emulator.Start();
			//switch to the emulator
            IntPtr calculatorHandle = NativeMethods.FindWindow("z80", "Wabbitemu");
            NativeMethods.SetForegroundWindow(calculatorHandle);
        }

        const string quote = "\"";
        /// <summary>
        /// Creates a listing file for the currently open file. Very useful for debugging.
        /// </summary>
        /// <param name="filePath">File to create list file of.</param>
        /// <param name="assembledName">Name of the file to create.</param>
        private bool createListing(string filePath, string assembledName)
        {
            Classes.Resources.GetResource("spasm.exe", "spasm.exe");
            //Clear any other assemblings
            //outputWindow.outputWindowBox.Text = "";
            //Get our emulator
            Classes.Resources.GetResource("Wabbitemu.exe", "Wabbitemu.exe");
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
            SpasmMethods.ClearIncludes();
            SpasmMethods.ClearDefines();
            SpasmMethods.AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        SpasmMethods.AddInclude(dir);
                }
            }
            //get the file name we'll use and use it to create the assembled name
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            //now we can set the args for spasm
            int error = 0;
            error |= SpasmMethods.SetInputFile(Path.Combine(originalDir, fileName));
            error |= SpasmMethods.SetOutputFile(Path.Combine(originalDir, assembledName));
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
            SpasmMethods.RunAssembly();
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
            Classes.Resources.GetResource("Revsoft.Wabbitcode.Resources.spasm.exe", "spasm.exe");
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

        #endregion

        #region Debugging

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

		private void startWithoutDebugMenuItem_Click(object sender, EventArgs e)
		{
			DebuggerService.StartWithoutDebug();
		}

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
			//DockingService.DebugPanel.Enabled = DebuggerService.IsBreakpointed;
			//DockingService.CallStack.Enabled = DebuggerService.IsBreakpointed;
			//DockingService.TrackWindow.Enabled = DebuggerService.IsBreakpointed;
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
            UpdateDebugStuff();
        }

		private void stepOverMenuItem_Click(object sender, EventArgs e)
		{
			DebuggerService.StepOver();
            UpdateDebugStuff();
		}

		private void stepOutMenuItem_Click(object sender, EventArgs e)
		{
			DebuggerService.StepOut();
            UpdateDebugStuff();
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
                DockingService.ActiveDocument.Cut();
        }

        private void copyToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.Copy();
        }

        private void pasteToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.Paste();
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
                                          FileName = FileLocations.SpasmFile,
                                          RedirectStandardOutput = true,
                                          RedirectStandardError = true,
                                          UseShellExecute = false,
                                          CreateNoWindow = true
                                      }
                              };

            //some strings we'll need to build 
            originaldir = Path.GetDirectoryName(filePath);
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
        private void AddSquiggleLine(int newLineNumber, Color underlineColor, string description)
        {
            if (DocumentService.ActiveDocument == null) 
                return;
            DocumentService.ActiveDocument.AddSquiggleLine(newLineNumber, underlineColor, description);
        }

        private void documentParser_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            foreach (ArrayList attributes in errorsToAdd)
                AddSquiggleLine((int)attributes[0], Color.Red, attributes[1].ToString());
            if (DocumentService.ActiveDocument != null)
                DocumentService.ActiveDocument.Refresh();
        }

        private void refreshViewMenuItem_Click(object sender, EventArgs e)
        {
            DockingService.ProjectViewer.BuildProjTree();
        }

		private void saveProjectMenuItem_Click(object sender, EventArgs e)
		{
			ProjectService.SaveProject();
            saveProjectMenuItem.Enabled = ProjectService.Project.NeedsSave;
		}

        private void saveAllToolButton_Click(object sender, EventArgs e)
        {
            foreach(newEditor child in MdiChildren)
                DocumentService.SaveDocument(child);
        }

        private void updateMenuItem_Click(object sender, EventArgs e)
        {
			try
			{
				if (UpdateService.CheckForUpdate())
				{
					var result = MessageBox.Show("New version available. Download now?", "Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
					if (result == System.Windows.Forms.DialogResult.Yes)
						UpdateService.StartUpdater();
				}
				else
				{
					MessageBox.Show("No new updates");
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error updating", ex);
			}
        }

        private void aboutMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox box = new AboutBox();
            box.ShowDialog();
			box.Dispose();
        }

        private void newBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            NewBreakpointForm form = new NewBreakpointForm();
            form.ShowDialog();
			form.Dispose();
        }

		private void toggleBreakpointMenuItem_Click(object sender, EventArgs e)
		{
			if (DockingService.ActiveDocument == null)
				return;
            DockingService.ActiveDocument.ToggleBreakpoint();
			DockingService.ActiveDocument.Refresh();

		}

        private void convertSpacesToTabsMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
                return;
            DockingService.ActiveDocument.ConvertSpacesToTabs();
        }

		private void formatDocMenuItem_Click(object sender, EventArgs e)
		{
			if (DockingService.ActiveDocument == null)
				return;
            DockingService.ActiveDocument.FormatLines();
		}

		internal void StartDebug()
		{
			showToolbar = Settings.Default.debugToolbar;
			Settings.Default.debugToolbar = true;
			if (!showToolbar)
				toolBarManager.AddControl(debugToolStrip, DockStyle.Top, mainToolBar, DockStyle.Right);
            UpdateDebugStuff();
			debugToolStrip.Height = mainToolBar.Height;
			UpdateChecks();
			DockingService.ShowDockPanel(DockingService.DebugPanel);
			DockingService.ShowDockPanel(DockingService.TrackWindow);
			DockingService.ShowDockPanel(DockingService.CallStack);
			UpdateTitle();
		}

		internal void UpdateBreakpoints()
		{
			foreach (newEditor child in DockingService.Documents)
			{
                Breakpoint[] marks = child.Breakpoints;
                foreach (Breakpoint breakpoint in marks)
				{
					WabbitcodeBreakpoint newBreakpoint = DebuggerService.FindBreakpoint(child.FileName, breakpoint.LineNumber);
					ListFileValue value = DebuggerService.GetListValue(child.FileName.ToLower(), breakpoint.LineNumber + 1);
                    if (value != null && newBreakpoint != null)
                    {
                        newBreakpoint.Address = value.Address;
                        newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
                        if (DebuggerService.IsAnApp && !newBreakpoint.IsRam)
                            newBreakpoint.Page = (byte)(DebuggerService.AppPage - value.Page);
                        else
                            newBreakpoint.Page = value.Page;
                        newBreakpoint.file = child.FileName;
                        newBreakpoint.lineNumber = breakpoint.LineNumber;
#if NEW_DEBUGGING
                        DebuggerService.SetBreakpoint(newBreakpoint);
#else
                        DebuggerService.Debugger.setBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#endif
                    }
                    else
                        child.RemoveBreakpoint(breakpoint.LineNumber);
                        //editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
				}
				child.CanSetNextStatement = true;
			}
		}

		internal void DoneStep(ListFileKey newKey)
		{
			UpdateStepOut();
            UpdateDebugStuff();
			DocumentService.RemoveDebugHighlight();
			DebuggerService.UpdateStack();
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
                child.RemoveInvisibleMarkers();
				child.CanSetNextStatement = false;
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
			saveProjectMenuItem.Visible = projectOpen;
		}
	}
}