namespace Revsoft.Wabbitcode.Services
{
    using System;
    using System.Collections.Generic;
    using System.Drawing;
    using System.IO;
    using System.Linq;
    using System.Text;
    using System.Windows.Forms;

    using Revsoft.TextEditor;
    using Revsoft.TextEditor.Document;
    using Revsoft.Wabbitcode.Classes;
    using Revsoft.Wabbitcode.Properties;
    using Revsoft.Wabbitcode.Services.Parser;
    using Revsoft.Wabbitcode.Services.Debugger;

    public static class DocumentService
    {
        private static int debugIndex;
        private static List<ListFileKey> highlights = new List<ListFileKey>();
        private static int recentFileIndex = 0;

        /// <summary>
        /// Each string is the path to a recently opened file. Is also stored in properties as a big long string.
        /// </summary>
        private static string[] recentFileList = new string[10];

        public enum FixableErrorType
        {
            RelativeJump,
        }

        public static NewEditor ActiveDocument
        {
            get
            {
                return DockingService.ActiveDocument;
            }
        }

        public static string ActiveFileName
        {
            get
            {
                if (ActiveDocument == null)
                {
                    return null;
                }
                return ActiveDocument.FileName;
            }
        }

        public static bool InternalSave
        {
            get;
            set;
        }

        public static NewEditor CreateNewDocument()
        {
            NewEditor doc = new NewEditor(DockingService.MainForm) { Text = "New Document", TabText = "New Document" };
            doc.SetHighlighting("Z80 Assembly");
            return doc;
        }

        public static NewEditor GotoFile(string file)
        {
            NewEditor editorBox = ActiveDocument;
            if (editorBox == null)
            {
                return OpenDocument(file);
            }

            if (string.Equals(file, ActiveFileName, StringComparison.OrdinalIgnoreCase))
            {
                return ActiveDocument;
            }

            foreach (NewEditor child in DockingService.Documents)
            {
                if (!string.IsNullOrEmpty(child.FileName) && string.Equals(child.FileName, file, StringComparison.OrdinalIgnoreCase))
                {
                    child.Show();
                    return child;
                }
            }

            return OpenDocument(file);
        }

        public static void GotoLabel(IParserData item)
        {
            ParserInformation info = item.Parent;
            string file = info.SourceFile;
            NewEditor child = GotoFile(file);
            child.ScrollToOffset(item.Location.Offset);
        }

        public static void GotoLine(int scrollToLine)
        {
            ActiveDocument.ScrollToLine(scrollToLine);
        }

        public static void GotoLine(string file, int scrollToLine)
        {
            NewEditor child = GotoFile(file);
            child.ScrollToLine(scrollToLine);
        }

        public static void HighlightDebugLine(int newLineNumber)
        {
            HighlightLine(newLineNumber, Color.Yellow);
            debugIndex = highlights.Count - 1;
        }

        public static void HighlightLine(int newLineNumber, Color foregroundColor)
        {
            ListFileKey value = new ListFileKey(ActiveFileName, newLineNumber);
            highlights.Add(value);
            ActiveDocument.HighlightLine(newLineNumber, foregroundColor);
        }

