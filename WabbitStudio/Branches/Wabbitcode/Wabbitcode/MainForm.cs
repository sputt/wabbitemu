using System.ComponentModel;
using Microsoft.Win32;
using Revsoft.Wabbitcode.DockingWindows;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using IFileReaderService = Revsoft.Wabbitcode.Services.IFileReaderService;

namespace Revsoft.Wabbitcode
{
	public partial class MainForm : Form
	{
		#region Private Members

	    private bool _showToolbar = true;
        private readonly object _codeInfoLock = new object();
		private IWabbitcodeDebugger _debugger;
        private readonly Dictionary<string, string> _foldingDictionary = new Dictionary<string, string>();
        private BackgroundWorker _debuggingWorker;

		#region Services

		private IDockingService _dockingService;
		private IProjectService _projectService;
		private IAssemblerService _assemblerService;
		private IParserService _parserService;
		private ISymbolService _symbolService;
		private IBackgroundAssemblerService _backgroundAssemblerService;
		private IDocumentService _documentService;
	    private IFileReaderService _fileReaderService;

		#endregion

		#endregion

		#region Events

		#endregion

		public MainForm(string[] args)
		{
			InitializeComponent();
			RestoreWindow();
			InitiailzeToolbars();
			InitializeService();
			InitializeEvents();

			DockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;
		
			WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
			WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnBreakpointRemoved;

            Editor.OnEditorOpened += Editor_OnEditorOpened;
            Editor.OnEditorClosing += Editor_OnEditorClosing;
            Editor.OnEditorToolTipRequested += Editor_OnEditorToolTipRequested;

			_dockingService.InitPanels(new ProjectViewer(_dockingService, _documentService, _projectService),
				new ErrorList(_assemblerService, _dockingService, _documentService, _projectService),
				new TrackingWindow(_dockingService, _symbolService),
				new DebugPanel(_dockingService),
				new CallStack(_dockingService, _documentService),
				new LabelList(_dockingService, _documentService, _parserService),
				new OutputWindow(_dockingService, _documentService),
				new FindAndReplaceForm(_dockingService, _projectService),
				new FindResultsWindow(_dockingService, _documentService),
				new MacroManager(_dockingService),
				new BreakpointManagerWindow(_dockingService, _documentService, _projectService),
				new StackViewer(_dockingService),
                new ExpressionWindow(_dockingService, _symbolService));
			_dockingService.LoadConfig(GetContentFromPersistString);

			if (args.Length == 0)
			{
				LoadStartupProject();
			}

			if (_projectService.Project == null)
			{
				_projectService.CreateInternalProject();
			}

			try
			{
                // TOOD: why is this here
				if (!_projectService.Project.IsInternal)
				{
					_projectService.Project.InitWatcher(projectWatcher_Changed, projectWatcher_Renamed);
					_dockingService.ProjectViewer.BuildProjTree();
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error building project tree", ex);
			}

			HandleArgs(args);
			UpdateMenus(_dockingService.ActiveDocument != null);
			UpdateChecks();
			UpdateConfigToolbarBox();

			try
			{
                ClearRecentItems();
			    var files = _documentService.GetRecentFiles();
			    foreach (string file in files)
			    {
			        AddRecentItem(file);
			    }
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error getting recent files", ex);
			}
		}

	    private IDockContent GetContentFromPersistString(string persistString)
        {
            if (persistString == typeof(OutputWindow).ToString())
            {
                return _dockingService.OutputWindow;
            }
            if (persistString == typeof(LabelList).ToString())
            {
                return _dockingService.LabelList;
            }
            if (persistString == typeof(ErrorList).ToString())
            {
                return _dockingService.ErrorList;
            }
            if (persistString == typeof(DebugPanel).ToString())
            {
                return _dockingService.DebugPanel;
            }
            if (persistString == typeof(CallStack).ToString())
            {
                return _dockingService.CallStack;
            }
            if (persistString == typeof(TrackingWindow).ToString())
            {
                return _dockingService.TrackWindow;
            }
            if (persistString == typeof(ProjectViewer).ToString())
            {
                return _dockingService.ProjectViewer;
            }

            string[] parsedStrings = persistString.Split(';');
	        string type = parsedStrings[0];
            if (parsedStrings.Length != 6 || type != typeof(Editor).ToString())
            {
                return null;
            }

            string fileName = parsedStrings[1];
            if (!File.Exists(fileName))
            {
                return null;
            }

            int horzVal = int.Parse(parsedStrings[2]);
	        int vertValue = int.Parse(parsedStrings[3]);
	        int line = int.Parse(parsedStrings[4]);
	        int column = int.Parse(parsedStrings[5]);

            Editor doc = _documentService.OpenDocument(fileName);
            doc.SetPosition(horzVal, vertValue, line, column);
            return doc;
        }

        void Editor_OnEditorToolTipRequested(object sender, EditorToolTipRequestEventArgs e)
        {
            if (_debugger == null)
            {
                return;
            }

            ushort? regValue =_debugger.GetRegisterValue(e.WordHovered);
            if (!regValue.HasValue)
            {
                return;
            }

            e.Tooltip = "$" + regValue.Value.ToString("X");
        }

        private void projectWatcher_Changed(object sender, FileSystemEventArgs e)
		{
			switch (e.ChangeType)
			{
				case WatcherChangeTypes.Changed:
					if (!string.IsNullOrEmpty(Path.GetExtension(e.FullPath)))
					{
					    string path = e.FullPath;
						foreach (var tempDoc in _dockingService.Documents
							.Where(doc => FileOperations.CompareFilePath(doc.FileName, path)))
						{
						    Editor doc = tempDoc;
						    this.Invoke(() =>
                            {
                                const string modifiedFormat = "{0} modified outside the editor.\nLoad changes?";
                                DialogResult result = MessageBox.Show(string.Format(modifiedFormat, e.FullPath),
                                    "File modified", MessageBoxButtons.YesNo);
                                if (result == DialogResult.Yes)
                                {
                                    _documentService.OpenDocument(doc, e.FullPath);
                                }
                            });
							break;
						}
					}

					break;
			}
		}

		private void projectWatcher_Renamed(object sender, RenamedEventArgs e)
		{
		    if (e.OldFullPath != _projectService.Project.ProjectDirectory)
		    {
		        return;
		    }

		    if (MessageBox.Show("Project Folder was renamed, would you like to rename the project?",
		            "Rename project",
		            MessageBoxButtons.YesNo,
		            MessageBoxIcon.Information) == DialogResult.Yes)
		    {
		        _projectService.Project.ProjectName = Path.GetFileNameWithoutExtension(e.FullPath);
		    }
		}

	    private void UpdateChecks()
		{
			if (IsDisposed || Disposing)
			{
				return;
			}

		    mainToolMenuItem.Checked = mainToolBar.Visible;
		    debugToolMenuItem.Checked = mainToolBar.Visible;
			labelListMenuItem.Checked = _dockingService.LabelList.Visible;
			projViewMenuItem.Checked = _dockingService.ProjectViewer.Visible;
			findResultsMenuItem.Checked = _dockingService.FindResults.Visible;

			// output stuff
			outWinMenuItem.Checked = _dockingService.OutputWindow.Visible;
			errListMenuItem.Checked = _dockingService.ErrorList.Visible;

			// debug stuff
			breakManagerMenuItem.Checked = _dockingService.BreakManagerWindow.Visible;
			debugPanelMenuItem.Checked = _dockingService.DebugPanel.Visible;
			callStackMenuItem.Checked = _dockingService.CallStack.Visible;
			stackViewerMenuItem.Checked = _dockingService.StackViewer.Visible;
			varTrackMenuItem.Checked = _dockingService.TrackWindow.Visible;

			// misc stuff
			statusBarMenuItem.Checked = statusBar.Visible;
			lineNumMenuItem.Checked = Settings.Default.lineNumbers;
			iconBarMenuItem.Checked = Settings.Default.iconBar;
		}

		private void GetCodeInfo(object sender, EditorSelectionEventArgs e)
		{
			if (string.IsNullOrEmpty(e.SelectedLines))
			{
				return;
			}

			lock (_codeInfoLock)
			{
				CodeCountInfo info = _assemblerService.CountCode(e.SelectedLines);
				this.Invoke(() => UpdateCodeInfo(info));
			}
		}

		/// <summary>
		/// Updates the code info with the latest size, min, and max string in the status bar.
		/// </summary>
		/// <param name="info">Code count information containing min runtime, max runtime and size</param>
		private void UpdateCodeInfo(CodeCountInfo info)
		{
			lineCodeInfo.Text = string.Format("Min: {0} Max: {1} Size: {2}", info.Min, info.Max, info.Size);
		}

		private void UpdateConfigToolbarBox()
		{
			IProject project = _projectService.Project;
			if (project.IsInternal)
			{
				return;
			}

			foreach (var config in _projectService.Project.BuildSystem.BuildConfigs)
			{
				configBox.Items.Add(config);
			}

			configBox.SelectedIndex = _projectService.Project.BuildSystem.CurrentConfigIndex;
		}

		/// <summary>
		/// Updates the title of the app with the filename.
		/// </summary>
		private void UpdateTitle()
		{
			string debugString = string.Empty;
			if (_debugger != null)
			{
				debugString = " (Debugging)";
			}
			if (!string.IsNullOrEmpty(_documentService.ActiveFileName))
			{
				Text = Path.GetFileName(_documentService.ActiveFileName) + debugString + " - Wabbitcode";
			}
			else
			{
				Text = "Wabbitcode" + debugString;
			}
		}

		private void AddRecentItem(string file)
		{
			ToolStripMenuItem button = new ToolStripMenuItem(file, null, OpenRecentDoc);
			recentFilesMenuItem.DropDownItems.Add(button);
		}

	    private void ClearRecentItems()
		{
			recentFilesMenuItem.DropDownItems.Clear();
		}

	    private void SetLineAndColStatus(object sender, EditorSelectionEventArgs e)
		{
			lineStatusLabel.Text = "Ln: " + e.Caret.Line;
			colStatusLabel.Text = "Col: " + e.Caret.Column;
		}

	    private void SetToolStripText(string text)
		{
			toolStripStatusLabel.Text = text;
		}

	    private static string GetOutputFileDetails(IProject project)
        {
            if (string.IsNullOrEmpty(project.BuildSystem.ProjectOutput))
            {
                throw new DebuggingException("No project outputs detected");
            }

            string createdName = project.BuildSystem.ProjectOutput;
            if (!Path.IsPathRooted(createdName))
            {
                createdName = FileOperations.NormalizePath(Path.Combine(project.ProjectDirectory, createdName));
            }

            if (string.IsNullOrEmpty(project.BuildSystem.ListOutput))
            {
                throw new DebuggingException("Missing list file");
            }

            string listName = project.BuildSystem.ListOutput;
            if (string.IsNullOrEmpty(project.BuildSystem.LabelOutput))
            {
                throw new DebuggingException("Missing label file");
            }

            string symName = project.BuildSystem.LabelOutput;

            StreamReader listReader = null;
            try
            {
                listReader = new StreamReader(listName);
                listReader.ReadToEnd();
            }
            catch (Exception)
            {
                throw new DebuggingException("Error reading list file");
            }
            finally
            {
                if (listReader != null)
                {
                    listReader.Dispose();
                }
            }

            StreamReader symReader = null;
            try
            {
                symReader = new StreamReader(symName);
                symReader.ReadToEnd();

            }
            catch (Exception)
            {
                throw new DebuggingException("Error reading label file");
            }
            finally
            {
                if (symReader != null)
                {
                    symReader.Dispose();
                }
            }

            return createdName;
        }

	    private void LockOpenEditors()
	    {
	        foreach (var document in _dockingService.Documents)
	        {
	            document.ReadOnly = true;
	        }
	    }

		private static void SaveRomPathRegistry(string romFileName)
		{
			RegistryKey romKey = null;
			try
			{
				romKey = Registry.CurrentUser.OpenSubKey("Software\\Wabbitemu", true);
				if (romKey != null)
				{
					romKey.SetValue("rom_path", romFileName);
				}
			}
			finally
			{
				if (romKey != null)
				{
					romKey.Close();
				}
			}
		}

        void Editor_OnEditorOpened(object sender, EditorEventArgs e)
        {
            if (_debugger != null)
            {
                e.Editor.ReadOnly = true;
            }

            string foldings;
            if (_foldingDictionary.TryGetValue(e.Editor.FileName.ToLower(), out foldings))
            {
                e.Editor.DocumentFoldings = foldings;
            }
        }

        void Editor_OnEditorClosing(object sender, EditorEventArgs e)
        {
            if (string.IsNullOrEmpty(e.Editor.FileName))
            {
                return;
            }

            string fileName = e.Editor.FileName.ToLower();
            string foldings = e.Editor.DocumentFoldings;
            if (_foldingDictionary.ContainsKey(fileName))
            {
                _foldingDictionary[fileName] = foldings;
            }
            else
            {
                _foldingDictionary.Add(fileName, foldings);
            }
        }

		private void WabbitcodeBreakpointManager_OnBreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e)
		{
			if (_debugger != null)
			{
				_debugger.ClearBreakpoint(e.Breakpoint);
			}
		}

