using System.Collections.Specialized;
using System.ComponentModel;
using Microsoft.Win32;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.DockingWindows;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using IFileReaderService = Revsoft.Wabbitcode.Services.Interfaces.IFileReaderService;

namespace Revsoft.Wabbitcode
{
	public partial class MainForm : Form
	{
		#region Private Members

	    private bool _showToolbar = true;
	    private IWabbitcodeDebugger _debugger;
        private BackgroundWorker _debuggingWorker;

		#region Services

        private IAssemblerService _assemblerService;
		private IDockingService _dockingService;
        private IDocumentService _documentService;
        private IFileReaderService _fileReaderService;
		private IProjectService _projectService;
		private IParserService _parserService;
		private ISymbolService _symbolService;
	    private IStatusBarService _statusBarService;
	    private IToolBarService _toolBarService;

	    #endregion

		#endregion

		#region Events

		#endregion

		public MainForm(string[] args)
		{
			InitializeComponent();
			RestoreWindow();
			InitializeService();
			InitializeEvents();
            InitiailzeToolbars();
		
			WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
			WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnBreakpointRemoved;

            /*Editor.OnEditorOpened += Editor_OnEditorOpened;
            Editor.OnEditorClosing += Editor_OnEditorClosing;
            Editor.OnEditorToolTipRequested += Editor_OnEditorToolTipRequested;*/
		    _dockingService.InitPanels();
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
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error building project tree", ex);
			}

			HandleArgs(args);
			UpdateMenus(_dockingService.ActiveDocument != null);

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

        private void ToolBarService_OnToolBarVisibilityChanged(object sender, ToolbarVisibilityChangedEventArgs e)
        {
            var menuItem = toolBarMenuItem.DropDownItems.Cast<ToolStripMenuItem>()
                .FirstOrDefault(i => i.Tag != null &&  i.Tag.ToString() == e.ToolBarName);
            if (menuItem != null)
            {
                menuItem.Checked = e.Visible;
            }
        }

	    private void ProjectService_OnProjectClosed(object sender, EventArgs eventArgs)
	    {
            UpdateProjectMenu(false);
	    }

