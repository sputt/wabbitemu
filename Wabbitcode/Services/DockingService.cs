using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.Resources;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
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
		public bool HasBeenInited { get; private set; }
		public LabelList LabelList { get; private set; }
		public MacroManager MacroManager { get; private set; }

		public IDockContent ActiveContent
		{
			get
			{
				return _dockPanel == null ? null : _dockPanel.ActiveContent;
			}
		}

		public Editor ActiveDocument
		{
			get
			{
				if (_dockPanel.ActiveDocument == null)
				{
					return null;
				}

				if (!(_dockPanel.ActiveDocument is Editor))
				{
					return null;
				}

				return _dockPanel.ActiveDocument as Editor;
			}
		}

		public IEnumerable<Editor> Documents
		{
			get
			{
				lock (_dockPanel.Documents)
				{
					return _dockPanel.Documents.Where(doc => doc is Editor).Cast<Editor>();
				}
			}
		}

		public MainForm MainForm
		{
			get
			{
				if (_dockPanel != null)
				{
					return (MainForm)_dockPanel.Parent;
				}

				return null;
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

		public static event EventHandler OnActiveDocumentChanged;

		private static void DockPanelOnActiveDocumentChanged(object sender, EventArgs eventArgs)
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

		public void LoadConfig()
		{
			try
			{
				DeserializeDockContent dockContent = GetContentFromPersistString;
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

		public IDockContent GetContentFromPersistString(string persistString)
		{
			if (persistString == typeof(OutputWindow).ToString())
			{
				return OutputWindow;
			}
			if (persistString == typeof(LabelList).ToString())
			{
				return LabelList;
			}
			if (persistString == typeof(ErrorList).ToString())
			{
				return ErrorList;
			}
			if (persistString == typeof(DebugPanel).ToString())
			{
				return DebugPanel;
			}
			if (persistString == typeof(CallStack).ToString())
			{
				return CallStack;
			}
			if (persistString == typeof(TrackingWindow).ToString())
			{
				return TrackWindow;
			}
			if (persistString == typeof(ProjectViewer).ToString())
			{
				return ProjectViewer;
			}

			string[] parsedStrings = persistString.Split(';');
			if (parsedStrings.Length != 6 || parsedStrings[0] != typeof(Editor).ToString() || !File.Exists(parsedStrings[1]))
			{
				return null;
			}
			return null;
			// TODO: fix
			/*Editor doc = DocumentService.OpenDocument(parsedStrings[1]);
			doc.SetPosition(
				int.Parse(parsedStrings[2]),
				int.Parse(parsedStrings[3]),
				int.Parse(parsedStrings[4]),
				int.Parse(parsedStrings[5]));
			return doc;*/
		}

		public void Invoke(Action action)
		{
			MainForm.Invoke(action);
		}

		public void InitPanels(ProjectViewer projectViewer, ErrorList errorList, TrackingWindow trackingWindow,
			DebugPanel debugPanel, CallStack callStack, LabelList labelList, OutputWindow outputWindow,
			FindAndReplaceForm findAndReplaceForm, FindResultsWindow findResults, MacroManager macroManager,
			BreakpointManagerWindow breakpointManagerWindow, StackViewer stackViewer)
		{
			if (HasBeenInited)
			{
				throw new Exception("Cannot init panels twice");
			}
			ProjectViewer = projectViewer;
			ErrorList = errorList;
			TrackWindow = trackingWindow;
			DebugPanel = debugPanel;
			CallStack = callStack;
			LabelList = labelList;
			OutputWindow = outputWindow;
			FindForm = findAndReplaceForm;
			FindResults = findResults;
			MacroManager = macroManager;
			BreakManagerWindow = breakpointManagerWindow;
			StackViewer = stackViewer;
			HasBeenInited = true;
		}

		#region IService

		public void DestroyService()
		{
			foreach (Editor editor in Documents.ToList())
			{
				try
				{
					editor.Dispose();
				}
				catch (Exception ex)
				{
					throw new Exception("Error destroying documents", ex);
				}
			}

			try
			{
				string dir = Path.GetDirectoryName(FileLocations.ConfigFile);
				if (dir != null && !Directory.Exists(dir))
				{
					if (MessageBox.Show("Directory '" + dir + "' does not exist. Would you like to create it?", "Directory does not exist", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) != DialogResult.Yes)
					{
						Properties.Settings.Default.configLoc = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "DockPanel.config");
					}
				}

				if (File.Exists(FileLocations.ConfigFile))
				{
					File.Delete(FileLocations.ConfigFile);
				}

				_dockPanel.SaveAsXml(FileLocations.ConfigFile);
				HasBeenInited = false;
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