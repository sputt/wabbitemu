using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using AvalonDock;
using ICSharpCode.AvalonEdit;
using Microsoft.Win32;
using System.IO;
using Revsoft.Wabbitcode.Panels;
using ICSharpCode.AvalonEdit.Highlighting;
using System.Collections.ObjectModel;
using System.Windows;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services
{
    public static class DocumentService
    {
        public static ObservableCollection<Editor> OpenDocuments
        {
            get { return (ObservableCollection<Editor>)DockingService.MainWindow.GetValue(DocumentsProperty); }
            set { DockingService.MainWindow.SetValue(DocumentsProperty, value); }
        }

        public static readonly DependencyProperty DocumentsProperty =
                                DependencyProperty.Register("OpenDocuments", typeof(ObservableCollection<Editor>),
                                    typeof(MainWindow), new UIPropertyMetadata(null));

        internal static void InitDocuments()
        {
            OpenDocuments = new ObservableCollection<Editor>();
            InitHighlighting();
        }

        internal static Editor CreateDocument(string title)
        {
            var editor = new Editor() { Title = title };
            OpenDocuments.Add(editor);
            editor.Activate();
            return editor;
        }

        internal static void OpenDocument()
        {
            var openFileDialog = new OpenFileDialog()
            {
                CheckFileExists = true,
                DefaultExt = "*.asm",
                Filter = "All Know File Types | *.asm; *.inc; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
                            "Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|Project Files (*.wcodeproj)" +
                           "|*.wcodeproj|All Files(*.*)|*.*",
                FilterIndex = 0,
                RestoreDirectory = true,
                Multiselect = true,
                Title = "Open File",
            };
            var result = openFileDialog.ShowDialog();
            if (result == false)
                return;
            foreach (string fileName in openFileDialog.FileNames)
            {
                string extCheck = Path.GetExtension(fileName).ToLower();
                if (extCheck == ".wcodeproj")
                    ProjectService.OpenProject(fileName);
                else
                    OpenDocument(fileName);
            }
        }

        internal static Editor OpenDocument(string filename)
        {
#if !DEBUG
            try
            {
#endif
                var doc = new Editor();
                OpenDocuments.Add(doc);
                //DockingService.StatusBar.ShowProgress();
                //DockingService.StatusBar.SetProgress(.1, "Open", OperationStatus.Error);
                OpenDocument(doc, filename);
                //DockingService.StatusBar.SetProgress(.9, "Open", OperationStatus.Error);
                //DockingService.StatusBar.HideProgress();
                DockingService.ShowDockPanel(doc);
                return doc;
#if !DEBUG
            }
            catch (Exception ex)
            {
                StringBuilder builder = new StringBuilder();
                builder.Append("Error opening file ");
                builder.AppendLine(filename);
                builder.Append(ex);
                DockingService.ShowError(builder.ToString());
                return null;
            }
#endif
        }

        internal static void OpenDocument(Editor doc, string filename)
        {
            doc.OpenFile(filename);
            ParserService.ParseFile(filename);
            RecentFileService.AddRecentFile(filename);
            RecentFileService.SaveRecentFileList();
            RecentFileService.GetRecentFiles();
        }

        internal static void InitHighlighting()
        {
            IHighlightingDefinition asmHighlighting;
            using (Stream s = typeof(MainWindow).Assembly.GetManifestResourceStream("Revsoft.Wabbitcode.Resources.Z80Asm.xshd"))
            {
                if (s == null)
                    throw new InvalidOperationException("Could not find embedded resource");
                using (System.Xml.XmlReader reader = new System.Xml.XmlTextReader(s))
                {
                    asmHighlighting = ICSharpCode.AvalonEdit.Highlighting.Xshd.HighlightingLoader.Load(reader, HighlightingManager.Instance);
                }
            }
            // and register it in the HighlightingManager
            HighlightingManager.Instance.RegisterHighlighting("Z80 Asm", new string[] { ".inc", ".z80", ".asm" }, asmHighlighting);
        }
    }
}