	    private void ProjectService_OnProjectOpened(object sender, EventArgs eventArgs)
	    {
            UpdateProjectMenu(true);
            UpdateMenus(_dockingService.Documents.Any());
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
            if (parsedStrings.Length != 4 || type != typeof(Editor).ToString())
            {
                return null;
            }

            string fileName = parsedStrings[1];
            if (!File.Exists(fileName))
            {
                return null;
            }

	        int line = int.Parse(parsedStrings[4]);
	        int column = int.Parse(parsedStrings[5]);

            Editor doc = _documentService.OpenDocument(fileName);
	        doc.CaretLine = line;
	        doc.CaretColumn = column;
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
						foreach (var tempDoc in _dockingService.Documents.OfType<Editor>()
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

		    debugToolMenuItem.Checked = debugToolStrip.Visible;
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
			lineNumMenuItem.Checked = Settings.Default.LineNumbers;
			iconBarMenuItem.Checked = Settings.Default.IconBar;
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

		    var activeFileEditor = _dockingService.ActiveDocument as AbstractFileEditor;
			if (activeFileEditor != null && !string.IsNullOrEmpty(activeFileEditor.FileName))
			{
				Text = Path.GetFileName(activeFileEditor.FileName) + debugString + " - Wabbitcode";
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
	        foreach (var document in _dockingService.Documents.OfType<AbstractFileEditor>())
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

	    private void DoAssembly(EventHandler<AssemblyFinishFileEventArgs> fileEventHandler,
            EventHandler<AssemblyFinishProjectEventArgs> projectEventHandler)
	    {
	        var activeEditor = _dockingService.ActiveDocument as AbstractFileEditor;
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
			else if (activeEditor != null)
			{
                activeEditor.SaveFile();
                string inputFile = activeEditor.FileName;

				if (fileEventHandler != null)
				{
					_assemblerService.AssemblerFileFinished += fileEventHandler;
				}

				Task.Factory.StartNew(() =>
				{
					string outputFile = Path.ChangeExtension(inputFile, _assemblerService.GetExtension(Settings.Default.OutputFile));
					string originalDir = Path.GetDirectoryName(inputFile);
				    var includes = Settings.Default.IncludeDirs ?? new StringCollection();
					_assemblerService.AssembleFile(inputFile, outputFile, originalDir, includes.Cast<string>());
					if (fileEventHandler != null)
					{
						_assemblerService.AssemblerFileFinished -= fileEventHandler;
					}
				});
			}
		}

		private void DockingService_OnActiveDocumentChanged(object sender, EventArgs eventArgs)
		{
			if (Disposing)
			{
				return;
			}

		    if (_dockingService.ActiveDocument == null)
		    {
		        _statusBarService.SetCaretPosition(-1, -1);
                _statusBarService.SetCodeCountInfo(null);
		    }

            UpdateMenus(ActiveMdiChild != null);
			UpdateTitle();
		}

        #region Initalization

        private void InitializeEvents()
        {
            _dockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;
            _projectService.ProjectOpened += ProjectService_OnProjectOpened;
            _projectService.ProjectClosed += ProjectService_OnProjectClosed;
            _toolBarService.OnToolBarVisibilityChanged += ToolBarService_OnToolBarVisibilityChanged;
            // TODO: fix
            /*Editor.OnEditorDragDrop += MainFormRedone_DragDrop;
            Editor.OnEditorDragEnter += MainFormRedone_DragEnter;*/
        }

        private void InitializeService()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>(dockPanel);
            _statusBarService = ServiceFactory.Instance.GetServiceInstance<IStatusBarService>(statusBar);
            _toolBarService = ServiceFactory.Instance.GetServiceInstance<IToolBarService>(toolStripContainer.TopToolStripPanel);
            _assemblerService = ServiceFactory.Instance.GetServiceInstance<IAssemblerService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
            _symbolService = ServiceFactory.Instance.GetServiceInstance<ISymbolService>();
            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            _fileReaderService = ServiceFactory.Instance.GetServiceInstance<IFileReaderService>();
        }

        private void InitiailzeToolbars()
        {
            _toolBarService.RegisterToolbar("MainToolBar", new MainToolBar());
            if (Settings.Default.DebugToolbar)
            {
                debugToolStrip.Show();
            }
            else
            {
                debugToolStrip.Hide();
            }
        }

        private void HandleArgs(ICollection<string> args)
        {
            if (args.Count == 0)
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
            if (string.IsNullOrEmpty(Settings.Default.StartupProject))
            {
                return;
            }

            try
            {
                bool valid = false;
                if (File.Exists(Settings.Default.StartupProject))
                {
                    valid = _projectService.OpenProject(Settings.Default.StartupProject);
                }
                else
                {
                    Settings.Default.StartupProject = string.Empty;
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
                    Settings.Default.StartupProject = string.Empty;
                }
            }
        }

        private void CreateInternalProject()
		{
			_projectService.CreateInternalProject();
			UpdateProjectMenu(false);
			_dockingService.ProjectViewer.BuildProjTree();
		}

        #endregion

        #region Document Handling

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
                    Form activeForm = _dockingService.ActiveDocument as Form;
                    if (activeForm != null)
                    {
                        activeForm.Refresh();
                    }
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
            _showToolbar = Settings.Default.DebugToolbar;
            Settings.Default.DebugToolbar = true;
            if (!_showToolbar)
            {
                debugToolStrip.Visible = true;
            }

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
            Settings.Default.DebugToolbar = _showToolbar;
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
            //runToolButton.Enabled = enabled || !isDebugging;
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
				RunCommand(new CloseProjectAction(_projectService));
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
            RunCommand(new CreateNewDocumentAction(_dockingService, _documentService));
        }

	    private void newProjectMenuItem_Click(object sender, EventArgs e)
		{
			RunCommand(new CreateNewProjectAction(_dockingService, _documentService, _projectService));
		}

        private void openFileMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new OpenFileAction());
        }

