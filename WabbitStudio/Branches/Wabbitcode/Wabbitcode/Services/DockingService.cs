using System.Linq;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.GUI.DockingWindows;
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
        private readonly ToolStripContainer _toolStripContainer;

        private readonly Dictionary<Type, ToolWindow> _registeredDockingWindows = new Dictionary<Type, ToolWindow>();
        

        #endregion

        #region Public Properties

        public DockPanel DockPanel
        {
            get { return _dockPanel; }
        }

        public ToolStripContainer ToolStripContainer
        {
            get { return _toolStripContainer; }
        }

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
            Logger.Log(sb.ToString(), ex);
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

        public DockingService(DockPanel dockPanel, ToolStripContainer toolStripContainer)
        {
            _dockPanel = dockPanel;
            _toolStripContainer = toolStripContainer;

            // Theme
            _dockPanel.Theme = new VS2012LightTheme();
            ToolStripManager.Renderer = new VS2012ToolStripRenderer();

            _dockPanel.ActiveDocumentChanged += DockPanelOnActiveDocumentChanged;
            _dockPanel.ContentAdded += DockPanel_ContentAdded;
            _dockPanel.ContentRemoved += DockPanel_ContentRemoved;

            // this registration should be fast, so its ok to do in the constructor
            RegisterDockingWindow<ProjectViewer>();
            RegisterDockingWindow<ErrorList>();
            RegisterDockingWindow<TrackingWindow>();
            RegisterDockingWindow<DebugPanel>();
            RegisterDockingWindow<CallStack>();
            RegisterDockingWindow<LabelList>();
            RegisterDockingWindow<OutputWindow>();
            RegisterDockingWindow<FindResultsWindow>();
            RegisterDockingWindow<MacroManager>();
            RegisterDockingWindow<BreakpointManagerWindow>();
            RegisterDockingWindow<StackViewer>();
            RegisterDockingWindow<ExpressionWindow>();
        }

        public void RegisterDockingWindow<T>() where T : ToolWindow
        {
            if (_registeredDockingWindows.ContainsKey(typeof(T)))
            {
                throw new ArgumentException("This docking window type is already registered");
            }

            _registeredDockingWindows.Add(typeof(T), null);
        }

        public ToolWindow GetDockingWindow(Type panelType)
        {
            if (!(typeof(ToolWindow).IsAssignableFrom(panelType)))
            {
                throw new ArgumentException("Not type of ToolWindow");
            }

            ToolWindow window;
            _registeredDockingWindows.TryGetValue(panelType, out window);
            if (window != null)
            {
                return window;
            }

            ToolWindow newWindow = (ToolWindow) Activator.CreateInstance(panelType);
            _registeredDockingWindows.Remove(panelType);
            _registeredDockingWindows.Add(panelType, newWindow);

            return newWindow;
        }

        public T GetDockingWindow<T>() where T : ToolWindow
        {
            ToolWindow window;
            _registeredDockingWindows.TryGetValue(typeof(T), out window);
            if (window != null)
            {
                return window as T;
            }

            T newWindow = Activator.CreateInstance<T>();
            _registeredDockingWindows.Remove(typeof(T));
            _registeredDockingWindows.Add(typeof(T), newWindow);

            return newWindow;
        }

        public void HideDockPanel(DockContent panel)
        {
            if (panel == null)
            {
                return;
            }

            panel.Hide();
        }

        public void HideDockPanel<T>() where T : ToolWindow
        {
            HideDockPanel(typeof(T));
        }

        public void HideDockPanel(Type panelType)
        {
            if (!(typeof(ToolWindow).IsAssignableFrom(panelType)))
            {
                throw new ArgumentException("Not type of ToolWindow");
            }

            ToolWindow window;
            _registeredDockingWindows.TryGetValue(panelType, out window);

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

        public void ShowDockPanel(Type panelType)
        {
            if (!(typeof(ToolWindow).IsAssignableFrom(panelType)))
            {
                throw new ArgumentException("Not type of ToolWindow");
            }

            ToolWindow window = GetDockingWindow(panelType);
            if (window != null)
            {
                window.Show(_dockPanel);
            }
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

        public void ShowDockPanel<T>() where T : ToolWindow
        {
            ShowDockPanel(typeof(T));
        }

        public void ShowDockPanel<T, TBefore>() 
            where T : ToolWindow 
            where TBefore : ToolWindow
        {
            ToolWindow window = GetDockingWindow<T>();
            ToolWindow beforeContent = GetDockingWindow<TBefore>();
            if (window != null && beforeContent != null)
            {
                window.Show(beforeContent.DockHandler.Pane, beforeContent);
            }
        }

        public void ShowDockPanel<T, TBefore>(DockAlignment alignment)
            where T : ToolWindow 
            where TBefore : ToolWindow
        {
            ToolWindow window = GetDockingWindow<T>();
            ToolWindow beforeContent = GetDockingWindow<TBefore>();
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