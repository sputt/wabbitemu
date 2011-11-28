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
	[ServiceDependency("RecentFileService")]
	[ServiceDependency("ProjectService")]
	[ServiceDependency("DockingService")]
	public class DocumentService : IDocumentService
	{
		public List<Editor> OpenDocuments;
		IRecentFileService recentFileService;
		IProjectService projectService;
		IDockingService dockingService;

		public void InitService(params Object[] objects)
		{
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
            //TODO: do this on different threads
			projectService.CurrentProject.ParserService.ParseFile(filename, doc.editor.Text);
            //handle recent file stuff
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