        private void openProjectMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new OpenProjectCommand());
        }

        private void saveMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new SaveCommand(_dockingService.ActiveDocument as AbstractFileEditor));
        }

        private void saveAsMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new SaveAsCommand(_dockingService.ActiveDocument as AbstractFileEditor));
        }

        private void saveAllMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new SaveAllCommand());
        }

        private void saveProjectMenuItem_Click(object sender, EventArgs e)
        {
            _projectService.SaveProject();
            saveProjectMenuItem.Enabled = _projectService.Project.NeedsSave;
        }

        private void closeMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new CloseCommand(_dockingService.ActiveDocument as AbstractFileEditor));
        }

        private void exitMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        #endregion

        #region Edit Menu

        private void undoMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new UndoAction(_dockingService.ActiveContent as IUndoable));
        }

        private void redoMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new RedoAction(_dockingService.ActiveContent as IUndoable));
        }

        private void cutMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new CutAction(_dockingService.ActiveContent as IClipboardOperation));
        }

        private void copyMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new CopyAction(_dockingService.ActiveContent as IClipboardOperation));
        }

        private void pasteMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new PasteAction(_dockingService.ActiveContent as IClipboardOperation));
        }

        private void selectAllMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new SelectAllAction(_dockingService.ActiveContent as ISelectable));
        }

        private void findMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.ShowFindForm(this, SearchMode.Find);
        }

        private void findInFilesMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                _dockingService.FindForm.ShowFor(this, null, SearchMode.FindInFiles);
            }
            else
            {
                activeTextEditor.ShowFindForm(this, SearchMode.FindInFiles);
            }
        }

        private void replaceMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.ShowFindForm(this, SearchMode.Replace);
        }

        private void replaceInFilesMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                _dockingService.FindForm.ShowFor(this, null, SearchMode.FindInFiles);
            }
            else
            {
                activeTextEditor.ShowFindForm(this, SearchMode.FindInFiles);
            }
        }

        private void findAllRefsMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            RunCommand(new FindAllReferencesCommand());
        }

        private void makeUpperMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextToUpper();
        }

        private void makeLowerMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextToLower();
        }

        private void invertCaseMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextInvertCase();
        }

        private void sentenceCaseMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextToSentenceCase();
        }

        private void formatDocMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.FormatLines();
        }

        private void convertSpacesToTabsMenuItem_Click(object sender, EventArgs e)
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.ConvertSpacesToTabs();
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
            var editor = _dockingService.ActiveDocument as ITextEditor;
            if (editor == null)
            {
                return;
            }

            GotoLine gotoBox = new GotoLine(editor.TotalLines);
            DialogResult gotoResult = gotoBox.ShowDialog();
            if (gotoResult != DialogResult.OK)
            {
                return;
            }

            int line = Convert.ToInt32(gotoBox.inputBox.Text);
            editor.GotoLine(line);
        }

        private void gLabelMenuItem_Click(object sender, EventArgs e)
        {
            GotoSymbol gotoSymbolBox = new GotoSymbol(_parserService);
            if (gotoSymbolBox.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            string symbolString = gotoSymbolBox.inputBox.Text;
            IParserData data = _parserService.GetParserData(symbolString, Settings.Default.CaseSensitive).FirstOrDefault();
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
            Preferences prefs = new Preferences();
            prefs.ShowDialog();
        }

        #endregion

        #region View Menu

        private void mainToolMenuItem_Click(object sender, EventArgs e)
        {
            if (mainToolMenuItem.Checked)
            {
                _toolBarService.ShowToolBar("MainToolBar");
            }
            else
            {
                _toolBarService.HideToolBar("MainToolBar");
            }

            Settings.Default.MainToolBar = mainToolMenuItem.Checked;
        }

        private void debugToolMenuItem_Click(object sender, EventArgs e)
        {
            if (debugToolMenuItem.Checked)
            {
                debugToolStrip.Show();
            }
            else
            {
                debugToolStrip.Hide();
            }

            Settings.Default.DebugToolbar = debugToolMenuItem.Checked;
        }

        private void editorToolBarMenuItem_Click(object sender, EventArgs e)
        {
            if (editorToolBarMenuItem.Checked)
            {
                editorToolStrip.Show();
            }
            else
            {
                editorToolStrip.Hide();
            }

            Settings.Default.EditorToolbar = editorToolBarMenuItem.Checked;
        }

        private void labelListMenuItem_Click(object sender, EventArgs e)
        {
            if (labelListMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.LabelList);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.LabelList);
            }
        }

        private void projViewMenuItem_Click(object sender, EventArgs e)
        {
            if (projViewMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.ProjectViewer);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.ProjectViewer);
            }
        }

        private void macroManagerMenuItem_Click(object sender, EventArgs e)
        {
            if (macroManagerMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.MacroManager);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.MacroManager);
            }
        }


        private void debugPanelMenuItem_Click(object sender, EventArgs e)
        {
            if (debugPanelMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.DebugPanel);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.DebugPanel);
            }
        }

        private void callStackMenuItem_Click(object sender, EventArgs e)
        {
            if (callStackMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.CallStack);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.CallStack);
            }
        }

        private void stackViewerMenuItem_Click(object sender, EventArgs e)
        {
            if (stackViewerMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.StackViewer);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.StackViewer);
            }
        }

        private void varTrackMenuItem_Click(object sender, EventArgs e)
        {
            if (varTrackMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.TrackWindow);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.TrackWindow);
            }
        }

        private void breakManagerMenuItem_Click(object sender, EventArgs e)
        {
            if (breakManagerMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.BreakManagerWindow);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.BreakManagerWindow);
            }
        }

        private void outWinMenuItem_Click(object sender, EventArgs e)
        {
            if (outWinMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.OutputWindow);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.OutputWindow);
            }
        }

        private void errListMenuItem_Click(object sender, EventArgs e)
        {
            if (errListMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.ErrorList);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.ErrorList);
            }
        }

        private void findResultsMenuItem_Click(object sender, EventArgs e)
        {
            if (findResultsMenuItem.Checked)
            {
                _dockingService.ShowDockPanel(_dockingService.FindResults);
            }
            else
            {
                _dockingService.HideDockPanel(_dockingService.FindResults);
            }
        }

        private void statusBarMenuItem_Click(object sender, EventArgs e)
        {
            statusBar.Visible = statusBarMenuItem.Checked;
        }

        private void lineNumMenuItem_Click(object sender, EventArgs e)
        {
            Settings.Default.LineNumbers = lineNumMenuItem.Checked;
        }

        private void iconBarMenuItem_Click(object sender, EventArgs e)
        {
            Settings.Default.IconBar = iconBarMenuItem.Checked;
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
            RunCommand(new AddNewFileAction(_dockingService.ProjectViewer));
        }

        private void existingFileMenuItem_Click(object sender, EventArgs e)
        {
            RunCommand(new AddExistingFileAction(_dockingService.ProjectViewer));
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
            RunCommand(new CloseProjectAction(_projectService));
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
            var activeFileEditor = _dockingService.ActiveDocument as AbstractFileEditor;
            if (activeFileEditor == null)
            {
                return;
            }
            
            string inputFile = activeFileEditor.FileName;
            string outputFile = Path.ChangeExtension(activeFileEditor.FileName, "lab");
            string originalDir = Path.GetDirectoryName(inputFile);
            var includeDirs = Settings.Default.IncludeDirs.Cast<string>();
            _assemblerService.AssembleFile(inputFile, outputFile, originalDir,
                includeDirs, AssemblyFlags.Normal | AssemblyFlags.Symtable);
        }

        private void listFileMenuItem_Click(object sender, EventArgs e)
        {
            var activeFileEditor = _dockingService.ActiveDocument as AbstractFileEditor;
            if (activeFileEditor == null)
            {
                return;
            }

            activeFileEditor.SaveFile();
            string inputFile = activeFileEditor.FileName;
            string outputFile = Path.ChangeExtension(inputFile, "lst");
            string originalDir = Path.GetDirectoryName(inputFile);
            var includeDirs = Settings.Default.IncludeDirs.Cast<string>();
            _assemblerService.AssembleFile(inputFile, outputFile, originalDir, includeDirs, AssemblyFlags.List | AssemblyFlags.Normal);
        }

        private void projStatsMenuItem_Click(object sender, EventArgs e)
        {
            var activeFileEditor = _dockingService.ActiveDocument as AbstractFileEditor;
            if (activeFileEditor == null)
            {
                return;
            }

            activeFileEditor.SaveFile();
            string inputFile = activeFileEditor.FileName;
            string outputFile = string.Empty;
            string originalDir = Path.GetDirectoryName(inputFile);
            var includeDirs = Settings.Default.IncludeDirs.Cast<string>();
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
            ITextEditor activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            RunCommand(new ToggleBreakpointAction(activeTextEditor));
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

				_dockingService.ShowDockPanel(_dockingService.ErrorList);
				_dockingService.ShowDockPanel(_dockingService.OutputWindow);
			    var activeForm = _dockingService.ActiveDocument as Form;
				if (activeForm != null)
				{
					activeForm.Refresh();
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
		}

	    private void ShowRefactorForm()
		{
			RefactorForm form = new RefactorForm(_dockingService, _projectService);
			form.ShowDialog();
		}

        private static void RunCommand(AbstractUiAction action)
        {
            action.Execute();
        }
	}
}