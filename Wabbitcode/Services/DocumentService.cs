using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Properties;
using System.Windows.Forms;
using System.IO;
using Revsoft.Wabbitcode.Classes;
using System.Drawing;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services
{
	public static class DocumentService
	{
		/// <summary>
		/// Each string is the path to a recently opened file. Is also stored in properties as a big long string.
		/// </summary>
		private static List<string> recentFileList = new List<string>(5);

		public static newEditor ActiveDocument
		{
			get { return DockingService.ActiveDocument; }
		}

		public static string ActiveFileName
		{
			get
			{
				if (ActiveDocument == null)
					return null;
				return ActiveDocument.FileName;
			}
		}

		public static newEditor CreateNewDocument()
		{
			newEditor doc = new newEditor { Text = "New Document" };
			doc.editorBox.SetHighlighting("Z80 Assembly");
			doc.editorBox.Font = Settings.Default.editorFont;
			return doc;
		}

		public static void OpenDocument()
		{
			OpenFileDialog openFileDialog = new OpenFileDialog()
            {
				CheckFileExists = true,
				DefaultExt = "*.asm",
                Filter = "All Know File Types | *.asm; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
                           " Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|Project Files (*.wcodeproj)"+
						   "|*.wcodeproj|All Files(*.*)|*.*",
                FilterIndex = 0,
                RestoreDirectory = true,
                Title = "Open File",
            };
			if (openFileDialog.ShowDialog() != DialogResult.OK)
				return;
			string fileName = openFileDialog.FileName;
			string extCheck = Path.GetExtension(fileName).ToLower();
			if (extCheck == ".wcodeproj")
				ProjectService.OpenProject(fileName);
			else
				OpenDocument(new newEditor(), fileName);
		}

		internal static void OpenDocument(newEditor doc, string filename)
		{
			DockingService.MainForm.IncrementProgress(10);

			doc.Text = Path.GetFileName(filename);
			doc.TabText = Path.GetFileName(filename);
			doc.ToolTipText = filename;
			doc.MdiParent = DockingService.MainForm;
			doc.OpenFile(filename);
			if (!recentFileList.Contains(filename))
				recentFileList.Add(filename);
			SaveRecentFileList();
			GetRecentFiles();
			DockingService.MainForm.IncrementProgress(90);
			DockingService.MainForm.HideProgressBar();
			DockingService.ShowDockPanel(doc);
		}

		/// <summary>
		/// Save active document
		/// </summary>
		public static void SaveDocument()
		{
			if (ActiveDocument == null)
				return;
			SaveDocument(ActiveDocument);
		}

		internal static void SaveDocument(newEditor doc)
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
				ProjectService.ProjectWatcher.EnableRaisingEvents = false;
			if (doc.FileName == null)
			{
				if (saveFileDialog.ShowDialog() != DialogResult.OK)
					return;
				if (saveFileDialog.FileName == "")
					return;
				doc.FileName = saveFileDialog.FileName;
			}
			if (doc.FileName != "")
				doc.SaveFile();
			if (ProjectService.ProjectWatcher != null)
				ProjectService.ProjectWatcher.EnableRaisingEvents = true;
		}

		/// <summary>
		/// Save document as dialog. Also called if doc has never been saved.
		/// </summary>
		/// <returns></returns>
		public static bool SaveDocumentAs()
		{
			if (ActiveDocument == null)
				return false;
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
			if (saved != DialogResult.OK)
				return false;
			string fileName = saveFileDialog.FileName;
			if (fileName == "")
				return false;
			ActiveDocument.FileName = fileName;
			ActiveDocument.Text = fileName;
			ActiveDocument.SaveFile();
			return true;
		}

		public static newEditor GotoFile(string file)
		{
			string fileToLower = file.ToLower();
			newEditor editorBox = ActiveDocument;
			if (editorBox == null)
			{
				newEditor doc = CreateNewDocument();
				DockingService.ShowDockPanel(doc);
				doc.OpenFile(file);
				doc.Text = Path.GetFileName(file);
				doc.TabText = Path.GetFileName(file);
				doc.ToolTipText = file;
				return doc;
			}

			if (fileToLower == ActiveFileName.ToLower())
				return editorBox;
			foreach (newEditor child in DockingService.Documents)
				if (child.FileName != null && child.FileName.ToLower() == fileToLower)
				{
					child.Show();
					return child;
				}
			editorBox = CreateNewDocument();
			DockingService.ShowDockPanel(editorBox);
			editorBox.OpenFile(file);
			editorBox.Text = Path.GetFileName(file);
			editorBox.TabText = Path.GetFileName(file);
			editorBox.ToolTipText = file;
			return editorBox;
		}

		public static void GotoLine(int scrollToLine)
		{
			newEditor doc = ActiveDocument;
			doc.editorBox.ActiveTextAreaControl.ScrollTo(scrollToLine);
			doc.editorBox.ActiveTextAreaControl.Caret.Line = scrollToLine - 1;
		}

		public static void GotoLine(string file, int scrollToLine)
		{
			newEditor doc = GotoFile(file);
			doc.editorBox.ActiveTextAreaControl.ScrollTo(scrollToLine);
			doc.editorBox.ActiveTextAreaControl.Caret.Line = scrollToLine - 1;
		}

		public static void GotoLabel(IParserData item)
		{
			ParserInformation info = item.Parent;
			string file = info.SourceFile;
			newEditor doc = GotoFile(file);
			int lineNum = doc.editorBox.Document.GetLineNumberForOffset(item.Offset);
			GotoLine(file, lineNum + 1);
		}

		private static List<TextMarker> highlights = new List<TextMarker>();
		private static int debugIndex;
		public static void HighlightDebugLine(int newLineNumber)
		{
			HighlightLine(newLineNumber, Color.Yellow);
			debugIndex = highlights.Count - 1;
		}

		internal static int GetDebugOffset()
		{
			return highlights[debugIndex].Offset;
		}

		internal static void HighlightCall()
		{
			int offset = GetDebugOffset();
			TextEditorControl editor = DockingService.ActiveDocument.editorBox;
			int startOffset = offset;
			while ((offset + Environment.NewLine.Length < editor.Text.Length &&
					editor.Text.Substring(offset, Environment.NewLine.Length) != Environment.NewLine) ||
					editor.Text[offset] == ';')
				offset++;
			string line = editor.Text.Substring(startOffset, offset - startOffset).ToLower();
			if (line.Contains("call") || line.Contains("bcall") || line.Contains("b_call") || line.Contains("rst"))
			{
				DebuggerService.StepStack.Push(editor.Document.GetLineNumberForOffset(offset));
				//DocumentService.HighlightLine(lineNumber, Color.Green);
			}
			//if (line.Contains("ret"))
			//	DebuggerService.StepStack.Pop();
		}

		public static void HighlightLine(int newLineNumber, Color foregroundColor)
		{
			//this code highlights the current line
			//I KNOW IT WORKS DONT FUCK WITH IT
			TextEditorControl editorBox = DockingService.ActiveDocument.editorBox;
			TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
			editorBox.ActiveTextAreaControl.ScrollTo(newLineNumber - 1);
			editorBox.ActiveTextAreaControl.Caret.Line = newLineNumber - 1;
			int start = textArea.Caret.Offset == editorBox.Text.Length ? textArea.Caret.Offset - 1 : textArea.Caret.Offset;
			int length = editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
			if (textArea.Document.TextContent[start] == '\n')
				start--;
			while (start > 0 && textArea.Document.TextContent[start] != '\n')
				start--;
			start++;
			while (start < textArea.Document.TextContent.Length && (textArea.Document.TextContent[start] == ' ' || textArea.Document.TextContent[start] == '\t'))
			{
				start++;
				length--;
			}
			if (length >= editorBox.Text.Length)
				length += (editorBox.Text.Length - 1) - length;
			if (editorBox.Text.IndexOf(';', start, length) != -1)
				length = editorBox.Text.IndexOf(';', start, length) - start - 1;
			if (editorBox.Text.Length <= start + length)
				length--;
			while (editorBox.Text[start + length] == ' ' || editorBox.Text[start + length] == '\t')
				length--;
			length++;
			TextMarker highlight = new TextMarker(start, length, TextMarkerType.SolidBlock, foregroundColor, Color.Black) 
				{ Tag = DockingService.ActiveDocument.FileName };
			editorBox.Document.MarkerStrategy.AddMarker(highlight);
			highlights.Add(highlight);
			editorBox.Refresh();
		}

		public static void RemoveDebugHighlight()
		{
			RemoveHighlight(debugIndex);
			
		}

		public static void RemoveHighlight(int index)
		{
			if (DockingService.ActiveDocument == null || highlights.Count == 0)
				return;
			TextMarker highlight = highlights[index];
			foreach (newEditor child in DockingService.Documents)
				if (child.FileName == highlight.Tag)
				{
					child.Show();
					break;
				}
			highlights.Remove(highlight);
			if (index <= debugIndex)
				debugIndex--;
			DockingService.ActiveDocument.editorBox.Document.MarkerStrategy.RemoveMarker(highlight);
		}

		/// <summary>
		/// This takes the string array of recent files, joins them into a large string and saves it in Properties.
		/// </summary>
		private static void SaveRecentFileList()
		{
			string list = null;
			foreach (String file in recentFileList)
				list += file + '\n';
			Settings.Default.recentFiles = list;
		}

		/// <summary>
		/// This loads the recent file list from Properties and adds it to the recent file menu.
		/// </summary>
		internal static void GetRecentFiles()
		{
			String line = Settings.Default.recentFiles;
			string[] list = line.Split('\n');

			foreach (String file in list)
			{
				if (string.IsNullOrEmpty(file))
					continue;
				DockingService.MainForm.AddRecentItem(file);
			}
		}

		public static bool InternalSave { get; set; }
	}
}