		void WabbitcodeBreakpointManager_OnBreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e)
		{
			if (_debugger != null)
			{
				e.Cancel = _debugger.SetBreakpoint(e.Breakpoint);
			}
		}

		private void UpdateBreakpoints()
		{
			var breakpoints = WabbitcodeBreakpointManager.Breakpoints.ToList();
			foreach (WabbitcodeBreakpoint breakpoint in breakpoints)
			{
				_debugger.SetBreakpoint(breakpoint);
			}
		}

		/// <summary>
		/// Updates all the menu items that depend on if there is an active child open.
		/// </summary>
		/// <param name="enabled">Whether items should be enabled or disabled.</param>
		private void UpdateMenus(bool enabled)
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
				UpdateDebugStuff();
			}

			toggleBreakpointMenuItem.Enabled = enabled;

			// Window Menu
			windowMenuItem.Enabled = enabled;
		}

		private void UpdateProjectMenu(bool projectOpen)
		{
			projMenuItem.Visible = projectOpen;
			includeDirButton.Visible = !projectOpen;
			saveProjectMenuItem.Visible = projectOpen;
		}

	    private void DoAssembly(AssemblerService.OnFinishAssemblyFile fileEventHandler, AssemblerService.OnFinishAssemblyProject projectEventHandler)
		{
			_dockingService.OutputWindow.ClearOutput();
			if (!_projectService.Project.IsInternal)
			{
				if (projectEventHandler != null)
				{
					_assemblerService.AssemblerProjectFinished += projectEventHandler;
				}
				Task.Factory.StartNew(() =>
				{
					_assemblerService.AssembleProject(_projectService.Project);
					if (projectEventHandler != null)
					{
						_assemblerService.AssemblerProjectFinished -= projectEventHandler;
					}
				});
			}
			else if (_dockingService.ActiveDocument != null)
			{
				bool saved = _dockingService.ActiveDocument.SaveFile();
				string inputFile = _dockingService.ActiveDocument.FileName;
				if (!saved)
				{
					return;
				}

				if (fileEventHandler != null)
				{
					_assemblerService.AssemblerFileFinished += fileEventHandler;
				}

				Task.Factory.StartNew(() =>
				{
					string outputFile = Path.ChangeExtension(inputFile, _assemblerService.GetExtension(Settings.Default.outputFile));
					string originalDir = Path.GetDirectoryName(inputFile);
					_assemblerService.AssembleFile(inputFile, outputFile, originalDir, Settings.Default.includeDirs.Cast<string>());
					if (fileEventHandler != null)
					{
						_assemblerService.AssemblerFileFinished -= fileEventHandler;
					}
				});
			}
		}

		private void configBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			_projectService.Project.BuildSystem.CurrentConfigIndex = configBox.SelectedIndex;
		}

		private void DockingService_OnActiveDocumentChanged(object sender, EventArgs eventArgs)
		{
			if (Disposing)
			{
				return;
			}

            UpdateMenus(ActiveMdiChild != null);
			UpdateTitle();
		}

        #region Initalization

        private void InitializeEvents()
        {
            Editor.OnEditorSelectionChanged += GetCodeInfo;
            Editor.OnEditorSelectionChanged += UpdateAssembledInfo;
            Editor.OnEditorSelectionChanged += SetLineAndColStatus;
            Editor.OnEditorDragDrop += MainFormRedone_DragDrop;
            Editor.OnEditorDragEnter += MainFormRedone_DragEnter;
        }

        private void InitializeService()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>(dockPanel);
            _assemblerService = ServiceFactory.Instance.GetServiceInstance<IAssemblerService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
            _symbolService = ServiceFactory.Instance.GetServiceInstance<ISymbolService>();
            _backgroundAssemblerService = ServiceFactory.Instance.GetServiceInstance<IBackgroundAssemblerService>();
            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            _fileReaderService = ServiceFactory.Instance.GetServiceInstance<IFileReaderService>();
        }

        private void InitiailzeToolbars()
        {
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
        }

        private void HandleArgs(string[] args)
        {
            if (args.Length == 0)
            {
                return;
            }
            foreach (string arg in args)
            {
                try
                {
                    if (string.IsNullOrEmpty(arg))
                    {
                        break;
                    }
                    _documentService.OpenDocument(arg);
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

        public void ProcessParameters(string[] args)
        {
            // The form has loaded, and initialization will have been be done.
            HandleArgs(args);
            Activate();
        }

        private void RestoreWindow()
        {
            try
            {
                WindowState = Settings.Default.WindowState;
                Size = Settings.Default.WindowSize;
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error restoring the window size", ex);
            }
        }

        #endregion

        #region Project Handling

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
                    valid = OpenProject(Settings.Default.startupProject);
                }
                else
                {
                    Settings.Default.startupProject = string.Empty;
                    DockingService.ShowError("Error: Project file not found");
                }

                if (_projectService.Project.IsInternal || !valid)
                {
                    CreateInternalProject();
                }
            }
            catch (Exception ex)
            {
                CreateInternalProject();
                var result = MessageBox.Show(
                                 "There was an error loading the startup project, would you like to remove it?\n" + ex,
                                 "Error",
                                 MessageBoxButtons.YesNo,
                                 MessageBoxIcon.Error);
                if (result == DialogResult.Yes)
                {
                    Settings.Default.startupProject = string.Empty;
                }
            }
        }

        private void CreateInternalProject()
		{
			_projectService.CreateInternalProject();
			UpdateProjectMenu(false);
			_dockingService.ProjectViewer.BuildProjTree();
		}

		private bool OpenProject(string fileName)
		{
			bool valid = _projectService.OpenProject(fileName);
			UpdateProjectMenu(true);
			UpdateMenus(_dockingService.Documents.Any());
			_dockingService.ProjectViewer.BuildProjTree();
			return valid;
		}

        private void CloseProject()
        {
            DialogResult result = DialogResult.No;
            if (_projectService.Project.NeedsSave && !Settings.Default.autoSaveProject)
            {
                result = MessageBox.Show("Would you like to save your changes to the project file?", "Save project?", MessageBoxButtons.YesNo, MessageBoxIcon.None);
            }
            if (result == DialogResult.Yes || Settings.Default.autoSaveProject)
            {
                _projectService.SaveProject();
            }

            _projectService.CloseProject();
            _dockingService.ProjectViewer.CloseProject();
            UpdateProjectMenu(false);
        }

        #endregion

        #region Document Handling

        private void OpenDocument()
        {
            var openFileDialog = new OpenFileDialog
            {
                CheckFileExists = true,
                DefaultExt = "*.asm",
                Filter = "All Know File Types | *.asm; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
                         " Assembly Files (*.z80)|*.z80 | Include Files (*.inc)|*.inc | Project Files (*.wcodeproj)" +
                         "|*.wcodeproj|All Files(*.*)|*.*",
                FilterIndex = 0,
                RestoreDirectory = true,
                Multiselect = true,
                Title = "Open File",
            };

            if (openFileDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            try
            {
                foreach (var fileName in openFileDialog.FileNames)
                {
                    string extCheck = Path.GetExtension(fileName);
                    if (string.Equals(extCheck, ".wcodeproj", StringComparison.OrdinalIgnoreCase))
                    {
                        OpenProject(fileName);
                        if (Settings.Default.startupProject == fileName)
                        {
                            continue;
                        }

                        if (MessageBox.Show("Would you like to make this your default project?",
                                "Startup Project",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Question) == DialogResult.Yes)
                        {
                            Settings.Default.startupProject = fileName;
                        }
                    }
                    else
                    {
                        _documentService.OpenDocument(fileName);
                    }
                }
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error opening file", ex);
            }
        }

        /// <summary>
        /// This opens the recend document clicked in the file menu.
        /// </summary>
        /// <param name="sender">This is the button object. This is casted to get which button was clicked.</param>
        /// <param name="e">Nobody cares about this arg.</param>
        private void OpenRecentDoc(object sender, EventArgs e)
        {
            MenuItem button = (MenuItem)sender;
            _documentService.OpenDocument(button.Text);
        }

        private void SaveAllDocuments()
        {
            foreach (Editor child in MdiChildren)
            {
                child.SaveFile();
            }

            UpdateTitle();
        }

        #endregion

        #region Document Actions

        private void Cut()
        {
            var toolWindow = _dockingService.ActiveContent as ToolWindow;
            if (toolWindow != null)
            {
                toolWindow.Cut();
            }
            else if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Cut();
            }
        }

        private void Copy()
        {
            var activeContent = _dockingService.ActiveContent as ToolWindow;
            if (activeContent != null)
            {
                activeContent.Copy();
            }
            else if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Copy();
            }
        }

        private void Paste()
        {
            var activeContent = _dockingService.ActiveContent as ToolWindow;
            if (activeContent != null)
            {
                activeContent.Paste();
            }
            else if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Paste();
            }
        }

        #endregion

        #region Debugging

        private void StartDebug()
        {
            if (_projectService.Project.IsInternal)
            {
                throw new DebuggingException("Debugging single files is not supported");
            }

            DoAssembly(null, (sender, e) =>
            {
                if (!e.AssemblySucceeded)
                {
                    if (DockingService.ShowMessageBox(this,
                        "There were errors assembling. Would you like to continue and try to debug?",
                        "Continue",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Error) != DialogResult.No)
                    {
                        return;
                    }

                    CancelDebug();
                    return;
                }

                this.Invoke(() =>
                {
                    ShowErrorPanels(e.Output);
                    LockOpenEditors();
                });

                _debugger = new WabbitcodeDebugger(_documentService, _fileReaderService, _symbolService);
                _debugger.OnDebuggerStep += debugger_OnDebuggerStep;
                _debugger.OnDebuggerRunningChanged += debugger_OnDebuggerRunningChanged;
                WabbitcodeDebugger.OnDebuggingEnded += (o, args) => CancelDebug();

                string createdName;
                try
                {
                    createdName = GetOutputFileDetails(e.Project);
                }
                catch (DebuggingException ex)
                {
                    DockingService.ShowError("Unable to start debugging", ex);
                    CancelDebug();
                    return;
                }

                try
                {
                    _debugger.InitDebugger(createdName);
                }
                catch (MissingROMException)
                {
                    this.Invoke(() =>
                    {
                        OpenFileDialog openFileDialog = new OpenFileDialog
                        {
                            CheckFileExists = true,
                            DefaultExt = "*.rom",
                            Filter = "All Know File Types | *.rom; *.sav; |ROM Files (*.rom)|*.rom|" +
                                     "Savestate Files (*.sav)|*.sav|All Files(*.*)|*.*",
                            FilterIndex = 0,
                            Multiselect = true,
                            RestoreDirectory = true,
                            Title = "Select new ROM file",
                        };

                        if (openFileDialog.ShowDialog() != DialogResult.OK)
                        {
                            CancelDebug();
                            return;
                        }

                        SaveRomPathRegistry(openFileDialog.FileName);
                    });
                    // TODO: fix to catch missing exception
                    _debugger.InitDebugger(createdName);
                }

                try
                {
                    _debugger.StartDebug();
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

                this.Invoke(() =>
                {
                    UpdateDebugStuff();
                    UpdateBreakpoints();
                    ShowDebugPanels();
                    EnableDebugPanels(false);
                });
            });
        }

        private void RestartDebug()
        {
            Cursor = Cursors.WaitCursor;
            _debuggingWorker = new BackgroundWorker();
            _debuggingWorker.DoWork += (sender, args) =>
            {
                if (_projectService.Project.IsInternal)
                {
                    throw new DebuggingException("Debugging single files is not supported");
                }

                try
                {
                    _debugger.RestartDebug();
                }
                catch (DebuggingException)
                {
                    if (DockingService.ShowMessageBox(this, "Unable to find the app, would you like to try and continue and debug?",
                        "Missing App",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Exclamation) != DialogResult.Yes)
                    {
                        CancelDebug();
                    }
                }
            };

            _debuggingWorker.RunWorkerCompleted += (sender, args) =>
            {
                UpdateDebugStuff();
                ShowDebugPanels();
                _documentService.RemoveDebugHighlight();
                Cursor = Cursors.Default;
            };
            _debuggingWorker.RunWorkerAsync();
        }

        private void CancelDebug()
        {
            if (InvokeRequired)
            {
                this.Invoke(CancelDebug);
                return;
            }

            _debugger = null;

            UpdateTitle();
            UpdateDebugStuff();
            HideDebugPanels();
            UpdateChecks();

            _documentService.RemoveDebugHighlight();
        }

        private void Step()
        {
            _debuggingWorker = new BackgroundWorker();
            _debuggingWorker.DoWork += (sender, args) => _debugger.Step();
            _debuggingWorker.RunWorkerCompleted += (sender, args) => UpdateDebugStuff();
            _debuggingWorker.RunWorkerAsync();
        }

        private void StepOver()
        {
            _debuggingWorker = new BackgroundWorker();
            _debuggingWorker.DoWork += (sender, args) => _debugger.StepOver();
            _debuggingWorker.RunWorkerCompleted += (sender, args) => UpdateDebugStuff();
            _debuggingWorker.RunWorkerAsync();
        }

        private void StepOut()
        {
            _debuggingWorker = new BackgroundWorker();
            _debuggingWorker.DoWork += (sender, args) => _debugger.StepOut();
            _debuggingWorker.RunWorkerCompleted += (sender, args) => UpdateDebugStuff();
            _debuggingWorker.RunWorkerAsync();
        }

        private void debugger_OnDebuggerStep(object sender, DebuggerStepEventArgs e)
        {
            this.Invoke(() =>
            {
                _documentService.RemoveDebugHighlight();
                _documentService.GotoLine(e.Location.FileName, e.Location.LineNumber);
                _documentService.HighlightDebugLine(e.Location.LineNumber);

                UpdateDebugStuff();
                UpdateDebugPanel();
                EnableDebugPanels(true);
            });
        }

        private void debugger_OnDebuggerRunningChanged(object sender, DebuggerRunningEventArgs e)
        {
            this.Invoke(() =>
            {
                _documentService.RemoveDebugHighlight();
                UpdateDebugStuff();
                if (e.Running)
                {
                    _dockingService.ActiveDocument.Refresh();
                    EnableDebugPanels(false);
                }
                else
                {
                    Activate();
                    _documentService.GotoLine(e.Location.FileName, e.Location.LineNumber);
                    _documentService.HighlightDebugLine(e.Location.LineNumber);

                    UpdateDebugPanel();
                    EnableDebugPanels(true);
                }
            });
        }

        private void EnableDebugPanels(bool enabled)
        {
            _dockingService.DebugPanel.EnablePanel(enabled);
            _dockingService.StackViewer.EnablePanel(enabled);
            _dockingService.TrackWindow.EnablePanel(enabled);
            _dockingService.CallStack.EnablePanel(enabled);
        }

        private void ShowDebugPanels()
        {
            _showToolbar = Settings.Default.debugToolbar;
            Settings.Default.debugToolbar = true;
            if (!_showToolbar)
            {
                debugToolStrip.Visible = true;
            }

            debugToolStrip.Height = mainToolBar.Height;
            UpdateChecks();
            _dockingService.ShowDockPanel(_dockingService.DebugPanel);
            _dockingService.ShowDockPanel(_dockingService.StackViewer);
            _dockingService.ShowDockPanel(_dockingService.ExpressionWindow, _dockingService.StackViewer, DockAlignment.Left);
            _dockingService.ShowDockPanel(_dockingService.CallStack, _dockingService.StackViewer);
            _dockingService.ShowDockPanel(_dockingService.TrackWindow, _dockingService.ExpressionWindow);
            UpdateTitle();
        }

        private void HideDebugPanels()
        {
            Settings.Default.debugToolbar = _showToolbar;
            if (!_showToolbar)
            {
                debugToolStrip.Visible = false;
            }

            if (_dockingService.DebugPanel != null)
            {
                _dockingService.HideDockPanel(_dockingService.DebugPanel);
            }

            if (_dockingService.TrackWindow != null)
            {
                _dockingService.HideDockPanel(_dockingService.TrackWindow);
            }

            if (_dockingService.CallStack != null)
            {
                _dockingService.HideDockPanel(_dockingService.CallStack);
            }

            if (_dockingService.StackViewer != null)
            {
                _dockingService.HideDockPanel(_dockingService.StackViewer);
            }

            if (_dockingService.ExpressionWindow != null)
            {
                _dockingService.HideDockPanel(_dockingService.ExpressionWindow);
            }
        }

        private void UpdateDebugStuff()
        {
            bool isDebugging = _debugger != null;
            bool isRunning = isDebugging && _debugger.IsRunning;
            bool enabled = isDebugging && !isRunning;
            bool hasCallStack = isDebugging && _debugger.CallStack.Count > 0;
            stepMenuItem.Enabled = enabled;
            gotoCurrentToolButton.Enabled = enabled;
            stepToolButton.Enabled = enabled;
            startDebugMenuItem.Enabled = enabled || !isDebugging;
            stepOverMenuItem.Enabled = enabled;
            stepOverToolButton.Enabled = enabled;
            stepOutMenuItem.Enabled = enabled && hasCallStack;
            stepOutToolButton.Enabled = enabled && hasCallStack;
            stopDebugMenuItem.Enabled = isDebugging;
            stopToolButton.Enabled = isDebugging;
            restartToolStripButton.Enabled = isDebugging;
            runMenuItem.Enabled = enabled;
            runDebuggerToolButton.Enabled = enabled || !isDebugging;
            runToolButton.Enabled = enabled || !isDebugging;
            pauseToolButton.Enabled = isRunning;
        }

        private void UpdateDebugPanel()
        {
            _dockingService.DebugPanel.UpdateFlags();
            _dockingService.DebugPanel.UpdateRegisters();
            _dockingService.DebugPanel.UpdateScreen();
        }

	    #endregion

        #region Form Events

        private void MainFormRedone_DragDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop) == false)
            {
                return;
            }

            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            foreach (string file in files)
            {
                _documentService.OpenDocument(file);
            }
        }

        private void MainFormRedone_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Copy : DragDropEffects.None;
        }

        private void MainFormRedone_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (_debugger != null)
			{
				CancelDebug();
			}

			if (!_projectService.Project.IsInternal)
			{
				CloseProject();
			}

			try
			{
				SaveWindow();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error saving window location", ex);
			}

			try
			{
				_dockingService.DestroyService();
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
				DockingService.ShowError("Error saving configuration file", ex);
			}
		}

        #endregion

        #region File Menu

        private void newFileMenuItem_Click(object sender, EventArgs e)
		{
			Editor doc = _documentService.CreateNewDocument();
			_dockingService.ShowDockPanel(doc);
		}

		private void newProjectMenuItem_Click(object sender, EventArgs e)
		{
			NewProjectDialog template = new NewProjectDialog(_dockingService, _documentService, _projectService);
			if (template.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			UpdateProjectMenu(true);
		}

        private void openFileMenuItem_Click(object sender, EventArgs e)
        {
            OpenDocument();
        }

        private void openProjectMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
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
                    string fileName = openFileDialog.FileName;

                    if (!OpenProject(fileName))
                    {
                        _projectService.CreateInternalProject();
                    }

                    if (Settings.Default.startupProject != fileName)
                    {
                        if (
                            MessageBox.Show("Would you like to make this your default project?",
                                            "Startup Project",
                                            MessageBoxButtons.YesNo,
                                            MessageBoxIcon.Question) == DialogResult.Yes)
                        {
                            Settings.Default.startupProject = fileName;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error opening file.", ex);
            }

            UpdateMenus(_dockingService.Documents.Any());
        }

        private void saveMenuItem_Click(object sender, EventArgs e)
        {
            SaveActiveDocument();
        }

        private void saveAsMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                _dockingService.ActiveDocument.SaveFileAs();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error saving file.", ex);
            }

            UpdateTitle();
        }

        private void saveAllMenuItem_Click(object sender, EventArgs e)
        {
            SaveAllDocuments();
        }

        private void saveProjectMenuItem_Click(object sender, EventArgs e)
        {
            _projectService.SaveProject();
            saveProjectMenuItem.Enabled = _projectService.Project.NeedsSave;
        }

        private void closeMenuItem_Click(object sender, EventArgs e)
        {
            if (ActiveMdiChild != null)
            {
                ActiveMdiChild.Close();
            }
        }

        private void exitMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        #endregion

        #region Edit Menu

        private void undoMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Undo();
            }
        }

        private void redoMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Redo();
            }
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

        private void selectAllMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _documentService.ActiveDocument.SelectAll();
        }

        private void findMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.ShowFindForm(this, SearchMode.Find);
        }

        private void findInFilesMenuItem_Click(object sender, EventArgs e)
        {
            _dockingService.FindForm.ShowFor(this, null, SearchMode.FindInFiles);
        }

        private void replaceMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.ShowFindForm(this, SearchMode.Replace);
        }

        private void replaceInFilesMenuItem_Click(object sender, EventArgs e)
        {
            _dockingService.FindForm.ShowFor(this, null, SearchMode.FindInFiles);
        }

        private void findAllRefsMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            string word = _dockingService.ActiveDocument.GetWord();
            _dockingService.FindResults.NewFindResults(word, _projectService.Project.ProjectName);
            var refs = _projectService.FindAllReferences(word);
            foreach (var fileRef in refs.SelectMany(reference => reference))
            {
                _dockingService.FindResults.AddFindResult(fileRef);
            }

            _dockingService.FindResults.DoneSearching();
            _dockingService.ShowDockPanel(_dockingService.FindResults);
        }

        private void makeUpperMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.SelectedTextToUpper();
        }

        private void makeLowerMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.SelectedTextToLower();
        }

        private void invertCaseMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.SelectedTextInvertCase();
        }

        private void sentenceCaseMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _documentService.ActiveDocument.SelectedTextToSentenceCase();
        }

        private void formatDocMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.FormatLines();
        }

        private void convertSpacesToTabsMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.ConvertSpacesToTabs();
        }

        private void prevBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (_documentService.ActiveDocument == null)
            {
                return;
            }

            _documentService.ActiveDocument.GotoPrevBookmark();
        }

        private void nextBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (_documentService.ActiveDocument == null)
            {
                return;
            }

            _documentService.ActiveDocument.GotoNextBookmark();
        }

        private void toggleBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            if (_documentService.ActiveDocument == null)
            {
                return;
            }

            _documentService.ActiveDocument.ToggleBookmark();
        }

        private void gLineMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            GotoLine gotoBox = new GotoLine(_dockingService.ActiveDocument.TotalNumberOfLines);
            DialogResult gotoResult = gotoBox.ShowDialog();
            if (gotoResult != DialogResult.OK)
            {
                return;
            }

            int line = Convert.ToInt32(gotoBox.inputBox.Text);
            _documentService.GotoLine(_dockingService.ActiveDocument, line);
        }

        private void gLabelMenuItem_Click(object sender, EventArgs e)
        {
            GotoSymbol gotoSymbolBox = new GotoSymbol(_parserService);
            if (gotoSymbolBox.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            string symbolString = gotoSymbolBox.inputBox.Text;
            IParserData data = _parserService.GetParserData(symbolString, Settings.Default.caseSensitive).FirstOrDefault();
            if (data == null)
            {
                return;
            }

            IIncludeFile includeFile = data as IIncludeFile;
            if (includeFile != null)
            {
                string fullPath = Path.IsPathRooted(includeFile.IncludedFile) ?
                    includeFile.IncludedFile :
                    FileOperations.NormalizePath(
                        _projectService.Project.GetFilePathFromRelativePath(includeFile.IncludedFile)
                    );
                _documentService.GotoFile(fullPath);
            }
            else
            {
                _documentService.GotoLabel(data);
            }
        }

        private void prefsMenuItem_Click(object sender, EventArgs e)
        {
            Preferences prefs = new Preferences(_dockingService);
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
            ToolStripMenuItem item = (ToolStripMenuItem)sender;
            item.Checked = !item.Checked;
            switch (item.Tag.ToString())
            {
                case "iconBar":
                    if (ActiveMdiChild != null)
                    {
                        _documentService.ActiveDocument.IsIconBarVisible = item.Checked;
                    }

                    break;
                case "lineNumbers":
                    if (ActiveMdiChild != null)
                    {
                        _documentService.ActiveDocument.ShowLineNumbers = item.Checked;
                    }

                    break;
                case "labelsList":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.LabelList);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.LabelList);
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
                        _dockingService.ShowDockPanel(_dockingService.OutputWindow);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.OutputWindow);
                    }

                    break;
                case "FindResults":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.FindResults);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.FindResults);
                    }

                    break;
                case "statusBar":
                    statusBar.Visible = item.Checked;
                    break;
                case "debugPanel":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.DebugPanel);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.DebugPanel);
                    }

                    break;
                case "callStack":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.CallStack);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.CallStack);
                    }

                    break;
                case "stackViewer":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.StackViewer);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.StackViewer);
                    }

                    break;
                case "varTrack":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.TrackWindow);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.TrackWindow);
                    }

                    break;
                case "breakManager":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.BreakManagerWindow);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.BreakManagerWindow);
                    }

                    break;
                case "projectViewer":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.ProjectViewer);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.ProjectViewer);
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
                        _dockingService.ShowDockPanel(_dockingService.ErrorList);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.ErrorList);
                    }

                    break;
                case "macroManager":
                    if (item.Checked)
                    {
                        _dockingService.ShowDockPanel(_dockingService.MacroManager);
                    }
                    else
                    {
                        _dockingService.HideDockPanel(_dockingService.MacroManager);
                    }

                    break;
            }

            debugToolStrip.Height = 25;
        }

        #endregion

        #region Refactor Menu

        private void renameMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }
            ShowRefactorForm();
        }

        private void extractMethodMenuItem_Click(object sender, EventArgs e)
        {
        }

        #endregion

        #region Project Menu

        private void addNewFileMenuItem_Click(object sender, EventArgs e)
        {
            RenameForm newNameForm = new RenameForm
            {
                Text = "New File"
            };
            var result = newNameForm.ShowDialog() != DialogResult.OK;
            newNameForm.Dispose();
            if (result)
            {
                return;
            }

            string name = newNameForm.NewText;
            _dockingService.ProjectViewer.AddNewFile(name);
        }

        private void existingFileMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
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
                _dockingService.ProjectViewer.AddExistingFile(file);
            }
        }

        private void buildOrderButton_Click(object sender, EventArgs e)
        {
            using (BuildSteps build = new BuildSteps(_projectService.Project))
            {
                build.ShowDialog();
            }
        }

        private void includeDirButton_Click(object sender, EventArgs e)
        {
            IncludeDir includes = new IncludeDir(_projectService.Project);
            includes.ShowDialog();
            includes.Dispose();
        }

        private void refreshViewMenuItem_Click(object sender, EventArgs e)
        {
            _dockingService.ProjectViewer.BuildProjTree();
        }

        private void closeProjMenuItem_Click(object sender, EventArgs e)
        {
            CloseProject();
        }

        #endregion

        #region Assemble Menu

        private void assembleMenuItem_Click(object sender, EventArgs e)
        {
            if (_debugger != null)
            {
                if (MessageBox.Show("Do you want to stop debugging?", "Debugging", MessageBoxButtons.YesNo) == DialogResult.No)
                {
                    return;
                }

                CancelDebug();
            }

            DoAssembly(OnAssemblyFinished, OnAssemblyFinished);
        }

        private void symTableMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.SaveFile();
            string inputFile = _documentService.ActiveFileName;
            string outputFile = Path.ChangeExtension(_documentService.ActiveFileName, "lab");
            string originalDir = Path.GetDirectoryName(inputFile);
            var includeDirs = Settings.Default.includeDirs.Cast<string>();
            _assemblerService.AssembleFile(inputFile, outputFile, originalDir,
                includeDirs, AssemblyFlags.Normal | AssemblyFlags.Symtable);
        }

        private void listFileMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.SaveFile();
            string inputFile = _dockingService.ActiveDocument.FileName;
            string outputFile = Path.ChangeExtension(inputFile, "lst");
            string originalDir = Path.GetDirectoryName(inputFile);
            var includeDirs = Settings.Default.includeDirs.Cast<string>();
            _assemblerService.AssembleFile(inputFile, outputFile, originalDir, includeDirs, AssemblyFlags.List | AssemblyFlags.Normal);
        }

        private void projStatsMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.SaveFile();
            string inputFile = _dockingService.ActiveDocument.FileName;
            string outputFile = string.Empty;
            string originalDir = Path.GetDirectoryName(inputFile);
            var includeDirs = Settings.Default.includeDirs.Cast<string>();
            _assemblerService.AssembleFile(inputFile, outputFile, originalDir, includeDirs, AssemblyFlags.Stats);
        }

        #endregion

        #region Debug Menu

        private void startDebugMenuItem_Click(object sender, EventArgs e)
        {
            if (_debugger == null)
            {
                StartDebug();
            }
            else
            {
                _debugger.Run();
            }
        }

        private void startWithoutDebugMenuItem_Click(object sender, EventArgs e)
        {
            // TODO: fix this
            //_debugger.StartWithoutDebug();
        }

        private void stopDebugMenuItem_Click(object sender, EventArgs e)
        {
            _debugger.CancelDebug();
        }

        private void runMenuItem_Click(object sender, EventArgs e)
        {
            if (_debugger != null)
            {
                _debugger.Run();
            }
        }

        private void stepMenuItem_Click(object sender, EventArgs e)
        {
            Step();
        }

	    private void stepOverMenuItem_Click(object sender, EventArgs e)
	    {
	        StepOver();
	    }

        private void stepOutMenuItem_Click(object sender, EventArgs e)
        {
            StepOut();
        }

        private void newBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            NewBreakpointForm form = new NewBreakpointForm(_dockingService, _documentService);
            form.ShowDialog();
            form.Dispose();
        }

        private void toggleBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            _dockingService.ActiveDocument.ToggleBreakpoint();
            _dockingService.ActiveDocument.Refresh();
        }

        #endregion

        #region Help Menu

        private void updateMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (UpdateService.CheckForUpdate())
                {
                    var result = MessageBox.Show("New version available. Download now?",
                        "Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
                    if (result == DialogResult.Yes)
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

        private void aboutMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox box = new AboutBox();
            box.ShowDialog();
            box.Dispose();
        }

        #endregion

        #region Main Toolbar

        private void newToolButton_Click(object sender, EventArgs e)
		{
			Editor doc = _documentService.CreateNewDocument();
			doc.TabText = "New Document";
			doc.Show(dockPanel);
		}

        private void openToolButton_Click(object sender, EventArgs e)
        {
            OpenDocument();
        }

        private void saveToolButton_Click(object sender, EventArgs e)
        {
            SaveActiveDocument();
        }

        private void saveAllToolButton_Click(object sender, EventArgs e)
        {
            SaveAllDocuments();
        }

        private void cutToolButton_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Cut();
            }
        }

        private void copyToolButton_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Copy();
            }
        }

        private void pasteToolButton_Click(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument != null)
            {
                _dockingService.ActiveDocument.Paste();
            }
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

            bool found = _documentService.ActiveDocument.Find(findBox.Text);
            if (!found)
            {
                MessageBox.Show("Text not found");
            }
        }

        #endregion

        #region Debug Toolbar

        private void pauseToolButton_Click(object sender, EventArgs e)
        {
            try
            {
                _debugger.Pause();
            }
            catch (DebuggingException)
            {
                MessageBox.Show("Unable to pause at this point.");
            }
        }

        private void stopDebugToolButton_Click(object sender, EventArgs e)
        {
            _debugger.CancelDebug();
        }

        private void restartToolStripButton_Click(object sender, EventArgs e)
        {
            RestartDebug();
        }

        private void stepToolButton_Click(object sender, EventArgs e)
        {
            Step();
        }

        private void stepOverToolButton_Click(object sender, EventArgs e)
        {
            StepOver();
        }

        private void stepOutToolButton_Click(object sender, EventArgs e)
        {
            StepOut();
        }

        private void gotoCurrentToolButton_Click(object sender, EventArgs e)
        {
            _documentService.GotoCurrentDebugLine();
        }

        #endregion

        private void OnAssemblyFinished(object sender, AssemblyFinishEventArgs e)
		{
			_assemblerService.AssemblerProjectFinished -= OnAssemblyFinished;

			this.Invoke(() => ShowErrorPanels(e.Output));
		}

		private void SaveActiveDocument()
		{
			try
			{
				_dockingService.ActiveDocument.SaveFile();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error saving file.", ex);
			}

            UpdateTitle();
		}

		private void SaveWindow()
		{
			Settings.Default.WindowSize = WindowState != FormWindowState.Normal ? new Size(RestoreBounds.Width, RestoreBounds.Height) : Size;
			Settings.Default.WindowState = WindowState;
		}

		private void ShowErrorPanels(AssemblerOutput output)
		{
			try
			{
				_dockingService.OutputWindow.ClearOutput();
				_dockingService.OutputWindow.AddText(output.OutputText);
				_dockingService.OutputWindow.HighlightOutput();

				// its more fun with colors
				_dockingService.ErrorList.ParseOutput(output.ParsedErrors);
				_dockingService.ShowDockPanel(_dockingService.ErrorList);
				_dockingService.ShowDockPanel(_dockingService.OutputWindow);
				if (_dockingService.ActiveDocument != null)
				{
					_dockingService.ActiveDocument.Refresh();
				}

				foreach (Editor child in _dockingService.Documents)
				{
					child.UpdateIcons(output.ParsedErrors);
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
		}

        private void UpdateAssembledInfo(object sender, EditorSelectionEventArgs e)
		{
			if (_debugger == null)
			{
				return;
			}

            CalcLocation label = _symbolService.ListTable.GetCalcLocation(e.Editor.FileName, e.Caret.Line);
			if (label == null)
			{
				return;
			}

			string assembledInfo = string.Format("Page: {0} Address: {1}", label.Page, label.Address.ToString("X4"));
			SetToolStripText(assembledInfo);
		}

	    private void ShowRefactorForm()
		{
			RefactorForm form = new RefactorForm(_dockingService, _projectService);
			form.ShowDialog();
		}
	}
}