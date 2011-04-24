using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Xml;
using AvalonDock;
using ICSharpCode.AvalonEdit.Highlighting;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Panels;
using Revsoft.Wabbitcode.Dialogs;
using Microsoft.Win32;

namespace Revsoft.Wabbitcode
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        readonly string[] args;
        public MainWindow(string[] args)
        {
            InitializeComponent();
            this.args = args;

            DataContext = this;
        }

        #region Startup
        public void HandleArgs(string[] args)
        {
            if (args.Length == 0)
                return;
            foreach (string file in args)
                if (File.Exists(file))
                    if (Path.GetExtension(file) == ".wcodeproj")
                    {
                        ProjectService.OpenProject(file);
                        return;
                    }
                    else
                    {
                        foreach (string arg in args)
                            try
                            {
                                if (string.IsNullOrEmpty(arg))
                                    break;
                                DocumentService.OpenDocument(arg);
                            }
                            catch (FileNotFoundException)
                            {
                                DockingService.ShowError("Error: File not found!");
                            }
                            catch (Exception ex)
                            {
                                DockingService.ShowError("Error in loading startup args", ex);
                            }
                    }
        }
        #endregion

        #region Form Methods
        private void Window_Closed(object sender, EventArgs e)
        {
            //Clean up and save panels and window pos
            DockingService.DestoryDocking();
        }

        private void Window_DragEnter(object sender, DragEventArgs e)
        {

        }

        private void Window_Initialized(object sender, EventArgs e)
        {
            //Init/reload panels and restore window pos
            DockingService.InitDocking(this, dockManager);
            //Init doc collection and highlighting
            DocumentService.InitDocuments();
            //Make any necessary directories
            WabbitcodePaths.InitPaths();
            //Make sure projects are ready to go
            ProjectService.InitProjects();
        }

        private void dockManager_Loaded(object sender, RoutedEventArgs e)
        {
            DockingService.InitPanels(StatusBar);

            //handle any arguments sent
            HandleArgs(args);
        }
        #endregion

        #region File Menu
        private void NewFile_Executed(object sender, RoutedEventArgs e)
        {
            DocumentService.CreateDocument("New Document");
        }

        private void NewProject_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            var dialog = new NewProjectDialog();
            dialog.ShowDialog();
        }

        private void OpenFile_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            DocumentService.OpenDocument();
        }

        private void OpenProject_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            var ofn = new OpenFileDialog()
            {
                AddExtension = true,
                CheckFileExists = true,
                DefaultExt = ".wcodeproj",
                Filter = "Project Files (*.wcodeproj)|*.wcodeproj|All Files(*.*)|*.*",
                FilterIndex = 0,
                InitialDirectory = WabbitcodePaths.ProjectDirectory,
                Multiselect = false,
                RestoreDirectory = true,
                Title = "Open Project",
            };
            if (ofn.ShowDialog() == true)
            {
                ProjectService.OpenProject(ofn.FileName);
            }
        }

        private void SaveFile_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.SaveFile();
        }

        private void SaveAll_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            foreach(Editor doc in DockingService.Documents)
                doc.SaveFile();
        }

        private void Close_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.Close();
        }
        #endregion

        #region Edit Menu
        private void Undo_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActivePanel != null)
                DockingService.ActivePanel.Undo();
        }

        private void Redo_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActivePanel != null)
                DockingService.ActivePanel.Redo();
        }

        private void Cut_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActivePanel != null)
                DockingService.ActivePanel.Cut();
        }

        private void Copy_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActivePanel != null)
                DockingService.ActivePanel.Copy();
        }

        private void Paste_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActivePanel != null)
                DockingService.ActivePanel.Paste();
        }

        private void SelectAll_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            if (DockingService.ActiveDocument != null)
                DockingService.ActiveDocument.SelectAll();
        }
        #endregion

        private void Assemble_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            string filePath = DockingService.ActiveDocument.FilePath;
            string outputPath = Path.ChangeExtension(filePath, ".8xk");
            AssemblerService.AssembleFile(filePath, outputPath, true);
        }

        private void About_Executed(object snder, ExecutedRoutedEventArgs e)
        {
            var aboutBox = new AboutBox();
            aboutBox.ShowDialog();
        }
    }
}
