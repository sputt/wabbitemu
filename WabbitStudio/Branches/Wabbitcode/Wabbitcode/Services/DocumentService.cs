using System.Collections.Specialized;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency(typeof (IBackgroundAssemblerService))]
	[ServiceDependency(typeof (IDockingService))]
	[ServiceDependency(typeof (IParserService))]
	[ServiceDependency(typeof(IProjectService))]
	[ServiceDependency(typeof (ISymbolService))]
    [ServiceDependency(typeof(IFileReaderService))]
	public class DocumentService : IDocumentService
	{
		private int _debugIndex;
		private readonly List<DocumentLocation> _highlights = new List<DocumentLocation>();
		private int _recentFileIndex;
		private readonly IDockingService _dockingService;
		private readonly IBackgroundAssemblerService _backgroundAssemblerService;
	    private readonly IFileReaderService _fileReaderService;
		private readonly IParserService _parserService;
		private readonly IProjectService _projectService;
		private readonly ISymbolService _symbolService;

		/// <summary>
		/// Each string is the path to a recently opened file. Is also stored in properties as a big long string.
		/// </summary>
		private readonly string[] _recentFileList = new string[10];

		public enum FixableErrorType
		{
			RelativeJump,
		}

		public Editor ActiveDocument
		{
			get { return _dockingService.ActiveDocument; }
		}

		public string ActiveFileName
		{
			get { return ActiveDocument == null ? null : ActiveDocument.FileName; }
		}

		public Editor CreateNewDocument()
		{
			Editor doc = new Editor(_backgroundAssemblerService, _dockingService, this,
				_fileReaderService, _parserService, _projectService, _symbolService)
			{
				Text = "New Document",
				TabText = "New Document"
			};
			doc.SetHighlighting("Z80 Assembly");
			return doc;
		}

		public Editor GotoFile(string file)
		{
			Editor editorBox = ActiveDocument;
			if (editorBox == null)
			{
				return OpenDocument(file);
			}

			if (string.Equals(file, ActiveFileName, StringComparison.OrdinalIgnoreCase))
			{
				return ActiveDocument;
			}

			foreach (Editor child in _dockingService.Documents.Where(child => !string.IsNullOrEmpty(child.FileName) &&
			                                                                  string.Equals(child.FileName, file, StringComparison.OrdinalIgnoreCase)))
			{
				child.Show();
				return child;
			}

			return OpenDocument(file);
		}

		public void GotoLabel(IParserData item)
		{
			ParserInformation info = item.Parent;
			string file = info.SourceFile;
			Editor child = GotoFile(file);
			child.ScrollToOffset(item.Location.Offset);
		}

		public void GotoLine(Editor editor, int line)
		{
			editor.ScrollToLine(line);
			// fix for 0 indexed
			editor.SetCaretPosition(0, line - 1);
		}

		public void GotoLine(string file, int scrollToLine)
		{
			Editor child = GotoFile(file);
			GotoLine(child, scrollToLine);
		}

		public void HighlightDebugLine(int newLineNumber)
		{
			HighlightLine(newLineNumber, Color.Yellow);
			_debugIndex = _highlights.Count - 1;
		}

		private void HighlightLine(int newLineNumber, Color foregroundColor)
		{
			DocumentLocation value = new DocumentLocation(ActiveFileName, newLineNumber);
			_highlights.Add(value);
			ActiveDocument.HighlightLine(newLineNumber, foregroundColor);
		}

		public void RemoveDebugHighlight()
		{
			if (ActiveDocument == null || _highlights.Count == 0 || _debugIndex < 0)
			{
				return;
			}
			DocumentLocation key = _highlights[_debugIndex];
			GotoFile(key.FileName).RemoveDebugHighlight(key.LineNumber);
			_highlights.Remove(key);
		}

		private void RemoveHighlight(int index)
		{
			if (ActiveDocument == null || _highlights.Count == 0)
			{
				return;
			}
			DocumentLocation key = _highlights[index];
			GotoFile(key.FileName).RemoveHighlight(key.LineNumber);
			_highlights.Remove(key);
			if (index <= _debugIndex)
			{
				_debugIndex--;
			}
		}

		/// <summary>
		/// This loads the recent file list from Properties and adds it to the recent file menu.
		/// </summary>
		public void GetRecentFiles()
		{
		    if (Settings.Default.recentFiles == null)
		    {
		        Settings.Default.recentFiles = new StringCollection();
		    }

			_dockingService.MainForm.ClearRecentItems();
			foreach (string file in Settings.Default.recentFiles.Cast<string>().Where(s => !string.IsNullOrEmpty(s)))
			{
				_dockingService.MainForm.AddRecentItem(file);
			}
		}

		public void GotoCurrentDebugLine()
		{
			GotoLine(_highlights[_debugIndex].FileName, _highlights[_debugIndex].LineNumber);
		}

		public void HighlightCall()
		{
			GotoFile(_highlights[_debugIndex].FileName)
                .HighlightLine(_highlights[_debugIndex].LineNumber, Color.LightGreen);
		}

		public Editor OpenDocument(string filename)
		{
			Editor doc = new Editor(_backgroundAssemblerService, _dockingService, this,
				_fileReaderService, _parserService, _projectService, _symbolService);
			OpenDocument(doc, filename);
			return doc;
		}

		public void OpenDocument(Editor doc, string filename)
		{
			doc.Text = Path.GetFileName(filename);
			doc.TabText = Path.GetFileName(filename);
			doc.ToolTipText = filename;
			doc.MdiParent = _dockingService.MainForm;
			doc.OpenFile(filename);
			AddRecentFile(filename);
			SaveRecentFileList();
			GetRecentFiles();
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
            if (Settings.Default.recentFiles == null)
            {
                Settings.Default.recentFiles = new StringCollection();
            }

            Settings.Default.recentFiles.Clear();
            Settings.Default.recentFiles.AddRange(_recentFileList);
		}

		#region IService

		public void InitService(params object[] objects)
		{
		}

		public void DestroyService()
		{
		}

		public DocumentService(IBackgroundAssemblerService backgroundAssemblerService, IDockingService dockingService, 
			IFileReaderService fileReaderService, IParserService parserService, IProjectService projectService,
            ISymbolService symbolService)
		{
			_backgroundAssemblerService = backgroundAssemblerService;
			_dockingService = dockingService;
		    _fileReaderService = fileReaderService;
			_parserService = parserService;
			_projectService = projectService;
			_symbolService = symbolService;
		}

		#endregion
	}
}