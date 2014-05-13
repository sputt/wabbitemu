using System.Linq;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Resources;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public sealed class DockingService : IDockingService
    {
        #region Private Members

        private readonly DockPanel _dockPanel;
        private readonly Dictionary<string, ToolWindow> _registeredDockingWindows = new Dictionary<string, ToolWindow>();

        #endregion

        #region Public Properties

        public IDockContent ActiveContent
        {
            get { return _dockPanel == null ? null : _dockPanel.ActiveContent; }
        }

        public IDockContent ActiveDocument
        {
            get { return _dockPanel.ActiveDocument; }
        }

        public IEnumerable<IDockContent> Documents
        {
            get
            {
                lock (_dockPanel.Documents)
                {
                    return _dockPanel.Documents;
                }
            }
        }

        #endregion

        #region Static Methods

        public static void ShowError(string error)
        {
            MessageBox.Show(error, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        public static void ShowError(string error, Exception ex)
        {
            StringBuilder sb = new StringBuilder(error);
            sb.Append("\nReason: ");
            sb.Append(ex.Message);
            MessageBox.Show(sb.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            DependencyFactory.Resolve<ILoggingService>().Log(sb.ToString(), ex);
        }

        internal static DialogResult ShowMessageBox(Form parent, string text, string caption,
            MessageBoxButtons messageBoxButtons, MessageBoxIcon messageBoxIcon)
        {
            if (parent == null || !parent.InvokeRequired)
            {
                return MessageBox.Show(parent, text, caption, messageBoxButtons, messageBoxIcon);
            }

            Func<DialogResult> invokeBox = () => ShowMessageBox(parent, text, caption, messageBoxButtons, messageBoxIcon);
            return (DialogResult) parent.Invoke(invokeBox);
        }

        #endregion

        #region Events

        public event EventHandler ActiveDocumentChanged;
        public event EventHandler<DockContentEventArgs> DocumentWindowAdded;
        public event EventHandler<DockContentEventArgs> DocumentWindowRemoved;

        private void DockPanelOnActiveDocumentChanged(object sender, EventArgs eventArgs)
        {
            if (ActiveDocumentChanged != null)
            {
                ActiveDocumentChanged(sender, eventArgs);
            }
        }

        private void DockPanel_ContentRemoved(object sender, DockContentEventArgs e)
        {
            if (DocumentWindowRemoved != null)
            {
                DocumentWindowRemoved(sender, e);
            }
        }

        private void DockPanel_ContentAdded(object sender, DockContentEventArgs e)
        {
            if (DocumentWindowAdded != null)
            {
                DocumentWindowAdded(sender, e);
            }
        }

        #endregion

        public DockingService(DockPanel dockPanel)
        {
            _dockPanel = dockPanel;

            // Theme
            _dockPanel.Theme = new VS2012LightTheme();
            ToolStripManager.Renderer = new VS2012ToolStripRenderer();

            _dockPanel.ActiveDocumentChanged += DockPanelOnActiveDocumentChanged;
            _dockPanel.ContentAdded += DockPanel_ContentAdded;
            _dockPanel.ContentRemoved += DockPanel_ContentRemoved;
        }

        public void RegisterDockingWindow(ToolWindow dockingWindow)
        {
            if (dockingWindow == null)
            {
                throw new ArgumentNullException("dockingWindow");
            }

            if (_registeredDockingWindows.ContainsKey(dockingWindow.WindowName))
            {
                throw new ArgumentException("This docking window name is already registered");
            }

            _registeredDockingWindows.Add(dockingWindow.WindowName, dockingWindow);
        }

        public ToolWindow GetDockingWindow(string name)
        {
            ToolWindow window;
            _registeredDockingWindows.TryGetValue(name, out window);
            return window;
        }

        public void HideDockPanel(DockContent panel)
        {
            if (panel == null)
            {
                return;
            }

            panel.Hide();
        }

        public void HideDockPanel(string panelName)
        {
            if (string.IsNullOrEmpty(panelName))
            {
                return;
            }

            ToolWindow window;
            _registeredDockingWindows.TryGetValue(panelName, out window);

            if (window != null)
            {
                window.Hide();
            }
        }

        public void ShowDockPanel(DockContent panel)
        {
            if (panel == null)
            {
                return;
            }

            panel.Show(_dockPanel);
        }

        public void ShowDockPanel(DockContent panel, IDockContent beforeContent)
        {
            if (panel == null || beforeContent == null)
            {
                return;
            }

            panel.Show(beforeContent.DockHandler.Pane, beforeContent);
        }

        public void ShowDockPanel(DockContent panel, IDockContent beforeContent, DockAlignment alignment)
        {
            if (panel == null || beforeContent == null)
            {
                return;
            }

            panel.Show(beforeContent.DockHandler.Pane, alignment, .5);
        }

        public void ShowDockPanel(string panelName)
        {
            if (string.IsNullOrEmpty(panelName))
            {
                return;
            }

            ToolWindow window;
            _registeredDockingWindows.TryGetValue(panelName, out window);

            if (window != null)
            {
                window.Show(_dockPanel);
            }
        }

        public void ShowDockPanel(string panelName, string beforeContentName)
        {
            if (string.IsNullOrEmpty(panelName) || string.IsNullOrEmpty(beforeContentName))
            {
                return;
            }

            ToolWindow window, beforeContent;
            _registeredDockingWindows.TryGetValue(panelName, out window);
            _registeredDockingWindows.TryGetValue(beforeContentName, out beforeContent);

            if (window != null && beforeContent != null)
            {
                window.Show(beforeContent.DockHandler.Pane, beforeContent);
            }
        }

        public void ShowDockPanel(string panelName, string beforeContentName, DockAlignment alignment)
        {
            if (string.IsNullOrEmpty(panelName) || string.IsNullOrEmpty(beforeContentName))
            {
                return;
            }

            ToolWindow window, beforeContent;
            _registeredDockingWindows.TryGetValue(panelName, out window);
            _registeredDockingWindows.TryGetValue(beforeContentName, out beforeContent);

            if (window != null && beforeContent != null)
            {
                window.Show(beforeContent.DockHandler.Pane, alignment, .5);
            }
        }

        public void LoadConfig(DeserializeDockContent dockContent)
        {
            //try
            //{
            if (File.Exists(FileLocations.ConfigFile))
            {
                _dockPanel.LoadFromXml(FileLocations.ConfigFile, dockContent);
            }
            //}
            //catch (Exception ex)
            //{
            //    ShowError("Error Loading the DockPanel Config File", ex);
            //}
        }

        public void InitPanels()
        {
            RegisterDockingWindow(new ProjectViewer());
            RegisterDockingWindow(new ErrorList());
            RegisterDockingWindow(new TrackingWindow());
            RegisterDockingWindow(new DebugPanel());
            RegisterDockingWindow(new CallStack());
            RegisterDockingWindow(new LabelList());
            RegisterDockingWindow(new OutputWindow());
            RegisterDockingWindow(new FindResultsWindow());
            RegisterDockingWindow(new MacroManager());
            RegisterDockingWindow(new BreakpointManagerWindow());
            RegisterDockingWindow(new StackViewer());
            RegisterDockingWindow(new ExpressionWindow());
        }

        public void SavePanels()
        {
            try
            {
                string dir = Path.GetDirectoryName(FileLocations.ConfigFile);
                if (dir != null && !Directory.Exists(dir))
                {
                    if (MessageBox.Show("Directory '" + dir + "' does not exist. Would you like to create it?", "Directory does not exist", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) != DialogResult.Yes)
                    {
                        Properties.Settings.Default.ConfigLoc = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "DockPanel.config");
                    }
                }

                if (File.Exists(FileLocations.ConfigFile))
                {
                    File.Delete(FileLocations.ConfigFile);
                }

                _dockPanel.SaveAsXml(FileLocations.ConfigFile);

                _dockPanel.Documents.OfType<Form>().ToList().ForEach(f => f.Close());
            }
            catch (Exception ex)
            {
                throw new Exception("Error saving DockPanel.config file", ex);
            }
        }
    }
}