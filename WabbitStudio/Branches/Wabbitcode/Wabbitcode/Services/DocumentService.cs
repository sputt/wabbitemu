using System.Collections.Specialized;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency(typeof (IDockingService))]
    [Obsolete]
	internal class DocumentService : IDocumentService
	{
		private int _debugIndex;
		private readonly List<DocumentLocation> _highlights = new List<DocumentLocation>();
		private int _recentFileIndex;
		private readonly IDockingService _dockingService;

		/// <summary>
		/// Each string is the path to a recently opened file.
		/// </summary>
		private readonly string[] _recentFileList = new string[10];

	    private Editor ActiveDocument
		{
			get { return _dockingService.ActiveDocument as Editor; }
		}

		public Editor CreateNewDocument()
		{
			Editor doc = new Editor
			{
				Text = "New Document",
				TabText = "New Document"
			};
			return doc;
		}

		public Editor GotoFile(string file)
		{
		    return OpenDocument(file);
		}

		public void GotoLabel(IParserData item)
		{
			ParserInformation info = item.Parent;
			string file = info.SourceFile;
			Editor child = GotoFile(file);
			child.GotoLine(item.Location.Line);
		    child.Focus();
		}

		public void GotoLine(string file, int scrollToLine)
		{
			Editor child = GotoFile(file);
			child.GotoLine(scrollToLine);
		}

		public void HighlightDebugLine(int newLineNumber)
		{
			HighlightLine(newLineNumber, Color.Yellow);
			_debugIndex = _highlights.Count - 1;
		}

		private void HighlightLine(int newLineNumber, Color foregroundColor)
		{
			DocumentLocation value = new DocumentLocation(ActiveDocument.FileName, newLineNumber);
			_highlights.Add(value);
			//ActiveDocument.HighlightLine(newLineNumber, foregroundColor, "Debug Highlight");
		}

		public void RemoveDebugHighlight()
		{
			if (ActiveDocument == null || _highlights.Count == 0 || _debugIndex < 0)
			{
				return;
			}
			RemoveHighlight(_debugIndex, "Debug Highlight");
		    _debugIndex = -1;
		}

		private void RemoveHighlight(int index, string tag)
		{
			if (ActiveDocument == null || _highlights.Count == 0)
			{
				return;
			}

			DocumentLocation key = _highlights[index];
			GotoFile(key.FileName).RemoveHighlight(key.LineNumber, tag);
			_highlights.Remove(key);
			if (index <= _debugIndex)
			{
				_debugIndex--;
			}
		}

		public Editor OpenDocument(string filename)
		{
            var child = _dockingService.Documents.OfType<Editor>().SingleOrDefault(e => FileOperations.CompareFilePath(e.FileName, filename));
            if (child != null)
            {
                child.Show();
                return child;
            }

            Editor doc = new Editor();
            OpenDocument(doc, filename);
            return doc;
		}

	    private void OpenDocument(Editor doc, string filename)
		{
			doc.Text = Path.GetFileName(filename);
			doc.TabText = Path.GetFileName(filename);
			doc.ToolTipText = filename;
			doc.OpenFile(filename);
			AddRecentFile(filename);
			SaveRecentFileList();
			_dockingService.ShowDockPanel(doc);
		}

		/// <summary>
		/// Adds a string to the recent file list
		/// </summary>
		/// <param name="filename">Full path of the file to save to the list</param>
		private void AddRecentFile(string filename)
		{
		    if (_recentFileList.Contains(filename))
		    {
		        return;
		    }

		    if (_recentFileIndex == _recentFileList.Length)
		    {
		        Array.ConstrainedCopy(_recentFileList, 1, _recentFileList, 0, _recentFileList.Length - 1);
		        _recentFileIndex--;
		    }

		    _recentFileList[_recentFileIndex++] = filename;
		}

		/// <summary>
		/// This takes the string array of recent files, joins them into a large string and saves it in Properties.
		/// </summary>
		private void SaveRecentFileList()
		{
            if (Settings.Default.RecentFiles == null)
            {
                Settings.Default.RecentFiles = new StringCollection();
            }

            Settings.Default.RecentFiles.Clear();
            Settings.Default.RecentFiles.AddRange(_recentFileList);
		}

		#region IService

		public void InitService(params object[] objects)
		{
            FileTypeMethodFactory.RegisterFileType(".asm", path => OpenDocument(path) != null);
            FileTypeMethodFactory.RegisterFileType(".z80", path => OpenDocument(path) != null);
            FileTypeMethodFactory.RegisterFileType(".inc", path => OpenDocument(path) != null);
            FileTypeMethodFactory.RegisterDefaultHandler(path => OpenDocument(path) != null);
		}

		public void DestroyService()
		{
		}

		public DocumentService(IDockingService dockingService)
		{
			_dockingService = dockingService;
		}

		#endregion
	}
}