using Revsoft.Wabbitcode.DockingWindows;
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
	public sealed class DockingService : IDockingService
	{
		#region Private Members

		private DockPanel _dockPanel;
        private readonly Dictionary<string, ToolWindow> _registeredDockingWindows = new Dictionary<string, ToolWindow>(); 

		#endregion

		#region Public Properties

	    public IDockContent ActiveContent
		{
			get
			{
				return _dockPanel == null ? null : _dockPanel.ActiveContent;
			}
		}

		public IDockContent ActiveDocument
		{
			get
			{
			    return _dockPanel.ActiveDocument;
			}
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
		}

		internal static DialogResult ShowMessageBox(Form parent, string text, string caption,
						MessageBoxButtons messageBoxButtons, MessageBoxIcon messageBoxIcon)
		{
		    if (parent == null || !parent.InvokeRequired)
		    {
		        return MessageBox.Show(parent, text, caption, messageBoxButtons, messageBoxIcon);
		    }

		    Func<DialogResult> invokeBox = () => ShowMessageBox(parent, text, caption, messageBoxButtons, messageBoxIcon);
		    return (DialogResult)parent.Invoke(invokeBox);
		}

		#endregion

		#region Events

		public event EventHandler OnActiveDocumentChanged;

		private void DockPanelOnActiveDocumentChanged(object sender, EventArgs eventArgs)
		{
			if (OnActiveDocumentChanged != null)
			{
				OnActiveDocumentChanged(sender, eventArgs);
			}
		}

		#endregion

        public void RegisterDockingWindow(string name, ToolWindow dockingWindow)
        {
            if (_registeredDockingWindows.ContainsKey(name))
            {
                throw new ArgumentException("This docking window name is already registered");
            }

            _registeredDockingWindows.Add(name, dockingWindow);
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
            if (panel == null)
            {
                return;
            }

            panel.Show(beforeContent.DockHandler.Pane, beforeContent);
        }

	    public void ShowDockPanel(DockContent panel, IDockContent beforeContent, DockAlignment alignment)
	    {
            if (panel == null)
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
			try
			{
				if (File.Exists(FileLocations.ConfigFile))
				{
					_dockPanel.LoadFromXml(FileLocations.ConfigFile, dockContent);
				}
			}
            catch (Exception ex)
            {
                ShowError("Error Loading the DockPanel Config File", ex);
            }
		}

	    public void InitPanels()
	    {
            RegisterDockingWindow(ProjectViewer.WindowName, new ProjectViewer());
            RegisterDockingWindow(ErrorList.WindowName, new ErrorList());
            RegisterDockingWindow(TrackingWindow.WindowName, new TrackingWindow());
            RegisterDockingWindow(DebugPanel.WindowName, new DebugPanel());
            RegisterDockingWindow(CallStack.WindowName, new CallStack());
            RegisterDockingWindow(LabelList.WindowName, new LabelList());
            RegisterDockingWindow(OutputWindow.WindowName, new OutputWindow());
            //RegisterDockingWindow(FindForm = new FindAndReplaceForm());
            RegisterDockingWindow(FindResultsWindow.WindowName, new FindResultsWindow());
            RegisterDockingWindow(MacroManager.WindowName, new MacroManager());
            RegisterDockingWindow(BreakpointManagerWindow.WindowName, new BreakpointManagerWindow());
            RegisterDockingWindow(StackViewer.WindowName, new StackViewer());
            RegisterDockingWindow(ExpressionWindow.WindowName, new ExpressionWindow());
	    }

	    #region IService

		public void DestroyService()
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
			}
			catch (Exception ex)
			{
				throw new Exception("Error saving DockPanel.config file", ex);
			}
		}

		public void InitService(params object[] objects)
		{
			if (objects.Length != 1)
			{
				throw new ArgumentException("Invalid number of arguments");
			}
			_dockPanel = objects[0] as DockPanel;
			if (_dockPanel == null)
			{
				throw new ArgumentException("First argument is not of type DockPanel");
			}

			// Theme
			_dockPanel.Theme = new VS2012LightTheme();
			ToolStripManager.Renderer = new VS2012ToolStripRenderer();

			_dockPanel.ActiveDocumentChanged += DockPanelOnActiveDocumentChanged;
		}

		#endregion
	}
}