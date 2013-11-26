using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.DockingWindows;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using IFileReaderService = Revsoft.Wabbitcode.Services.Interfaces.IFileReaderService;

namespace Revsoft.Wabbitcode
{
	public partial class MainForm : Form
	{
	    private const string MainMenuName = "Main Menu";
	    private const string DebugToolBarName = "Debug Toolbar";
	    private const string MainToolBarName = "Main Toolbar";

	    #region Private Members

	    private bool _showToolbar = true;

		#region Services

	    private IDockingService _dockingService;
        private IDocumentService _documentService;
		private IProjectService _projectService;
	    private IStatusBarService _statusBarService;
	    private IToolBarService _toolBarService;
	    private IMenuService _menuService;
	    private IDebuggerService _debuggerService;
	    private IPluginService _pluginService;

	    #endregion

		#endregion

		#region Events

		#endregion

		public MainForm(ICollection<string> args)
		{
			InitializeComponent();
			RestoreWindow();
			InitializeService();
			InitializeEvents();
            InitializeMenus();
            InitializeToolbars();

		    _dockingService.InitPanels();
			_dockingService.LoadConfig(GetContentFromPersistString);

		    InitializePlugins();

			if (args.Count == 0)
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
		}

	    // TODO: generalize this
	    private IDockContent GetContentFromPersistString(string persistString)
        {
            if (persistString == typeof(OutputWindow).ToString())
            {
                return _dockingService.GetDockingWindow(OutputWindow.WindowName);
            }
            if (persistString == typeof(LabelList).ToString())
            {
                return _dockingService.GetDockingWindow(LabelList.WindowName);
            }
            if (persistString == typeof(ErrorList).ToString())
            {
                return _dockingService.GetDockingWindow(ErrorList.WindowName);
            }
            if (persistString == typeof(DebugPanel).ToString())
            {
                return _dockingService.GetDockingWindow(DebugPanel.WindowName);
            }
            if (persistString == typeof(CallStack).ToString())
            {
                return _dockingService.GetDockingWindow(CallStack.WindowName);
            }
            if (persistString == typeof(TrackingWindow).ToString())
            {
                return _dockingService.GetDockingWindow(TrackingWindow.WindowName);
            }
            if (persistString == typeof(ProjectViewer).ToString())
            {
                return _dockingService.GetDockingWindow(ProjectViewer.WindowName);
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

	        int column = int.Parse(parsedStrings[2]);
            int line = int.Parse(parsedStrings[3]);

            Editor doc = _documentService.OpenDocument(fileName);
	        doc.CaretLine = line;
	        doc.CaretColumn = column;
            return doc;
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

	    /// <summary>
		/// Updates the title of the app with the filename.
		/// </summary>
		private void UpdateTitle()
		{
			string debugString = string.Empty;
			if (_debuggerService.CurrentDebugger != null)
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

			UpdateTitle();
		}

        #region Initalization

        private void InitializeEvents()
        {
            _dockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;
            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += (sender, args) => EndDebug();
            _projectService.ProjectOpened += ProjectService_OnProjectOpened;
        }

	    private void InitializeService()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>(dockPanel);
            _statusBarService = ServiceFactory.Instance.GetServiceInstance<IStatusBarService>(statusBar);
            _menuService = ServiceFactory.Instance.GetServiceInstance<IMenuService>(toolStripContainer.TopToolStripPanel);
            _toolBarService = ServiceFactory.Instance.GetServiceInstance<IToolBarService>(toolStripContainer.TopToolStripPanel);
            ServiceFactory.Instance.GetServiceInstance<IAssemblerService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            ServiceFactory.Instance.GetServiceInstance<IParserService>();
            ServiceFactory.Instance.GetServiceInstance<ISymbolService>();
            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            ServiceFactory.Instance.GetServiceInstance<IFileReaderService>();
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
            _pluginService = ServiceFactory.Instance.GetServiceInstance<IPluginService>();
        }

        private void InitializeToolbars()
        {
            _toolBarService.RegisterToolbar(DebugToolBarName, new DebugToolBar());
            _toolBarService.RegisterToolbar(MainToolBarName, new MainToolBar());
        }

	    private void InitializeMenus()
	    {
	        _menuService.RegisterMenu(MainMenuName, new MainMenuStrip());
	    }

        private void InitializePlugins()
        {
            _pluginService.LoadPlugins();
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

        private void SaveWindow()
        {
            Settings.Default.WindowSize = WindowState != FormWindowState.Normal ? new Size(RestoreBounds.Width, RestoreBounds.Height) : Size;
            Settings.Default.WindowState = WindowState;
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
                    _projectService.CreateInternalProject();
                }
            }
            catch (Exception ex)
            {
                _projectService.CreateInternalProject();
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

        private void ProjectService_OnProjectOpened(object sender, EventArgs eventArgs)
        {
            if (_projectService.Project.IsInternal || Settings.Default.StartupProject == _projectService.Project.ProjectFile)
            {
                return;
            }

            if (MessageBox.Show("Would you like to make this your default project?",
                    "Startup Project",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Question) == DialogResult.Yes)
            {
                Settings.Default.StartupProject = _projectService.Project.ProjectFile;
            }
        }

        #endregion

        #region Debugging

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            e.Debugger.OnDebuggerStep += debugger_OnDebuggerStep;
            e.Debugger.OnDebuggerRunningChanged += debugger_OnDebuggerRunningChanged;
            this.Invoke(ShowDebugPanels);
        }

        private void EndDebug()
        {
            if (InvokeRequired)
            {
                this.Invoke(EndDebug);
                return;
            }

            UpdateTitle();
            HideDebugPanels();

            _documentService.RemoveDebugHighlight();
        }

        private void debugger_OnDebuggerStep(object sender, DebuggerStepEventArgs e)
        {
            this.Invoke(() =>
            {
                _documentService.RemoveDebugHighlight();
                _documentService.GotoLine(e.Location.FileName, e.Location.LineNumber);
                _documentService.HighlightDebugLine(e.Location.LineNumber);

            });
        }

        private void debugger_OnDebuggerRunningChanged(object sender, DebuggerRunningEventArgs e)
        {
            this.Invoke(() =>
            {
                _documentService.RemoveDebugHighlight();
                if (e.Running)
                {
                    Form activeForm = _dockingService.ActiveDocument as Form;
                    if (activeForm != null)
                    {
                        activeForm.Refresh();
                    }
                }
                else
                {
                    Activate();
                    _documentService.GotoLine(e.Location.FileName, e.Location.LineNumber);
                    _documentService.HighlightDebugLine(e.Location.LineNumber);
                }
            });
        }

        private void ShowDebugPanels()
        {
            _showToolbar = Settings.Default.DebugToolbar;
            Settings.Default.DebugToolbar = true;
            if (!_showToolbar)
            {
                _toolBarService.ShowToolBar(DebugToolBarName);
            }

            _dockingService.ShowDockPanel(DebugPanel.WindowName);
            _dockingService.ShowDockPanel(StackViewer.WindowName);
            _dockingService.ShowDockPanel(ExpressionWindow.WindowName, StackViewer.WindowName, DockAlignment.Left);
            _dockingService.ShowDockPanel(CallStack.WindowName, StackViewer.WindowName);
            _dockingService.ShowDockPanel(TrackingWindow.WindowName, ExpressionWindow.WindowName);
            UpdateTitle();
        }

        private void HideDebugPanels()
        {
            Settings.Default.DebugToolbar = _showToolbar;
            if (!_showToolbar)
            {
                _toolBarService.HideToolBar(DebugToolBarName);
            }

            _dockingService.HideDockPanel(DebugPanel.WindowName);
            _dockingService.HideDockPanel(TrackingWindow.WindowName);
            _dockingService.HideDockPanel(CallStack.WindowName);
            _dockingService.HideDockPanel(StackViewer.WindowName);
            _dockingService.HideDockPanel(ExpressionWindow.WindowName);
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
			if (_debuggerService.CurrentDebugger != null)
			{
				new StopDebuggerAction().Execute();
			}

			if (!_projectService.Project.IsInternal)
			{
				new CloseProjectAction().Execute();
			}

            _pluginService.UnloadPlugins();

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
	}
}