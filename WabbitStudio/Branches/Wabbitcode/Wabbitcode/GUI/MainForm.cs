using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.GUI.DocumentWindows;
using Revsoft.Wabbitcode.GUI.Menus;
using Revsoft.Wabbitcode.GUI.ToolBars;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
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

        #region Services

        private readonly IDockingService _dockingService = DependencyFactory.Resolve<IDockingService>();
        private readonly IProjectService _projectService = DependencyFactory.Resolve<IProjectService>();
        private readonly IStatusBarService _statusBarService = DependencyFactory.Resolve<IStatusBarService>();
        private readonly IToolBarService _toolBarService = DependencyFactory.Resolve<IToolBarService>();
        private readonly IMenuService _menuService = DependencyFactory.Resolve<IMenuService>();
        private readonly IDebuggerService _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        private readonly IPluginService _pluginService = DependencyFactory.Resolve<IPluginService>();

        #endregion

        #endregion

        #region Events

        #endregion

        public MainForm(ToolStripContainer toolStripContainer, ICollection<string> args)
        {
            _toolStripContainer = toolStripContainer;
            InitializeComponent();
            RestoreWindow();

            InitializeUi();
            InitializeEvents();

            HandleArgs(args);
        }

        private IDockContent GetContentFromPersistString(string persistString)
        {
            string[] parsedStrings = persistString.Split(';');
            Type type = Type.GetType(parsedStrings[0]);
            if (type == null)
            {
                return null;
            }

            if (typeof(ToolWindow).IsAssignableFrom(type))
            {
                ToolWindow window = _dockingService.GetDockingWindow(type);
                if (window != null)
                {
                    return window;
                }
            }

            if (parsedStrings.Length < 3 || !typeof(AbstractFileEditor).IsAssignableFrom(type))
            {
                return null;
            }

            FilePath fileName = new FilePath(parsedStrings[1]);
            if (!File.Exists(fileName))
            {
                return null;
            }

            AbstractUiAction.RunCommand(new OpenFileAction(fileName));
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
            if (Disposing || IsDisposed)
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
            Task.Factory.StartNew(() =>
            {
                _dockingService.ActiveDocumentChanged += DockingServiceActiveDocumentChanged;
                _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
                _debuggerService.OnDebuggingEnded += (sender, args) => EndDebug();
                _projectService.ProjectOpened += ProjectService_OnProjectOpened;

                LoadStartupProject();

                if (_projectService.Project == null)
                {
                    _projectService.CreateInternalProject();
                }

                try
                {
                    _pluginService.LoadPlugins();
                }
                catch (Exception ex)
                {
                    Logger.Log("Loading plugin failed", ex);
                }
            });
        }

        private void InitializeUi()
        {
            IFileTypeMethodFactory fileTypeMethodFactory = DependencyFactory.Resolve<IFileTypeMethodFactory>();
            fileTypeMethodFactory.RegisterFileType(".asm", path => DocumentWindows.TextEditor.OpenDocument(path) != null);
            fileTypeMethodFactory.RegisterFileType(".z80", path => DocumentWindows.TextEditor.OpenDocument(path) != null);
            fileTypeMethodFactory.RegisterFileType(".inc", path => DocumentWindows.TextEditor.OpenDocument(path) != null);
            fileTypeMethodFactory.RegisterFileType(".bmp", path => ImageViewer.OpenImage(path) != null);
            fileTypeMethodFactory.RegisterFileType(".png", path => ImageViewer.OpenImage(path) != null);
            fileTypeMethodFactory.RegisterDefaultHandler(path => DocumentWindows.TextEditor.OpenDocument(path) != null); 

            _dockingService.LoadConfig(GetContentFromPersistString);
            _menuService.RegisterMenu(MainMenuName, new MainMenuStrip());
            _toolBarService.RegisterToolbar(DebugToolBarName, new DebugToolBar());
            _toolBarService.RegisterToolbar(MainToolBarName, new MainToolBar());
        }

        private static void HandleArgs(ICollection<string> args)
        {
            if (args.Count == 0)
            {
                return;
            }

            AbstractUiAction.RunCommand(new OpenFileAction(args
                .Where(arg => !string.IsNullOrEmpty(arg))
                .Select(arg => new FilePath(arg))
                .ToArray()));
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

            _dockingService.ShowDockPanel<DebugPanel>();
            _dockingService.ShowDockPanel<StackViewer>();
            _dockingService.ShowDockPanel<ExpressionWindow, StackViewer>(DockAlignment.Left);
            _dockingService.ShowDockPanel<CallStack, StackViewer>();
            _dockingService.ShowDockPanel<TrackingWindow, ExpressionWindow>();
            UpdateTitle();
        }

        private void HideDebugPanels()
        {
            Settings.Default.DebugToolbar = _showToolbar;
            if (!_showToolbar)
            {
                _toolBarService.HideToolBar(DebugToolBarName);
            }

            _dockingService.HideDockPanel<DebugPanel>();
            _dockingService.HideDockPanel<TrackingWindow>();
            _dockingService.HideDockPanel<CallStack>();
            _dockingService.HideDockPanel<StackViewer>();
            _dockingService.HideDockPanel<ExpressionWindow>();
        }

        #endregion

        #region Form Events

        private void MainFormRedone_DragDrop(object sender, DragEventArgs e)
        {
            AbstractUiAction.RunCommand(new DragDropCommand(e.Data));
        }

        private void MainFormRedone_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Copy : DragDropEffects.None;
        }

        private void MainFormRedone_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (_debuggerService.CurrentDebugger != null)
            {
                AbstractUiAction.RunCommand(new StopDebuggerAction());
            }

            if (!_projectService.Project.IsInternal)
            {
                AbstractUiAction.RunCommand(new CloseProjectAction());
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