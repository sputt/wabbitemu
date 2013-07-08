using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;

using WabbitemuLib;
using Revsoft.Wabbitcode.Exceptions;
using System.Threading.Tasks;

namespace Revsoft.Wabbitcode
{
    public partial class MainFormRedone : Form
    {
        private const int VK_LSHIFT = 0xA0;
        private const int VK_RSHIFT = 0xA1;
        private const int VK_SHIFT = 16;
        private const int WM_KEYDOWN = 0x0100;

        private readonly ArrayList errorsToAdd = new ArrayList();

        public delegate void DebuggingStarted(object sender, DebuggingEventArgs e);
        public event DebuggingStarted OnDebuggingStarted;

        public delegate void DebuggingEnded(object sender, DebuggingEventArgs e);
        public event DebuggingEnded OnDebuggingEnded;

        private bool showToolbar = true;
        private WabbitcodeDebugger debugger;

        public MainFormRedone(string[] args)
        {
            InitializeComponent();
            this.RestoreWindow();

            if (Settings.Default.mainToolBar)
            {
                mainToolBar.Show();
            }
            else
            {
                mainToolBar.Hide();
            }

            if (Settings.Default.debugToolbar)
            {
                debugToolStrip.Show();
            }
            else
            {
                debugToolStrip.Hide();
            }

            DockingService.InitDocking(dockPanel);
            DockingService.InitPanels();
            if (args.Length == 0)
            {
                this.LoadStartupProject();
            }

            DockingService.LoadConfig();
            try
            {
                if (!ProjectService.IsInternal)
                {
                    DockingService.ProjectViewer.BuildProjTree();
                }
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error building project tree", ex);
            }

            this.HandleArgs(args);
            this.UpdateMenus(DockingService.ActiveDocument != null);
            this.UpdateChecks();
            this.UpdateConfig();

            try
            {
                DocumentService.GetRecentFiles();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error getting recent files", ex);
            }
        }

        public enum MODE
        {
            MODE_NORMAL = 1,
            MODE_CODE_COUNTER = 2,
            MODE_SYMTABLE = 4,
            MODE_STATS = 8,
            MODE_LIST = 16,
            MODE_COMMANDLINE = 32
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
                        debugger.setBreakpoint(Handle, newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
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
            debugger.CancelDebug();
        }

        public void MainFormRedone_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop) == false)
            {
                return;
            }