        public static void OpenDocument()
        {
            var openFileDialog = new OpenFileDialog()
            {
                CheckFileExists = true,
                DefaultExt = "*.asm",
                Filter = "All Know File Types | *.asm; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
                         " Assembly Files (*.z80)|*.z80 | Include Files (*.inc)|*.inc | Project Files (*.wcodeproj)"+
                         "|*.wcodeproj|All Files(*.*)|*.*",
                         FilterIndex = 0,
                         RestoreDirectory = true,
                         Multiselect = true,
                         Title = "Open File",
            };
            if (openFileDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            foreach (var fileName in openFileDialog.FileNames)
            {
                string extCheck = Path.GetExtension(fileName);
                if (string.Equals(extCheck, ".wcodeproj", StringComparison.OrdinalIgnoreCase))
                {
                    ProjectService.OpenProject(fileName);
                }
                else
                {
                    OpenDocument(fileName);
                }
            }
        }

        public static void RemoveDebugHighlight()
        {
            if (ActiveDocument == null || highlights.Count == 0 || debugIndex < 0)
            {
                return;
            }
            ListFileKey key = highlights[debugIndex];
            GotoFile(key.FileName).RemoveDebugHighlight(key.LineNumber);
            highlights.Remove(key);
            debugIndex = -1;
        }

        public static void RemoveHighlight(int index)
        {
            if (ActiveDocument == null || highlights.Count == 0)
            {
                return;
            }
            ListFileKey key = highlights[index];
            GotoFile(key.FileName).RemoveHighlight(key.LineNumber);
            highlights.Remove(key);
            if (index <= debugIndex)
            {
                debugIndex--;
            }
        }

        /// <summary>
        /// Save active document
        /// </summary>
        public static void SaveDocument()
        {
            if (ActiveDocument == null)
            {
                return;
            }
            SaveDocument(ActiveDocument);
        }

        /// <summary>
        /// Save document as dialog. Also called if doc has never been saved.
        /// </summary>
        /// <returns></returns>
        public static bool SaveDocumentAs()
        {
            if (ActiveDocument == null)
            {
                return false;
            }
            SaveFileDialog saveFileDialog = new SaveFileDialog()
            {
                DefaultExt = "asm",
                RestoreDirectory = true,
                Filter = "All Know File Types | *.asm; *.z80| Assembly Files (*.asm)|*.asm|Z80" +
                         " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
                         FilterIndex = 0,
                         Title = "Save File As"
            };
            DialogResult saved = saveFileDialog.ShowDialog();
            saveFileDialog.Dispose();
            if (saved != DialogResult.OK)
            {
                return false;
            }
            string fileName = saveFileDialog.FileName;
            if (fileName == "")
            {
                return false;
            }
            ActiveDocument.FileName = fileName;
            ActiveDocument.Text = fileName;
            ActiveDocument.SaveFile();
            return true;
        }

        internal static int GetDebugLine()
        {
            return highlights[debugIndex].LineNumber;
        }

        /// <summary>
        /// This loads the recent file list from Properties and adds it to the recent file menu.
        /// </summary>
        internal static void GetRecentFiles()
        {
            DockingService.MainForm.ClearRecentItems();
            string line = Settings.Default.recentFiles;
            string[] list = line.Split('\n');
            foreach (string file in list)
            {
                if (string.IsNullOrEmpty(file.Trim()))
                {
                    continue;
                }
                DockingService.MainForm.AddRecentItem(file);
            }
        }

        internal static void GotoCurrentDebugLine()
        {
            GotoFile(highlights[debugIndex].FileName);
        }

        internal static void HighlightCall()
        {
            GotoFile(highlights[debugIndex].FileName).HighlightCall(highlights[debugIndex].LineNumber);
        }

        internal static NewEditor OpenDocument(string filename)
        {
#if !DEBUG
            try
            {
#endif
                NewEditor doc = new NewEditor(DockingService.MainForm);
                OpenDocument(doc, filename);
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

        internal static void OpenDocument(NewEditor doc, string filename)
        {
            DockingService.MainForm.IncrementProgress(10);

            doc.Text = Path.GetFileName(filename);
            doc.TabText = Path.GetFileName(filename);
            doc.ToolTipText = filename;
            doc.MdiParent = DockingService.MainForm;
            doc.OpenFile(filename);
            AddRecentFile(filename);
            SaveRecentFileList();
            GetRecentFiles();
            DockingService.MainForm.IncrementProgress(90);
            DockingService.MainForm.HideProgressBar();
            DockingService.ShowDockPanel(doc);
        }

        internal static void SaveDocument(NewEditor doc)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog()
            {
                DefaultExt = "asm",
                RestoreDirectory = true,
                Filter = "All Know File Types | *.asm; *.z80| Assembly Files (*.asm)|*.asm|Z80" +
                         " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
                         FilterIndex = 0,
                         Title = "Save File As"
            };
            if (ProjectService.ProjectWatcher != null)
            {
                ProjectService.ProjectWatcher.EnableRaisingEvents = false;
            }
            if (string.IsNullOrEmpty(doc.FileName))
            {
                if (saveFileDialog.ShowDialog() != DialogResult.OK)
                {
                    return;
                }
                if (saveFileDialog.FileName == "")
                {
                    return;
                }
                doc.FileName = saveFileDialog.FileName;
            }

            if (!string.IsNullOrEmpty(doc.FileName))
            {
                doc.SaveFile();
            }
            if (ProjectService.ProjectWatcher != null)
            {
                ProjectService.ProjectWatcher.EnableRaisingEvents = true;
            }
        }

        /// <summary>
        /// Adds a string to the recent file list
        /// </summary>
        /// <param name="filename">Full path of the file to save to the list</param>
        private static void AddRecentFile(string filename)
        {
            if (!recentFileList.Contains(filename))
            {
                if (recentFileIndex == recentFileList.Length)
                {
                    Array.ConstrainedCopy(recentFileList, 1, recentFileList, 0, recentFileList.Length - 1);
                    recentFileIndex--;
                }

                recentFileList[recentFileIndex++] = filename;
            }
        }

        /// <summary>
        /// This takes the string array of recent files, joins them into a large string and saves it in Properties.
        /// </summary>
        private static void SaveRecentFileList()
        {
            StringBuilder list = new StringBuilder();
            foreach (string file in recentFileList)
            {
                list.Append(file + "\n");
            }
            Settings.Default.recentFiles = list.ToString();
        }
    }
}