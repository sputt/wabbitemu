using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using ICSharpCode.AvalonEdit.Highlighting;
using ICSharpCode.AvalonEdit.Highlighting.Xshd;
using Microsoft.Win32;
using Revsoft.Wabbitcode.Interface.Services;
using Revsoft.Wabbitcode.Panels;

namespace Revsoft.Wabbitcode.Services
{
	public class DocumentService : IDocumentService
	{
		public List<Editor> OpenDocuments;
		IRecentFileService recentFileService;
		IProjectService projectService;

		public void InitService(params Object[] objects)
		{
			recentFileService = ServiceFactory.Instance.GetServiceInstance<RecentFileService>();
			projectService = ServiceFactory.Instance.GetServiceInstance<ProjectService>();
			OpenDocuments = new List<Editor>();
			InitHighlighting();
		}

		public void DestroyService() { }

		public Editor CreateDocument(string title)
		{
			var editor = new Editor() { Title = title };
			OpenDocuments.Add(editor);
			editor.Activate();
			return editor;
		}

		public void OpenDocument()
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
			if (result != true)
				return;
			foreach (string fileName in openFileDialog.FileNames)
			{
				if (string.Equals(Path.GetExtension(fileName),  ".wcodeproj"))
					projectService.OpenProject(fileName);
				else
					OpenDocument(fileName);
			}
		}

		public Editor OpenDocument(string filename)
		{
			try
			{
				var doc = new Editor();
				OpenDocuments.Add(doc);
				//DockingService.StatusBar.ShowProgress();
				//DockingService.StatusBar.SetProgress(.1, "Open", OperationStatus.Error);
				OpenDocument(doc, filename);
				//DockingService.StatusBar.SetProgress(.9, "Open", OperationStatus.Error);
				//DockingService.StatusBar.HideProgress();
				IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<DockingService>();
				dockingService.ShowDockPanel(doc);
				doc.Activate();
				return doc;
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error opening file " + filename, ex);
				return null;
			}
		}

		public void OpenDocument(Editor doc, string filename)
		{
			doc.OpenFile(filename);
			projectService.CurrentProject.ParserService.ParseFile(filename);
			recentFileService.AddRecentFile(filename);
			recentFileService.SaveRecentFileList();
			recentFileService.GetRecentFiles();
		}

		public void OpenDocument(Editor doc)
		{
			OpenDocuments.Add(doc);
		}

		private void InitHighlighting()
		{
			IHighlightingDefinition asmHighlighting;
			Stream s = null;
			try
			{
				s = typeof(MainWindow).Assembly.GetManifestResourceStream("Revsoft.Wabbitcode.Resources.Z80Asm.xshd");
				using (XmlReader reader = new XmlTextReader(s))
				{
					s = null;
					asmHighlighting = HighlightingLoader.Load(reader, HighlightingManager.Instance);
					// and register it in the HighlightingManager
					HighlightingManager.Instance.RegisterHighlighting("Z80 Asm", new string[] { ".inc", ".z80", ".asm" }, asmHighlighting);
				}
			}
			finally
			{
				if (s != null)
				{
					s.Dispose();
				}
			}
		}
	}
}
