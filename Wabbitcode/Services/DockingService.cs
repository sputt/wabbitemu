using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.Resource;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services
{
    public static class DockingService
    {
        #region Private Members
        private static BreakpointManager breakManager;
        private static CallStack callStack;
        private static DebugPanel debugPanel;
        private static DirectoryViewer directoryViewer;
        private static DockPanel dockPanel;
        private static ErrorList errorList;
        private static FindAndReplaceForm findForm;
        private static FindResultsWindow findResults;
        private static bool initialized;
        private static LabelList labelList;
        private static MacroManager macroManager;
        private static OutputWindow outputWindow;
        private static ProjectViewer projectViewer;
        private static StackViewer stackViewer;
        private static TrackingWindow trackWindow;
        #endregion

        #region Public Properties
        public static IDockContent ActiveContent
        {
            get
            {
                if (dockPanel == null)
                {
                    return null;
                }

                return dockPanel.ActiveContent;
            }
        }

        public static NewEditor ActiveDocument
        {
            get
            {
                if (dockPanel.ActiveDocument == null)
                {
                    return null;
                }

                if (!(dockPanel.ActiveDocument is NewEditor))
                {
                    return null;
                }

                return dockPanel.ActiveDocument as NewEditor;
            }
        }

        public static BreakpointManager BreakManager
        {
            get
            {
                return breakManager;
            }
        }

        public static CallStack CallStack
        {
            get
            {
                return callStack;
            }
        }

        public static DebugPanel DebugPanel
        {
            get
            {
                return debugPanel;
            }
        }

        public static DirectoryViewer DirectoryViewer
        {
            get
            {
                return directoryViewer;
            }
        }

        public static DockPanel DockPanel
        {
            get
            {
                return dockPanel;
            }
            set
            {
                dockPanel = value;
            }
        }

        public static IEnumerable<NewEditor> Documents
        {
            get
            {
                lock (dockPanel.Documents)
                {
                    return dockPanel.Documents.Where(doc => doc is NewEditor).Cast<NewEditor>();
                }
            }
        }

        public static ErrorList ErrorList
        {
            get
            {
                return errorList;
            }
        }

        public static FindAndReplaceForm FindForm
        {
            get
            {
                return findForm;
            }
        }

        public static FindResultsWindow FindResults
        {
            get
            {
                return findResults;
            }
        }

        public static bool HasBeenInited
        {
            get
            {
                return initialized;
            }
        }

        public static LabelList LabelList
        {
            get
            {
                return labelList;
            }
        }

        public static MacroManager MacroManager
        {
            get
            {
                return macroManager;
            }
        }

        public static MainFormRedone MainForm
        {
            get
            {
                if (dockPanel != null)
                {
                    return (MainFormRedone)dockPanel.Parent;
                }

                return null;
            }
        }

        public static OutputWindow OutputWindow
        {
            get
            {
                return outputWindow;
            }
        }

        public static ProjectViewer ProjectViewer
        {
            get
            {
                return projectViewer;
            }
        }

        public static StackViewer StackViewer
        {
            get
            {
                return stackViewer;
            }
        }

        public static TrackingWindow TrackWindow
        {
            get
            {
                return trackWindow;
            }
        }
        #endregion

        public static void HideDockPanel(DockContent panel)
        {
            if (panel == null)
            {
                return;
            }

            panel.Hide();
        }

        public static void ShowDockPanel(DockContent panel)
        {
            if (panel == null)
            {
                return;
            }

            panel.Show(dockPanel);
        }

        public static void ShowError(string error)
        {
            MessageBox.Show(error, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        public static void ShowError(string error, Exception ex)
        {
            StringBuilder sb = new StringBuilder(error);
            sb.Append("\nReason: ");
            sb.Append(ex.Message.ToString());
            MessageBox.Show(sb.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        internal static void Destroy()
        {
            foreach (NewEditor editor in Documents.ToList())
            {
                try
                {
                    editor.Dispose();
                }
                catch (Exception ex)
                {
                    ShowError("Error destroying documents", ex);
                }
            }

            try
            {
                string dir = Path.GetDirectoryName(FileLocations.ConfigFile);
                if (!Directory.Exists(dir))
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

                dockPanel.SaveAsXml(FileLocations.ConfigFile);
                initialized = false;
            }
            catch (Exception ex)
            {
                ShowError("Error saving DockPanel.config file!", ex);
            }
        }

        internal static void InitDocking(DockPanel dockingPanel)
        {
            dockPanel = dockingPanel;

            // Theme
            dockPanel.Theme = new VS2012LightTheme();
            ToolStripManager.Renderer = new VS2012ToolStripRenderer();
        }

        internal static void InitPanels()
        {
            try
            {
                projectViewer = new ProjectViewer();
                directoryViewer = new DirectoryViewer();
                errorList = new ErrorList();
                trackWindow = new TrackingWindow(MainForm);
                debugPanel = new DebugPanel(MainForm);
                callStack = new CallStack(MainForm);
                labelList = new LabelList();
                outputWindow = new OutputWindow();
                findForm = new FindAndReplaceForm();
                findResults = new FindResultsWindow();
                macroManager = new MacroManager();
                breakManager = new BreakpointManager(MainForm);
                stackViewer = new StackViewer(MainForm);
                initialized = true;
            }
            catch (Exception ex)
            {
                ShowError("Error in InitPanels", ex);
            }
        }

        internal static void LoadConfig()
        {
            try
            {
                DeserializeDockContent dockContent = new DeserializeDockContent(GetContentFromPersistString);
                if (File.Exists(FileLocations.ConfigFile))
                {
                    dockPanel.LoadFromXml(FileLocations.ConfigFile, dockContent);
                }
            }
            catch (Exception ex)
            {
                ShowError("Error Loading the DockPanel Config File", ex);
            }
        }

        private static IDockContent GetContentFromPersistString(string persistString)
        {
            if (persistString == typeof(OutputWindow).ToString())
            {
                return outputWindow;
            }
            if (persistString == typeof(LabelList).ToString())
            {
                return labelList;
            }
            if (persistString == typeof(ErrorList).ToString())
            {
                return errorList;
            }
            if (persistString == typeof(DebugPanel).ToString())
            {
                return debugPanel;
            }
            if (persistString == typeof(CallStack).ToString())
            {
                return callStack;
            }
            if (persistString == typeof(TrackingWindow).ToString())
            {
                return trackWindow;
            }
            if (persistString == typeof(ProjectViewer).ToString())
            {
                return DockingService.ProjectViewer;
            }
            if (persistString == typeof(DirectoryViewer).ToString())
            {
                return DockingService.DirectoryViewer;
            }

            string[] parsedStrings = persistString.Split(';');
            if (parsedStrings.Length != 6 || parsedStrings[0] != typeof(NewEditor).ToString() || !File.Exists(parsedStrings[1]))
            {
                return null;
            }
            NewEditor doc = DocumentService.OpenDocument(parsedStrings[1]);
            doc.SetPosition(
                int.Parse(parsedStrings[2]),
                int.Parse(parsedStrings[3]),
                int.Parse(parsedStrings[4]),
                int.Parse(parsedStrings[5]));
            return doc;
        }

        internal static DialogResult ShowMessageBox(Form parent, string text, string caption,
                        MessageBoxButtons messageBoxButtons, MessageBoxIcon messageBoxIcon)
        {
            if (parent.InvokeRequired)
            {
                Func<DialogResult> invokeBox = () => ShowMessageBox(parent, text, caption, messageBoxButtons, messageBoxIcon);
                return (DialogResult)parent.Invoke(invokeBox);
            }
            else
            {
                return MessageBox.Show(parent, text, caption, messageBoxButtons, messageBoxIcon);
            }
        }
    }
}