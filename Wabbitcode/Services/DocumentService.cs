using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Parser;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency(typeof(IDockingService))]
	[ServiceDependency(typeof(IBackgroundAssemblerService))]
	public static class DocumentService
	{
		private static int _debugIndex;
		private static readonly List<DocumentLocation> Highlights = new List<DocumentLocation>();
		private static int _recentFileIndex;
		private static IDockingService _dockingService;
		private static IBackgroundAssemblerService _backgroundAssemblerService;

		/// <summary>
		/// Each string is the path to a recently opened file. Is also stored in properties as a big long string.
		/// </summary>
		private static readonly string[] RecentFileList = new string[10];

		public enum FixableErrorType
		{
			RelativeJump,
		}

		public static NewEditor ActiveDocument
		{
			get
			{
				return _dockingService.ActiveDocument;
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

		public static void SetServices(IDockingService dockingService, IBackgroundAssemblerService backgroundAssemblerService)
		{
			_dockingService = dockingService;
			_backgroundAssemblerService = backgroundAssemblerService;
		}

		public static NewEditor CreateNewDocument()
		{
			NewEditor doc = new NewEditor(_dockingService, _backgroundAssemblerService)
			{
				Text = "New Document", 
				TabText = "New Document"
			};
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

			foreach (NewEditor child in _dockingService.Documents.Where(child => !string.IsNullOrEmpty(child.FileName) &&
				string.Equals(child.FileName, file, StringComparison.OrdinalIgnoreCase)))
			{
				child.Show();
				return child;
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

		public static void GotoLine(int line)
		{
			ActiveDocument.ScrollToLine(line);
			// fix for 0 indexed
			ActiveDocument.SetCaretPosition(0, line - 1);
		}

		public static void GotoLine(string file, int scrollToLine)
		{
			NewEditor child = GotoFile(file);
			child.ScrollToLine(scrollToLine);
		}

		public static void HighlightDebugLine(int newLineNumber)
		{
			HighlightLine(newLineNumber, Color.Yellow);
			_debugIndex = Highlights.Count - 1;
		}

		private static void HighlightLine(int newLineNumber, Color foregroundColor)
		{
			DocumentLocation value = new DocumentLocation(ActiveFileName, newLineNumber);
			Highlights.Add(value);
			ActiveDocument.HighlightLine(newLineNumber, foregroundColor);
		}

		public static void RemoveDebugHighlight()
		{
			if (ActiveDocument == null || Highlights.Count == 0 || _debugIndex < 0)
			{
				return;
			}
			DocumentLocation key = Highlights[_debugIndex];
			GotoFile(key.FileName).RemoveDebugHighlight(key.LineNumber);
			Highlights.Remove(key);
		}

		private static void RemoveHighlight(int index)
		{
			if (ActiveDocument == null || Highlights.Count == 0)
			{
				return;
			}
			DocumentLocation key = Highlights[index];
			GotoFile(key.FileName).RemoveHighlight(key.LineNumber);
			Highlights.Remove(key);
			if (index <= _debugIndex)
			{
				_debugIndex--;
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
		public static void SaveDocumentAs()
		{
			if (ActiveDocument == null)
			{
				return;
			}
			SaveFileDialog saveFileDialog = new SaveFileDialog
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
				return;
			}
			string fileName = saveFileDialog.FileName;
			if (string.IsNullOrEmpty(fileName))
			{
				return;
			}
			ActiveDocument.FileName = fileName;
			ActiveDocument.Text = fileName;
			ActiveDocument.SaveFile();
		}

		/// <summary>
		/// This loads the recent file list from Properties and adds it to the recent file menu.
		/// </summary>
		internal static void GetRecentFiles()
		{
			_dockingService.MainForm.ClearRecentItems();
			string line = Settings.Default.recentFiles;
			string[] list = line.Split('\n');
			foreach (string file in list.Where(file => !string.IsNullOrEmpty(file.Trim())))
			{
				_dockingService.MainForm.AddRecentItem(file);
			}
		}

		internal static void GotoCurrentDebugLine()
		{
			GotoFile(Highlights[_debugIndex].FileName);
		}

		internal static void HighlightCall()
		{
			GotoFile(Highlights[_debugIndex].FileName).HighlightCall(Highlights[_debugIndex].LineNumber);
		}

		internal static NewEditor OpenDocument(string filename)
		{
			NewEditor doc = new NewEditor(_dockingService, _backgroundAssemblerService);
			OpenDocument(doc, filename);
			return doc;
		}

		internal static void OpenDocument(NewEditor doc, string filename)
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

		internal static void SaveDocument(NewEditor doc)
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog
			{
				DefaultExt = "asm",
				RestoreDirectory = true,
				Filter = "All Know File Types | *.asm; *.z80| Assembly Files (*.asm)|*.asm|Z80" +
						 " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
				FilterIndex = 0,
				Title = "Save File As"
			};
			//if (_projectService.ProjectWatcher != null)
			//{
			//	ProjectService.ProjectWatcher.EnableRaisingEvents = false;
			//}
			if (string.IsNullOrEmpty(doc.FileName))
			{
				if (saveFileDialog.ShowDialog() != DialogResult.OK)
				{
					return;
				}
				if (string.IsNullOrEmpty(saveFileDialog.FileName))
				{
					return;
				}
				doc.FileName = saveFileDialog.FileName;
			}

			if (!string.IsNullOrEmpty(doc.FileName))
			{
				doc.SaveFile();
			}
			//if (ProjectService.ProjectWatcher != null)
			//{
			//	ProjectService.ProjectWatcher.EnableRaisingEvents = true;
			//}
		}

		/// <summary>
		/// Adds a string to the recent file list
		/// </summary>
		/// <param name="filename">Full path of the file to save to the list</param>
		private static void AddRecentFile(string filename)
		{
			if (!RecentFileList.Contains(filename))
			{
				if (_recentFileIndex == RecentFileList.Length)
				{
					Array.ConstrainedCopy(RecentFileList, 1, RecentFileList, 0, RecentFileList.Length - 1);
					_recentFileIndex--;
				}

				RecentFileList[_recentFileIndex++] = filename;
			}
		}

		/// <summary>
		/// This takes the string array of recent files, joins them into a large string and saves it in Properties.
		/// </summary>
		private static void SaveRecentFileList()
		{
			StringBuilder list = new StringBuilder();
			foreach (string file in RecentFileList)
			{
				list.Append(file + "\n");
			}
			Settings.Default.recentFiles = list.ToString();
		}
	}
}