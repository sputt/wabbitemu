using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using AvalonDock;
using ICSharpCode.AvalonEdit;
using Revsoft.Wabbitcode.Panels;
using System.Collections.Generic;

namespace Revsoft.Wabbitcode.Services
{
    public static class DockingService
    {
        static DockingManager dockManager;
        public static MainWindow MainWindow { get; private set; }

        public static IWabbitcodePanel ActivePanel { get { return (IWabbitcodePanel) dockManager.ActiveContent; } }
        public static ManagedContent ActiveContent { get { return dockManager.ActiveContent; } }
        public static Editor ActiveDocument { get { return (Editor) dockManager.ActiveDocument; } }

        public static WabbitcodeStatusBarService StatusBar { get; private set; }
        public static LabelList LabelList { get; private set; }
        public static ProjectViewer ProjectViewer { get; private set; }
        public static OutputWindow OutputWindow { get; private set; }
        public static ErrorList ErrorList { get; private set; }
        
        internal static void InitDocking(MainWindow main, DockingManager manager)
        {
            MainWindow = main;
            dockManager = manager;

            Application.Current.Resources["ThemeDictionary"] = new ResourceDictionary();
            ThemeFactory.ChangeTheme("dev2010");

            MainWindow.WindowState = PropertyService.GetWabbitcodeProperty("WindowState");
            MainWindow.Top = PropertyService.GetWabbitcodeProperty("Top");
            MainWindow.Left = PropertyService.GetWabbitcodeProperty("Left");
            MainWindow.Width = PropertyService.GetWabbitcodeProperty("Width");
            MainWindow.Height = PropertyService.GetWabbitcodeProperty("Height");

            dockManager.DeserializationCallback = new DockingManager.DeserializationCallbackHandler(DeserializationCallback);
        }

        internal static void DeserializationCallback(object s, DeserializationCallbackEventArgs e)
        {
            if (e.Name != "EditorDocument")
                return;
            e.Content = new Editor();
        }

        static readonly DependencyProperty ErrorListProperty =
                                DependencyProperty.Register("ErrorListVisible", typeof(ObservableCollection<Editor>),
                                    typeof(MainWindow), new UIPropertyMetadata(null));

        internal static void InitPanels(WabbitcodeStatusBar statusBar)
        {
            StatusBar = new WabbitcodeStatusBarService(statusBar);

            //Init Normal Panels
            LabelList = new LabelList();
            LabelList.Show(dockManager, AnchorStyle.Right);

            ProjectViewer = new ProjectViewer();
            ProjectViewer.Show(dockManager, AnchorStyle.Left);

            OutputWindow = new OutputWindow();
            OutputWindow.Show(dockManager, AnchorStyle.Bottom);

            ErrorList = new ErrorList();
            ErrorList.Show(dockManager, AnchorStyle.Bottom);

            if (File.Exists(WabbitcodePaths.DockConfig))
            {
#if !DEBUG
                try {
#endif
                dockManager.RestoreLayout(WabbitcodePaths.DockConfig);
#if !DEBUG
                } catch (Exception) { }
#endif
                return;
            }            
        }

        internal static void DestoryDocking()
        {
            PropertyService.SaveWabbitcodeProperty("WindowState", MainWindow.WindowState);
            PropertyService.SaveWabbitcodeProperty("Top", MainWindow.Top);
            PropertyService.SaveWabbitcodeProperty("Left", MainWindow.Left);
            PropertyService.SaveWabbitcodeProperty("Width", MainWindow.Width);
            PropertyService.SaveWabbitcodeProperty("Height", MainWindow.Height);
            PropertyService.Save();

            dockManager.SaveLayout(WabbitcodePaths.DockConfig);
        }

        public static void ShowError(string error)
        {
            MessageBox.Show(error, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        public static void ShowError(string error, Exception ex)
        {
            StringBuilder sb = new StringBuilder(error);
            sb.Append("\n");
            sb.Append(ex.ToString());
            MessageBox.Show(sb.ToString(), "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        internal static void ShowDockPanel(DockableContent doc)
        {
            doc.Show(dockManager);
        }

        internal static void ShowDockPanel(DocumentContent doc)
        {
            doc.Show(dockManager);
            doc.Activate();
        }

        public static ManagedContentCollection<DocumentContent> Documents { get { return dockManager.Documents; } }
    }
}