            string[] files = (string[]) e.Data.GetData(DataFormats.FileDrop);
            foreach (string file in files)
            {
                DocumentService.OpenDocument(file);
            }
        }

        public void MainFormRedone_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Copy : DragDropEffects.None;
        }

        public void ProcessParameters(object sender, string[] args)
        {
            // The form has loaded, and initialization will have been be done.
            this.HandleArgs(args);
            NativeMethods.SetForegroundWindow(this.Handle);
        }

        public void UpdateChecks()
        {
            if (this.IsDisposed || this.Disposing)
            {
                return;
            }

            mainToolMenuItem.Checked = true; // toolBarManager.ContainsControl(mainToolBar);
            debugToolMenuItem.Checked = false; // toolBarManager.ContainsControl(debugToolStrip);
            labelListMenuItem.Checked = DockingService.LabelList.Visible;
            projViewMenuItem.Checked = DockingService.ProjectViewer.Visible;
            dirViewMenuItem.Checked = DockingService.DirectoryViewer.Visible;
            findResultsMenuItem.Checked = DockingService.FindResults.Visible;

            // output stuff
            outWinMenuItem.Checked = DockingService.OutputWindow.Visible;
            errListMenuItem.Checked = DockingService.ErrorList.Visible;

            // debug stuff
            breakManagerMenuItem.Checked = DockingService.BreakManager.Visible;
            debugPanelMenuItem.Checked = DockingService.DebugPanel.Visible;
            callStackMenuItem.Checked = DockingService.CallStack.Visible;
            stackViewerMenuItem.Checked = DockingService.StackViewer.Visible;
            varTrackMenuItem.Checked = DockingService.TrackWindow.Visible;

            // misc stuff
            statusBarMenuItem.Checked = this.statusBar.Visible;
            lineNumMenuItem.Checked = Settings.Default.lineNumbers;
            iconBarMenuItem.Checked = Settings.Default.iconBar;
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

        public void UpdateConfig()
        {
            if (ProjectService.IsInternal)
            {
                return;
            }

            foreach (var config in ProjectService.BuildConfigs)
            {
                configBox.Items.Add(config);
            }

            configBox.SelectedIndex = ProjectService.CurrentConfigIndex;
        }

        public void UpdateDebugStuff()
        {
            bool isBreakpointed = debugger != null ? debugger.IsBreakpointed : false;
            bool isDebugging = debugger != null;
            bool enabled = isBreakpointed && isDebugging;
            stepMenuItem.Enabled = enabled;
            gotoCurrentToolButton.Enabled = enabled;
            stepToolButton.Enabled = enabled;
            stepOverMenuItem.Enabled = enabled;
            stepOverToolButton.Enabled = enabled;
            stepOutMenuItem.Enabled = enabled;
            stepOutToolButton.Enabled = enabled;
            stopDebugMenuItem.Enabled = isDebugging;
            stopToolButton.Enabled = isDebugging;
            pauseToolButton.Enabled = !isBreakpointed && isDebugging;
        }

        /// <summary>
        /// Updates the title of the app with the filename.
        /// </summary>
        public void UpdateTitle()
        {
            string debugString = string.Empty;
            if (debugger != null)
            {
                debugString = " (Debugging)";
            }
            if (!string.IsNullOrEmpty(DocumentService.ActiveFileName))
            {
                Text = Path.GetFileName(DocumentService.ActiveFileName) + debugString + " - Wabbitcode";
            }
            else
            {
                Text = "Wabbitcode" + debugString;
            }
        }

        internal void AddRecentItem(string file)
        {
            ToolStripMenuItem button = new ToolStripMenuItem(file, null, this.openRecentDoc);
            recentFilesMenuItem.DropDownItems.Add(button);
        }

        internal void CancelDebug()
        {
            this.UpdateTitle();
            this.UpdateDebugStuff();
            if (DockingService.DebugPanel != null)
            {
                DockingService.HideDockPanel(DockingService.DebugPanel);
            }

            if (DockingService.TrackWindow != null)
            {
                DockingService.HideDockPanel(DockingService.TrackWindow);
            }

            if (DockingService.CallStack != null)
            {
                DockingService.HideDockPanel(DockingService.CallStack);
            }

            Settings.Default.debugToolbar = showToolbar;
            if (!showToolbar)
            {
                debugToolStrip.Visible = false;
            }

            this.UpdateChecks();
            DocumentService.RemoveDebugHighlight();
            foreach (NewEditor child in MdiChildren)
            {
                child.RemoveInvisibleMarkers();
                child.CanSetNextStatement = false;
            }

            if (OnDebuggingEnded != null)
            {
                OnDebuggingEnded(this, new DebuggingEventArgs(null));
            }
        }

        internal void ClearRecentItems()
        {
            recentFilesMenuItem.DropDownItems.Clear();
        }

        internal void DoneStep(ListFileKey newKey)
        {
            this.UpdateStepOut();
            this.UpdateDebugStuff();
            DocumentService.RemoveDebugHighlight();
            DocumentService.GotoLine(newKey.FileName, newKey.LineNumber);
            DocumentService.HighlightDebugLine(newKey.LineNumber);
            debugger.UpdateStack();
            DockingService.MainForm.UpdateTrackPanel();
            DockingService.MainForm.UpdateDebugPanel();
        }

        internal void HideProgressBar()
        {
            progressBar.Visible = false;
        }

        internal void IncrementProgress(int progress)
        {
            progressBar.Visible = true;
            progressBar.Increment(progress);
        }

        internal void SetLineAndColStatus(string line, string col)
        {
            lineStatusLabel.Text = "Ln: " + line;
            colStatusLabel.Text = "Col: " + col;
        }

        internal void SetProgress(int percent)
        {
            progressBar.Visible = true;
            progressBar.Value = percent;
        }

        internal void SetToolStripText(string text)
        {
            toolStripStatusLabel.Text = text;
        }

        internal void StartDebug()
        {
            if (ProjectService.IsInternal)
            {
                throw new DebuggingException("Debugging single files is not supported");
            }
            DoAssembly(null, (sender, e) =>
            {
                if (!e.AssemblySucceeded)
                {
                    if (DockingService.ShowMessageBox(DockingService.MainForm,
                                    "There were errors assembling. Would you like to continue and try to debug?",
                                    "Continue",
                                    MessageBoxButtons.YesNo,
                                    MessageBoxIcon.Error) == DialogResult.No)
                    {
                        CancelDebug();
                        return;
                    }
                    debugger = new WabbitcodeDebugger();
                    string createdName = debugger.ParseOutputFiles(e.Project);
                    debugger.InitDebugger(createdName);

                    if (OnDebuggingStarted != null)
                    {
                        OnDebuggingStarted(this, new DebuggingEventArgs(debugger));
                    }

                    if (debugger.IsAnApp)
                    {
                        try
                        {
                            debugger.VerifyApp(createdName);
                        }
                        catch (DebuggingException)
                        {
                            if (DockingService.ShowMessageBox(this, "Unable to find the app, would you like to try and continue and debug?",
                                    "Missing App",
                                    MessageBoxButtons.YesNo,
                                    MessageBoxIcon.Exclamation) != DialogResult.Yes)
                            {
                                CancelDebug();
                                return;
                            }
                        }

                        int counter = 0;
                        // apps key
                        debugger.SimulateKeyPress(Keys.B);
                        for (; counter >= 0; counter--)
                        {
                            debugger.SimulateKeyPress(Keys.Down);
                        }

                        debugger.SimulateKeyPress(Keys.Enter);

                        this.Invoke(() =>
                        {
                            UpdateDebugStuff();
                            UpdateBreakpoints();
                            ShowDebugPanels();
                        });
                    }
                }
            });
            return;
        }

        private void ShowDebugPanels()
        {
            showToolbar = Settings.Default.debugToolbar;
            Settings.Default.debugToolbar = true;
            if (!showToolbar)
            {
                debugToolStrip.Visible = true;
            }

            debugToolStrip.Height = mainToolBar.Height;
            this.UpdateChecks();
            DockingService.ShowDockPanel(DockingService.DebugPanel);
            DockingService.ShowDockPanel(DockingService.TrackWindow);
            DockingService.ShowDockPanel(DockingService.CallStack);
            this.UpdateTitle();
        }

        internal void UpdateBreakpoints()
        {
            foreach (NewEditor child in DockingService.Documents)
            {
                Breakpoint[] marks = child.Breakpoints;
                foreach (Breakpoint breakpoint in marks)
                {
                    WabbitcodeBreakpoint newBreakpoint = debugger.FindBreakpoint(child.FileName, breakpoint.LineNumber);
                    ListFileValue value = debugger.GetListValue(child.FileName.ToLower(), breakpoint.LineNumber + 1);
                    if (value != null && newBreakpoint != null)
                    {
                        newBreakpoint.Address = value.Address;
                        newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
                        if (debugger.IsAnApp && !newBreakpoint.IsRam)
                        {
                            newBreakpoint.Page = (byte)(debugger.AppPage - value.Page);
                        }
                        else
                        {
                            newBreakpoint.Page = value.Page;
                        }

                        newBreakpoint.file = child.FileName;
                        newBreakpoint.lineNumber = breakpoint.LineNumber;
                        debugger.SetBreakpoint(newBreakpoint);
                    }
                    else
                    {
                        child.RemoveBreakpoint(breakpoint.LineNumber);

                        // editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
                    }
                }

                child.CanSetNextStatement = true;
            }
        }

        internal void UpdateDebugPanel()
        {
            DockingService.DebugPanel.UpdateFlags();
            DockingService.DebugPanel.UpdateRegisters();
            DockingService.DebugPanel.UpdateScreen();
        }

        /// <summary>
        /// Updates all the menu items that depend on if there is an active child open.
        /// </summary>
        /// <param name="enabled">Whether items should be enabled or disabled.</param>
        internal void UpdateMenus(bool enabled)
        {
            // Main Toolbar
            saveToolStripButton.Enabled = enabled;
            saveAllToolButton.Enabled = enabled;
            cutToolStripButton.Enabled = enabled;
            copyToolStripButton.Enabled = enabled;
            pasteToolStripButton.Enabled = enabled;
            findBox.Enabled = enabled;

            // File Menu
            saveMenuItem.Enabled = enabled;
            saveAsMenuItem.Enabled = enabled;
            saveAllMenuItem.Enabled = enabled;
            closeMenuItem.Enabled = enabled;

            // Edit Menu
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

            // View Menu
            lineNumMenuItem.Enabled = enabled;
            iconBarMenuItem.Enabled = enabled;

            // Refactor Menu
            renameMenuItem.Enabled = enabled;
            extractMethodMenuItem.Enabled = enabled;

            // Assemble Menu
            symTableMenuItem.Enabled = enabled;
            projStatsMenuItem.Enabled = enabled;
            listFileMenuItem.Enabled = enabled;

            // Debug Menu
            if (!enabled)
            {
                this.UpdateDebugStuff();
            }

            toggleBreakpointMenuItem.Enabled = enabled;
            if (ProjectService.IsInternal)
            {
                startDebugMenuItem.Enabled = enabled;
                startWithoutDebugMenuItem.Enabled = enabled;
                runToolButton.Enabled = enabled;
                runMenuItem.Enabled = enabled;
                runDebuggerToolButton.Enabled = enabled;
                assembleMenuItem.Enabled = enabled;
            }
            else
            {
                startDebugMenuItem.Enabled = true;
                startWithoutDebugMenuItem.Enabled = true;
                runToolButton.Enabled = true;
                runMenuItem.Enabled = true;
                runDebuggerToolButton.Enabled = true;
                assembleMenuItem.Enabled = true;
            }

            // Window Menu
            windowMenuItem.Enabled = enabled;
        }

        internal void UpdateProjectMenu(bool projectOpen)
        {
            projMenuItem.Visible = projectOpen;
            includeDirButton.Visible = !projectOpen;
            saveProjectMenuItem.Visible = projectOpen;
        }

        internal void UpdateTrackPanel()
        {
            DockingService.TrackWindow.updateVars();
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            switch (msg.Msg)
            {
            case WM_KEYDOWN:
                if ((int)msg.WParam == VK_SHIFT)
                {
                    if ((NativeMethods.GetKeyState(VK_LSHIFT) & 0xFF00) != 0)
                    {
                        msg.WParam = (IntPtr)VK_LSHIFT;
                    }
                    else
                    {
                        msg.WParam = (IntPtr)VK_RSHIFT;
                    }
                }

                break;
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        private static bool InvisibleMarkers(TextMarker marker)
        {
            return marker.TextMarkerType == TextMarkerType.Invisible;
        }

        private void aboutMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox box = new AboutBox();
            box.ShowDialog();
            box.Dispose();
        }

        private void addNewFileMenuItem_Click(object sender, EventArgs e)
        {
            RenameForm newNameForm = new RenameForm();
            newNameForm.Text = "New File";
            var result = newNameForm.ShowDialog() != DialogResult.OK;
            newNameForm.Dispose();
            if (result)
            {
                return;
            }

            string name = newNameForm.NewText;
            DockingService.ProjectViewer.AddNewFile(name);
        }

        private void AddSquiggleLine(int newLineNumber, Color underlineColor, string description)
        {
            if (DocumentService.ActiveDocument == null)
            {
                return;
            }

            DocumentService.ActiveDocument.AddSquiggleLine(newLineNumber, underlineColor, description);
        }

        private void DoAssembly(AssemblerService.OnFinishAssemblyFile fileEventHandler, AssemblerService.OnFinishAssemblyProject projectEventHandler)
        {
            DockingService.OutputWindow.ClearOutput();
            if (!ProjectService.IsInternal)
            {
                if (projectEventHandler != null)
                {
                    AssemblerService.Instance.AssemblerProjectFinished += projectEventHandler;
                }
                Task.Factory.StartNew(() => 
                {
                    AssemblerService.Instance.AssembleProject(ProjectService.Project);
                    if (projectEventHandler != null)
                    {
                        AssemblerService.Instance.AssemblerProjectFinished -= projectEventHandler;
                    }
                });
            }
            else if (DockingService.ActiveDocument != null)
            {
                bool saved = DockingService.ActiveDocument.SaveFile();
                string inputFile = DockingService.ActiveDocument.FileName;
                if (saved)
                {
                    if (fileEventHandler != null)
                    {
                        AssemblerService.Instance.AssemblerFileFinished += fileEventHandler;
                    }
                    Task.Factory.StartNew(() =>
                    {
                        AssemblerService.Instance.AssembleFile(inputFile);
                        if (fileEventHandler != null)
                        {
                            AssemblerService.Instance.AssemblerFileFinished -= fileEventHandler;
                        }
                    });
                }
            }
            else
            {
                return;
            }
        }   

        private void assembleMenuItem_Click(object sender, EventArgs e)
        {
            DoAssembly(OnAssemblyFinished, OnAssemblyFinished);
        }

        private void buildOrderButton_Click(object sender, EventArgs e)
        {
            using (BuildSteps build = new BuildSteps())
            {
                build.ShowDialog();
            }
        }

        private void closeMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild != null)
            {
                ActiveMdiChild.Close();
            }
        }

        private void CloseProject()
        {
            ProjectService.CloseProject();
            DockingService.DirectoryViewer.CloseProject();
            DockingService.ProjectViewer.CloseProject();
            this.UpdateProjectMenu(false);
        }

        private void closeProjMenuItem_Click(object sender, EventArgs e)
        {
            this.CloseProject();
        }

        private void configBox_SelectedIndexChanged(object sender, System.EventArgs e)
        {
            ProjectService.CurrentConfigIndex = configBox.SelectedIndex;
        }

        private void convertSpacesToTabsMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.ConvertSpacesToTabs();
        }

        private void Copy()
        {
            if (DockingService.ActiveContent is ToolWindow)
            {
                ((ToolWindow)DockingService.ActiveContent).Copy();
            }
            else if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Copy();
            }
        }

        private void copyMenuItem_Click(object sender, EventArgs e)
        {
            this.Copy();
        }

        private void copyToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Copy();
            }
        }

        private void countCodeMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void Cut()
        {
            if (DockingService.ActiveContent is ToolWindow)
            {
                ((ToolWindow)DockingService.ActiveContent).Cut();
            }
            else if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Cut();
            }
        }

        private void cutMenuItem_Click(object sender, EventArgs e)
        {
            this.Cut();
        }

        private void cutToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Cut();
            }
        }

        private void dockPanel_ActiveDocumentChanged(object sender, EventArgs e)
        {
            if (Disposing)
            {
                return;
            }

            if (ActiveMdiChild != null)
            {
                this.UpdateMenus(true);
                DockingService.ActiveDocument.UpdateLabelBox();
            }
            else
            {
                this.UpdateMenus(false);
                DockingService.LabelList.DisableLabelBox();
            }

            this.UpdateTitle();
        }

        private void documentParser_DoWork(object sender, DoWorkEventArgs e)
        {
            string originaldir = string.Empty;
            ArrayList arguments = (ArrayList)e.Argument;
            TextEditorControl editorBox = (TextEditorControl)arguments[0];
            string text = arguments[1].ToString();
            foreach (TextMarker marker in editorBox.Document.MarkerStrategy.TextMarker)
            {
                if (marker.Tag == "Code Check")
                {
                    editorBox.Document.MarkerStrategy.RemoveMarker(marker);
                }
            }

            string filePath = editorBox.FileName;

            // setup wabbitspasm to run silently
            Process wabbitspasm = new Process
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

            // some strings we'll need to build
            originaldir = Path.GetDirectoryName(filePath);
            string includedir = "-I \"" + Application.StartupPath + "\"";
            if (!string.IsNullOrEmpty(Settings.Default.includeDir))
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (!string.IsNullOrEmpty(dir))
                    {
                        includedir += ";\"" + dir + "\"";
                    }
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
                {
                    continue;
                }

                int firstColon = line.IndexOf(':');
                int secondColon = line.IndexOf(':', firstColon + 1);
                int thirdColon = line.IndexOf(':', secondColon + 1);
                int lineNum = Convert.ToInt32(line.Substring(firstColon + 1, secondColon - firstColon - 1));
                string description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                ArrayList listofattributes = new ArrayList { lineNum, description };
                errorsToAdd.Add(listofattributes);
            }
        }

        private void documentParser_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            foreach (ArrayList attributes in errorsToAdd)
            {
                this.AddSquiggleLine((int)attributes[0], Color.Red, attributes[1].ToString());
            }

            if (DocumentService.ActiveDocument != null)
            {
                DocumentService.ActiveDocument.Refresh();
            }
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
            {
                return;
            }

            foreach (string file in openFileDialog.FileNames)
            {
                DockingService.ProjectViewer.AddExistingFile(file);
            }
        }

        private void exitMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void extractMethodMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void findAllRefsMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            ParserService parserService = new ParserService();
            string word = DockingService.ActiveDocument.GetWord();
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

        private void findBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != (char)Keys.Enter)
            {
                return;
            }

            if (ActiveMdiChild == null)
            {
                return;
            }

            if (!findBox.Items.Contains(findBox.Text))
            {
                findBox.Items.Add(findBox.Text);
            }

            bool found = DocumentService.ActiveDocument.Find(findBox.Text);
            if (!found)
            {
                MessageBox.Show("Text not found");
            }
        }

        private void findInFilesMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                DockingService.FindForm.ShowFor(false, true);
            }
            else
            {
                DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, false, true);
            }
        }

        private void findMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, false, false);
        }

        private void formatDocMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.FormatLines();
        }

        private void gLabelMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void gLineMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            GotoLine gotoBox = new GotoLine(DockingService.ActiveDocument.TotalNumberOfLines);
            DialogResult gotoResult = gotoBox.ShowDialog();
            if (gotoResult != DialogResult.OK)
            {
                return;
            }

            int line = Convert.ToInt32(gotoBox.inputBox.Text);
            DocumentService.GotoLine(line);
        }

        private void gotoCurrentToolButton_Click(object sender, EventArgs e)
        {
            DocumentService.GotoCurrentDebugLine();
        }

        private void HandleArgs(string[] args)
        {
            if (args.Length == 0)
            {
                return;
            }
            else
            {
                foreach (string arg in args)
                {
                    try
                    {
                        NewEditor doc = new NewEditor(this);
                        if (string.IsNullOrEmpty(arg))
                        {
                            break;
                        }
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
            }
        }

        private void hexFileMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void includeDirButton_Click(object sender, EventArgs e)
        {
            IncludeDir includes = new IncludeDir();
            includes.ShowDialog();
            includes.Dispose();
        }

        private void invertCaseMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.SelectedTextInvertCase();
        }

        private void listFileMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.SaveFile();
            string inputFile = DockingService.ActiveDocument.FileName;
            string outputFile = Path.ChangeExtension(inputFile, "lst");
            AssemblerService.Instance.AssembleFile(inputFile, outputFile, AssemblyFlags.List | AssemblyFlags.Normal);
        }

        private void LoadStartupProject()
        {
            if (string.IsNullOrEmpty(Settings.Default.startupProject))
            {
                return;
            }

            try
            {
                bool valid = false;
                if (File.Exists(Settings.Default.startupProject))
                {
                    valid = ProjectService.OpenProject(Settings.Default.startupProject);
                }
                else
                {
                    Settings.Default.startupProject = string.Empty;
                    DockingService.ShowError("Error: Project file not found");
                }

                if (ProjectService.IsInternal || !valid)
                {
                    ProjectService.CreateInternalProject();
                }
            }
            catch (Exception ex)
            {
                ProjectService.CreateInternalProject();
                var result = MessageBox.Show(
                                 "There was an error loading the startup project, would you like to remove it?\n" + ex.ToString(),
                                 "Error",
                                 MessageBoxButtons.YesNo,
                                 MessageBoxIcon.Error);
                if (result == DialogResult.Yes)
                {
                    Settings.Default.startupProject = string.Empty;
                }
            }
        }

        private void MainFormRedone_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (debugger != null)
            {
                debugger.CancelDebug();
            }

            if (!ProjectService.IsInternal)
            {
                this.CloseProject();
            }

            try
            {
                this.SaveWindow();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error saving window location", ex);
            }

            try
            {
                DockingService.Destroy();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error destroying DockService", ex);
            }

            try
            {
                Settings.Default.Save();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error saving settings", ex);
            }
        }

        private void makeLowerMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.SelectedTextToLower();
        }

        private void makeUpperMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.SelectedTextToUpper();
        }

        private void newBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            NewBreakpointForm form = new NewBreakpointForm();
            form.ShowDialog();
            form.Dispose();
        }

        private void newFileMenuItem_Click(object sender, EventArgs e)
        {
            NewEditor doc = DocumentService.CreateNewDocument();
            DockingService.ShowDockPanel(doc);
        }

        private void newProjectMenuItem_Click(object sender, EventArgs e)
        {
            NewProjectDialog template = new NewProjectDialog();
            if (template.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            this.UpdateProjectMenu(true);
        }

        private void newToolButton_Click(object sender, EventArgs e)
        {
            NewEditor doc = DocumentService.CreateNewDocument();
            doc.TabText = "New Document";
            doc.Show(dockPanel);
        }

        private void nextBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (DocumentService.ActiveDocument == null)
            {
                return;
            }

            DocumentService.ActiveDocument.GotoNextBookmark();
        }

        private void OnAssemblyFinished(object sender, AssemblyFinishEventArgs e)
        {
            AssemblerService.Instance.AssemblerProjectFinished -= this.OnAssemblyFinished;
            string originalDir;
            if (e is AssemblyFinishFileEventArgs)
            {
                originalDir = Path.GetDirectoryName(((AssemblyFinishFileEventArgs)e).InputFile);
            }
            else
            {
                originalDir = ((AssemblyFinishProjectEventArgs)e).Project.ProjectDirectory;
            }

            DockingService.MainForm.Invoke(() =>
                                           this.ShowErrorPanels(e.Output, originalDir));
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
            try
            {
                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    if (!ProjectService.OpenProject(openFileDialog.FileName))
                    {
                        ProjectService.CreateInternalProject();
                    }
                }
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error opening file.", ex);
            }

            this.UpdateMenus(DockingService.Documents.Count() > 0);
        }

        /// <summary>
        /// This opens the recend document clicked in the file menu.
        /// </summary>
        /// <param name="sender">This is the button object. This is casted to get which button was clicked.</param>
        /// <param name="e">Nobody cares about this arg.</param>
        private void openRecentDoc(object sender, EventArgs e)
        {
            MenuItem button = (MenuItem)sender;
            DocumentService.OpenDocument(button.Text);
        }

        private void openToolButton_Click(object sender, EventArgs e)
        {
            try
            {
                DocumentService.OpenDocument();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error opening file", ex);
            }
        }

        private void Paste()
        {
            if (DockingService.ActiveContent is ToolWindow)
            {
                ((ToolWindow)DockingService.ActiveContent).Paste();
            }
            else if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Paste();
            }
        }

        private void pasteMenuItem_Click(object sender, EventArgs e)
        {
            this.Paste();
        }

        private void pasteToolButton_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Paste();
            }
        }

        private void pauseToolButton_Click(object sender, EventArgs e)
        {
            debugger.Pause();
        }

        private void prefsMenuItem_Click(object sender, EventArgs e)
        {
            Preferences prefs = new Preferences();
            prefs.ShowDialog();
        }

        private void prevBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (DocumentService.ActiveDocument == null)
            {
                return;
            }

            DocumentService.ActiveDocument.GotoPrevBookmark();
        }

        private void projStatsMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.SaveFile();
            string inputFile = DockingService.ActiveDocument.FileName;
            AssemblerService.Instance.AssembleFile(inputFile, string.Empty, AssemblyFlags.Stats);
        }

        private void redoMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Redo();
            }
        }

        private void refreshViewMenuItem_Click(object sender, EventArgs e)
        {
            DockingService.ProjectViewer.BuildProjTree();
        }

        private void renameMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            RefactorForm form = new RefactorForm();
            form.ShowDialog();
        }

        private void replaceInFilesMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, true, true);
        }

        private void replaceMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.FindForm.ShowFor(DockingService.ActiveDocument.EditorBox, true, false);
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

        private void runMenuItem_Click(object sender, EventArgs e)
        {
            debugger.Run();
        }

        private void saveAllToolButton_Click(object sender, EventArgs e)
        {
            foreach (NewEditor child in MdiChildren)
            {
                DocumentService.SaveDocument(child);
            }
        }

        private void saveAsMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                DocumentService.SaveDocumentAs();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error saving file.", ex);
            }
        }

        private void saveMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                DocumentService.SaveDocument();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error saving file.", ex);
            }
        }

        private void saveProjectMenuItem_Click(object sender, EventArgs e)
        {
            ProjectService.SaveProject();
            saveProjectMenuItem.Enabled = ProjectService.Project.NeedsSave;
        }

        private void saveToolButton_Click(object sender, EventArgs e)
        {
            DocumentService.SaveDocument();
        }

        private void SaveWindow()
        {
            if (this.WindowState != FormWindowState.Normal)
            {
                Settings.Default.WindowSize = new Size(this.RestoreBounds.Width, this.RestoreBounds.Height);
            }
            else
            {
                Settings.Default.WindowSize = this.Size;
            }

            Settings.Default.WindowState = this.WindowState;
        }

        private void selectAllMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DocumentService.ActiveDocument.SelectAll();
        }

        private void SendFileEmu(string assembledName)
        {
            Process emulator = new Process();
            Classes.Resources.GetResource("wabbitemu.exe", "Wabbitemu.exe");
            emulator.StartInfo.Arguments = "\"" + assembledName + "\"";
            emulator.StartInfo.FileName = "Wabbitemu.exe";
            emulator.Start();

            // switch to the emulator
            IntPtr calculatorHandle = NativeMethods.FindWindow("z80", "Wabbitemu");
            NativeMethods.SetForegroundWindow(calculatorHandle);
        }

        private void sentenceCaseMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            NewEditor child = DockingService.ActiveDocument;
            DocumentService.ActiveDocument.SelectedTextToSentenceCase();
        }

        private void ShowErrorPanels(AssemblerOutput output, string originalDir)
        {
            try
            {
                DockingService.OutputWindow.ClearOutput();
                DockingService.OutputWindow.AddText(output.OutputText);
                DockingService.OutputWindow.HighlightOutput();

                // its more fun with colors
                DockingService.ErrorList.ParseOutput(output.ParsedErrors, originalDir);
                DockingService.ShowDockPanel(DockingService.ErrorList);
                DockingService.ShowDockPanel(DockingService.OutputWindow);
                if (DockingService.ActiveDocument != null)
                {
                    DockingService.ActiveDocument.Refresh();
                }

                foreach (NewEditor child in DockingService.Documents)
                {
                    child.UpdateIcons(output.ParsedErrors);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void startDebugMenuItem_Click(object sender, EventArgs e)
        {
            if (debugger == null)
            {
                StartDebug();
            }
            else
            {
                debugger.Run();
            }
        }

        private void startWithoutDebugMenuItem_Click(object sender, EventArgs e)
        {
            debugger.StartWithoutDebug();
        }

        private void stepButton_Click(object sender, EventArgs e)
        {
            debugger.Step();
            this.UpdateDebugStuff();
        }

        private void stepOutMenuItem_Click(object sender, EventArgs e)
        {
            debugger.StepOut();
            this.UpdateDebugStuff();
        }

        private void stepOverMenuItem_Click(object sender, EventArgs e)
        {
            debugger.StepOver();
            this.UpdateDebugStuff();
        }

        private void symTableMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.SaveFile();
            string outputFile = Path.ChangeExtension(DocumentService.ActiveFileName, "lab");
            AssemblerService.Instance.AssembleFile(DocumentService.ActiveFileName, outputFile, AssemblyFlags.Normal | AssemblyFlags.Symtable, false);
        }

        private void toggleBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (DocumentService.ActiveDocument == null)
            {
                return;
            }

            DocumentService.ActiveDocument.ToggleBookmark();
        }

        private void toggleBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument == null)
            {
                return;
            }

            DockingService.ActiveDocument.ToggleBreakpoint();
            DockingService.ActiveDocument.Refresh();
        }

        private void undoMenuItem_Click(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
            {
                DockingService.ActiveDocument.Undo();
            }
        }

        private void updateMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (UpdateService.CheckForUpdate())
                {
                    var result = MessageBox.Show("New version available. Download now?", "Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
                    if (result == System.Windows.Forms.DialogResult.Yes)
                    {
                        UpdateService.StartUpdater();
                    }
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

        private void UpdateStepOut()
        {
            if (debugger.StepStack.Count > 0)
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

        /// <summary>
        /// This handles all things relating to the view menu. Just does a switch based
        /// on the tag, and does the appropriate action based on the check mark state
        /// this probably isnt a great way to handle it, but it works
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void viewMenuItem_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;
            item.Checked = !item.Checked;
            switch (item.Tag.ToString())
            {
            case "iconBar":
                if (ActiveMdiChild != null)
                {
                    DocumentService.ActiveDocument.IsIconBarVisible = item.Checked;
                }

                break;
            case "lineNumbers":
                if (ActiveMdiChild != null)
                {
                    DocumentService.ActiveDocument.ShowLineNumbers = item.Checked;
                }

                break;
            case "labelsList":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.LabelList);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.LabelList);
                }

                break;
            case "mainToolBar":
                if (item.Checked)
                {
                    mainToolBar.Show();
                }
                else
                {
                    mainToolBar.Hide();
                }

                Settings.Default.mainToolBar = item.Checked;
                break;
            case "editorToolBar":
                if (item.Checked)
                {
                    editorToolStrip.Show();
                }
                else
                {
                    editorToolStrip.Hide();
                }

                Settings.Default.editorToolbar = item.Checked;
                break;
            case "outputWindow":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.OutputWindow);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.OutputWindow);
                }

                break;
            case "FindResults":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.FindResults);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.FindResults);
                }

                break;
            case "statusBar":
                statusBar.Visible = item.Checked;
                break;
            case "debugPanel":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.DebugPanel);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.DebugPanel);
                }

                break;
            case "callStack":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.CallStack);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.CallStack);
                }

                break;
            case "stackViewer":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.StackViewer);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.StackViewer);
                }

                break;
            case "varTrack":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.TrackWindow);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.TrackWindow);
                }

                break;
            case "breakManager":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.BreakManager);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.BreakManager);
                }

                break;
            case "projectViewer":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.ProjectViewer);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.ProjectViewer);
                }

                break;
            case "directoryViewer":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.DirectoryViewer);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.DirectoryViewer);
                }

                break;
            case "debugToolBar":
                if (item.Checked)
                {
                    debugToolStrip.Show();
                }
                else
                {
                    debugToolStrip.Hide();
                }

                Settings.Default.debugToolbar = item.Checked;
                break;
            case "errorList":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.ErrorList);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.ErrorList);
                }

                break;
            case "macroManager":
                if (item.Checked)
                {
                    DockingService.ShowDockPanel(DockingService.MacroManager);
                }
                else
                {
                    DockingService.HideDockPanel(DockingService.MacroManager);
                }

                break;
            }

            debugToolStrip.Height = 25;
        }

        internal string TranlateSymbolToAddress(string text)
        {
            if (debugger == null)
            {
                return string.Empty;
            }
            if (debugger.SymbolTable.StaticLabels.Contains(text))
            {
                return debugger.SymbolTable.StaticLabels[text].ToString();
            }
            return string.Empty;
        }

        internal void RemoveBreakpoint(int lineNumber, string fileName)
        {
            if (debugger != null)
            {
                debugger.RemoveBreakpoint(lineNumber, fileName);
            }

        }

        internal void AddBreakpoint(int lineNumber, string fileName)
        {
            if (debugger != null)
            {
                debugger.AddBreakpoint(lineNumber, fileName);
            }
        }

        internal void UpdateAssembledInfo(string fileName, int lineNumber)
        {
            ListFileValue label = debugger.GetListValue(fileName, lineNumber);
            if (label != null)
            {
                string assembledInfo = "Page: " + label.Page.ToString() + " Address: " + label.Address.ToString("X4");
                SetToolStripText(assembledInfo);
            }
        }

        internal void AddStackEntry(int lineNumber)
        {
            debugger.StepStack.Push(lineNumber);
        }

        internal void SetPC(string fileName, int lineNumber)
        {
            debugger.SetPCToSelect(fileName, lineNumber);
        }
    }
}