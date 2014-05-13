using System;
using System.Collections.Generic;
using System.IO;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class FileService : IFileService
    {
        private readonly Dictionary<FilePath, Tuple<string, DateTime>> _cachedFiles = new Dictionary<FilePath, Tuple<string, DateTime>>();
        private readonly Dictionary<FilePath, IDocument> _openDocuments = new Dictionary<FilePath, IDocument>();

        public FileService(IDockingService dockingService)
        {
            dockingService.DocumentWindowAdded += DockingService_DocumentWindowAdded;
            dockingService.DocumentWindowRemoved += DockingService_DocumentWindowRemoved;
        }

        /// <summary>
        /// Returns the string contents of a line from a given file
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="lineNumber"></param>
        /// <returns>The line in the given file</returns>
        public string GetLine(FilePath fileName, int lineNumber)
        {
            // expected 1-index, convert to 0-index
            lineNumber--;
            if (lineNumber < 0)
            {
                return null;
            }

            string fileText = GetFileText(fileName);
            string[] lines = fileText.Split('\n');
            return lines[lineNumber];
        }

        /// <summary>
        /// Returns the string concents of a given file
        /// </summary>
        /// <param name="fileName"></param>
        /// <returns>The text of a file</returns>
        public string GetFileText(FilePath fileName)
        {
            StreamReader reader = null;
            try
            {
                reader = new StreamReader(fileName);
                return GetFileText(fileName, reader);
            }
            catch (IOException)
            {
                return string.Empty;
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                }
            }
        }

        public IDocument GetOpenDocument(FilePath fileName)
        {
            if (_openDocuments.ContainsKey(fileName))
            {
                return _openDocuments[fileName];
            }

            new OpenFileAction(fileName).Execute();
            return _openDocuments[fileName];
        }

        public void SetFileText(FilePath fileName, string text)
        {
            var tuple = new Tuple<string, DateTime>(text, DateTime.Now);
            if (_cachedFiles.ContainsKey(fileName))
            {
                if (_cachedFiles[fileName].Item1.Equals(text))
                {
                    return;
                }

                _cachedFiles[fileName] = tuple;
            }
            else
            {
                _cachedFiles.Add(fileName, tuple);
            }

            StreamWriter writer = null;
            try
            {
                writer = new StreamWriter(fileName);
                writer.Write(text);
            }
            catch (IOException)
            {
            }
            finally
            {
                if (writer != null)
                {
                    writer.Flush();
                    writer.Close();
                }
            }
        }

        public string GetFileText(FilePath fileName, TextReader reader)
        {
            string text;
            if (_openDocuments.ContainsKey(fileName))
            {
                IDocument document = _openDocuments[fileName];
                return document.TextContent;
            }

            if (_cachedFiles.ContainsKey(fileName))
            {
                text = _cachedFiles[fileName].Item1;
                DateTime currentWriteTime = _cachedFiles[fileName].Item2;
                DateTime lastWriteTime = File.GetLastWriteTime(fileName);
                if (currentWriteTime >= lastWriteTime)
                {
                    return text;
                }

                lock (_cachedFiles)
                {
                    _cachedFiles.Remove(fileName);
                }
            }

            try
            {
                text = reader.ReadToEnd();
            }
            catch (Exception)
            {
                return null;
            }

            DateTime time = File.GetLastWriteTime(fileName);
            lock (_cachedFiles)
            {
                _cachedFiles.Add(fileName, new Tuple<string, DateTime>(text, time));
            }
            return text;
        }

        private void DockingService_DocumentWindowRemoved(object sender, DockContentEventArgs e)
        {
            ITextEditor editor = e.Content as ITextEditor;
            if (editor == null)
            {
                return;
            }

            _openDocuments.Remove(editor.FileName);
        }

        private void DockingService_DocumentWindowAdded(object sender, DockContentEventArgs e)
        {
            ITextEditor editor = e.Content as ITextEditor;
            if (editor == null)
            {
                return;
            }

            _openDocuments.Add(editor.FileName, editor.Document);
        }
    }
}