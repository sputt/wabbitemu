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

		#endregion

		#region Public Properties
		public BreakpointManagerWindow BreakManagerWindow { get; private set; }
		public CallStack CallStack { get; private set; }
		public DebugPanel DebugPanel { get; private set; }
		public ErrorList ErrorList { get; private set; }
		public FindAndReplaceForm FindForm { get; private set; }
		public FindResultsWindow FindResults { get; private set; }
		public OutputWindow OutputWindow { get; private set; }
		public ProjectViewer ProjectViewer { get; private set; }
		public StackViewer StackViewer { get; private set; }
		public TrackingWindow TrackWindow { get; private set; }
		public LabelList LabelList { get; private set; }
		public MacroManager MacroManager { get; private set; }
        public ExpressionWindow ExpressionWindow { get; private set; }

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
			if (parent.InvokeRequired)
			{
				Func<DialogResult> invokeBox = () => ShowMessageBox(parent, text, caption, messageBoxButtons, messageBoxIcon);
				return (DialogResult)parent.Invoke(invokeBox);
			}
			return MessageBox.Show(parent, text, caption, messageBoxButtons, messageBoxIcon);
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

		public void HideDockPanel(DockContent panel)
		{
			if (panel == null)
			{
				return;
			}

			panel.Hide();
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
            ProjectViewer = new ProjectViewer();
            ErrorList = new ErrorList();
            TrackWindow = new TrackingWindow();
            DebugPanel = new DebugPanel();
            CallStack = new CallStack();
            LabelList = new LabelList();
            OutputWindow = new OutputWindow();
            FindForm = new FindAndReplaceForm();
            FindResults = new FindResultsWindow();
            MacroManager = new MacroManager();
            BreakManagerWindow = new BreakpointManagerWindow();
            StackViewer = new StackViewer();
            ExpressionWindow = new ExpressionWindow();
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