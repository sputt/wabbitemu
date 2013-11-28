using System.Reflection;
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

			HandleArgs(args);
		}

	    private IDockContent GetContentFromPersistString(string persistString)
        {
            ToolWindow window = _dockingService.GetDockingWindow(persistString);
	        if (window != null)
	        {
	            return window;
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
            try
            {
                _pluginService.LoadPlugins();
            }
            catch (ReflectionTypeLoadException)
            {
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

            _dockingService.ShowDockPanel(DebugPanel.WindowIdentifier);
            _dockingService.ShowDockPanel(StackViewer.WindowIdentifier);
            _dockingService.ShowDockPanel(ExpressionWindow.WindowIdentifier, StackViewer.WindowIdentifier, DockAlignment.Left);
            _dockingService.ShowDockPanel(CallStack.WindowIdentifier, StackViewer.WindowIdentifier);
            _dockingService.ShowDockPanel(TrackingWindow.WindowIdentifier, ExpressionWindow.WindowIdentifier);
            UpdateTitle();
        }

        private void HideDebugPanels()
        {
            Settings.Default.DebugToolbar = _showToolbar;
            if (!_showToolbar)
            {
                _toolBarService.HideToolBar(DebugToolBarName);
            }

            _dockingService.HideDockPanel(DebugPanel.WindowIdentifier);
            _dockingService.HideDockPanel(TrackingWindow.WindowIdentifier);
            _dockingService.HideDockPanel(CallStack.WindowIdentifier);
            _dockingService.HideDockPanel(StackViewer.WindowIdentifier);
            _dockingService.HideDockPanel(ExpressionWindow.WindowIdentifier);
        }

	    #endregion

        #region Form Events

        private void MainFormRedone_DragDrop(object sender, DragEventArgs e)
        {
            new DragDropCommand(e.Data).Execute();
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