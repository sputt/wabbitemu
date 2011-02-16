using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Docking;
using System.IO;
using Revsoft.Wabbitcode.Docking_Windows;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;

namespace Revsoft.Wabbitcode.Services
{
	public static class DockingService
	{
		private static DockPanel dockPanel;
		public static DockPanel DockPanel
		{
			get { return dockPanel; }
			set { dockPanel = value; }
		}

		public static MainFormRedone MainForm
		{
			get { return (MainFormRedone)dockPanel.Parent; }
		}

		private static IList<IDockContent> documents = new List<IDockContent>();
		public static IEnumerable<IDockContent> Documents
		{
			get
			{
				documents.Clear();
				if (dockPanel == null)
					return documents;
				foreach (IDockContent doc in dockPanel.Documents)
					if (doc.GetType() == typeof(newEditor))
						documents.Add(doc);
				return documents;
			}
		}

		public static newEditor ActiveDocument
		{
			get
			{
				if (dockPanel.ActiveDocument == null)
					return null;
				if (dockPanel.ActiveDocument.GetType() != typeof(newEditor))
					return null;
				return dockPanel.ActiveDocument as newEditor;
			}
		}

		public static IDockContent ActiveContent
		{
			get
			{
				if (dockPanel == null)
					return null;
				return dockPanel.ActiveContent;
			}
		}

		private static ProjectViewer projectViewer;
		public static ProjectViewer ProjectViewer
		{
			get { return projectViewer; }
		}

		private static DirectoryViewer directoryViewer;
		public static DirectoryViewer DirectoryViewer
		{
			get { return directoryViewer; }
		}

		private static DebugPanel debugPanel;
		public static DebugPanel DebugPanel
		{
			get { return debugPanel; }
		}

		private static TrackingWindow trackWindow;
		public static TrackingWindow TrackWindow
		{
			get { return trackWindow; }
		}

		private static ErrorList errorList;
		public static ErrorList ErrorList
		{
			get { return errorList; }
		}

		private static CallStack callStack;
		public static CallStack CallStack
		{
			get { return callStack; }
		}

		private static StackViewer stackViewer;
		public static StackViewer StackViewer
		{
			get { return stackViewer; }
		}

		private static OutputWindow outputWindow;
		public static OutputWindow OutputWindow
		{
			get { return outputWindow; }
		}

		private static LabelList labelList;
		public static LabelList LabelList
		{
			get { return labelList; }
		}

		private static FindAndReplaceForm findForm;
		public static FindAndReplaceForm FindForm
		{
			get { return findForm; }
		}

		private static FindResultsWindow findResults;
		public static FindResultsWindow FindResults
		{
			get { return findResults; }
		}

		private static MacroManager macroManager;
		public static MacroManager MacroManager
		{
			get { return macroManager; }
		}

		private static BreakpointManager breakManager;
		public static BreakpointManager BreakManager
		{
			get { return breakManager; }
		}

        private static bool initialized;
        public static bool HasBeenInited
        {
            get { return initialized; }
        }

		public static void ShowDockPanel(DockContent panel)
		{
			if (panel == null)
				return;
			panel.Show(dockPanel);
		}

		public static void HideDockPanel(DockContent panel)
		{
			if (panel == null)
				return;
			panel.Hide();
		}

		internal static void InitPanels()
		{
#if !DEBUG
            try
            {
#endif
                projectViewer = new ProjectViewer();
                directoryViewer = new DirectoryViewer();
                errorList = new ErrorList();
                trackWindow = new TrackingWindow();
                debugPanel = new DebugPanel();
                callStack = new CallStack();
                labelList = new LabelList();
                outputWindow = new OutputWindow();
                findForm = new FindAndReplaceForm();
                findResults = new FindResultsWindow();
                macroManager = new MacroManager();
                breakManager = new BreakpointManager();
                stackViewer = new StackViewer();
                initialized = true;
#if !DEBUG
            }
            catch (Exception ex)
            {
                ShowError("Error in InitPanels", ex);
            }
#endif
		}

		internal static void InitDocking(DockPanel dockingPanel)
		{
			dockPanel = dockingPanel;
		}

		internal static void LoadConfig()
		{
			DeserializeDockContent dockContent = new DeserializeDockContent(GetContentFromPersistString);
#if !DEBUG
            try
			{
#endif
			if (File.Exists(FileLocations.ConfigFile))
				dockPanel.LoadFromXml(FileLocations.ConfigFile, dockContent);
#if !DEBUG
			}
			catch (Exception ex)
			{
                ShowError("Error Loading the DockPanel Config File", ex);
			}
#endif
        }

		private static IDockContent GetContentFromPersistString(string persistString)
		{
			if (persistString == typeof(OutputWindow).ToString())
				return outputWindow;
			if (persistString == typeof(LabelList).ToString())
				return labelList;
			if (persistString == typeof(ErrorList).ToString())
				return errorList;
			if (persistString == typeof(DebugPanel).ToString())
				return debugPanel;
			if (persistString == typeof(CallStack).ToString())
				return callStack;
			if (persistString == typeof(TrackingWindow).ToString())
				return trackWindow;
			if (persistString == typeof(ProjectViewer).ToString())
				return DockingService.ProjectViewer;
			if (persistString == typeof(DirectoryViewer).ToString())
				return DockingService.DirectoryViewer;

			string[] parsedStrings = persistString.Split(';');
			if (parsedStrings.Length != 6 || parsedStrings[0] != typeof(newEditor).ToString() || !File.Exists(parsedStrings[1]))
				return null;
            newEditor doc = DocumentService.OpenDocument(parsedStrings[1]);
            doc.SetPosition(int.Parse(parsedStrings[2]), int.Parse(parsedStrings[3]),
                                                        int.Parse(parsedStrings[4]), int.Parse(parsedStrings[5]));
            return doc;
		}

		internal static void Destroy()
		{
#if !DEBUG
			try
			{
#endif
				if (File.Exists(FileLocations.ConfigFile))
					File.Delete(FileLocations.ConfigFile);
				dockPanel.SaveAsXml(FileLocations.ConfigFile);
                initialized = false;
#if !DEBUG
			}
			catch (Exception ex)
			{
                ShowError("Error saving DockPanel.config file!", ex);
			}
#endif
		}

		public static DialogResult RequestDialog(string text, string caption, MessageBoxButtons buttons, MessageBoxIcon icon)
		{
			return DialogResult.None;
		}

        public static void ShowError(string error)
        {
            MessageBox.Show(error, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        public static void ShowError(string error, Exception ex)
        {
            StringBuilder sb = new StringBuilder(error);
            sb.Append("\n");
            sb.Append(ex.ToString());
            MessageBox.Show(sb.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
    }
}
