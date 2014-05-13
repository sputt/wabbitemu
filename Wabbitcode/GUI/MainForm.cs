using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;
using Microsoft.Practices.Unity;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.GUI.DocumentWindows;
using Revsoft.Wabbitcode.GUI.Menus;
using Revsoft.Wabbitcode.GUI.ToolBars;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.GUI
{
	public partial class MainForm : Form
	{
	    private const string MainMenuName = "Main Menu";
	    private const string DebugToolBarName = "Debug Toolbar";
	    private const string MainToolBarName = "Main Toolbar";

	    #region Private Members

	    private bool _showToolbar = true;
	    private IFileTypeMethodFactory _fileTypeMethodFactory;

		#region Services

	    private IDockingService _dockingService;
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
			InitializeDependencies();
			InitializeEvents();
            InitializeMenus();
            InitializeToolbars();
		    RegisterFileTypes();

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
	        Type type = Type.GetType(parsedStrings[0]);
            if (parsedStrings.Length < 3 || type == null || type.IsAssignableFrom(typeof(AbstractFileEditor)))
            {
                return null;
            }

            FilePath fileName = new FilePath(parsedStrings[1]);
            if (!File.Exists(fileName))
            {
                return null;
            }

            new OpenFileAction(fileName).Execute();
	        var doc = _dockingService.Documents.OfType<AbstractFileEditor>()
                .FirstOrDefault(d => fileName == d.FileName);
	        if (doc == null)
	        {
	            return null;
	        }

	        doc.PersistStringLoad(parsedStrings);
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

	    private void DockingServiceActiveDocumentChanged(object sender, EventArgs eventArgs)
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
            _dockingService.ActiveDocumentChanged += DockingServiceActiveDocumentChanged;
            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += (sender, args) => EndDebug();
            _projectService.ProjectOpened += ProjectService_OnProjectOpened;
        }

	    private void InitializeDependencies()
        {
            // services
            DependencyFactory.RegisterType<IDockingService, DockingService>(new InjectionConstructor(dockPanel));
            DependencyFactory.RegisterType<IStatusBarService, StatusBarService>(new InjectionConstructor(statusBar));
            DependencyFactory.RegisterType<IMenuService, MenuService>(new InjectionConstructor(toolStripContainer.TopToolStripPanel));
            DependencyFactory.RegisterType<IToolBarService, ToolBarService>(new InjectionConstructor(toolStripContainer.TopToolStripPanel));
            DependencyFactory.RegisterType<IAssemblerService, AssemblerService>();
            DependencyFactory.RegisterType<IBackgroundAssemblerService, BackgroundAssemblerService>();
            DependencyFactory.RegisterType<IProjectService, ProjectService>();
            DependencyFactory.RegisterType<IParserService, ParserService>();
            DependencyFactory.RegisterType<ISymbolService, SymbolService>();
            DependencyFactory.RegisterType<IFileService, FileService>();
            DependencyFactory.RegisterType<IDebuggerService, DebuggerService>();
            DependencyFactory.RegisterType<IPluginService, PluginService>();
            DependencyFactory.RegisterType<ILoggingService, LoggingService>();
            // factories
            DependencyFactory.RegisterType<IAssemblerFactory, AssemblerFactory>();
            DependencyFactory.RegisterType<IParserFactory, ParserFactory>();
            DependencyFactory.RegisterType<IFileTypeMethodFactory, FileTypeMethodFactory>();
	        
            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _statusBarService = DependencyFactory.Resolve<IStatusBarService>();
            _menuService = DependencyFactory.Resolve<IMenuService>();
            _toolBarService = DependencyFactory.Resolve<IToolBarService>();
            _projectService = DependencyFactory.Resolve<IProjectService>();
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            _pluginService = DependencyFactory.Resolve<IPluginService>();
	        _fileTypeMethodFactory = DependencyFactory.Resolve<IFileTypeMethodFactory>();
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

        private void RegisterFileTypes()
        {
            _fileTypeMethodFactory.RegisterFileType(".asm", path => Editor.OpenDocument(path) != null);
            _fileTypeMethodFactory.RegisterFileType(".z80", path => Editor.OpenDocument(path) != null);
            _fileTypeMethodFactory.RegisterFileType(".inc", path => Editor.OpenDocument(path) != null);
            _fileTypeMethodFactory.RegisterFileType(".bmp", path => ImageViewer.OpenImage(path) != null);
            _fileTypeMethodFactory.RegisterFileType(".png", path => ImageViewer.OpenImage(path) != null);
            _fileTypeMethodFactory.RegisterDefaultHandler(path => Editor.OpenDocument(path) != null);
        }

        private static void HandleArgs(ICollection<string> args)
        {
            if (args.Count == 0)
            {
                return;
            }

            new OpenFileAction(args
                .Where(arg => !string.IsNullOrEmpty(arg))
                .Select(arg => new FilePath(arg))
                .ToArray()
                ).Execute();
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
                    valid = _projectService.OpenProject(new FilePath(Settings.Default.StartupProject));
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

        private void ProjectService_OnProjectOpened(object sender, EventArgs e)
        {
            if (_projectService.Project.IsInternal || Settings.Default.StartupProject == _projectService.Project.ProjectFile)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.Invoke(() => ProjectService_OnProjectOpened(sender, e));
                return;
            }

            if (MessageBox.Show(this, "Would you like to make this your default project?",
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
				_dockingService.SavePanels();
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