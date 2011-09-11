using System;
using System.IO;
using System.Windows;
using AvalonDock;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Panels;

namespace Revsoft.Wabbitcode.Services
{
	public class DockingService : IDockingService
	{
		public DockingManager DockManager { get; private set; }
		public MainWindow MainWindow { get; private set; }

		public IWabbitcodePanel ActivePanel { get { return (IWabbitcodePanel) DockManager.ActiveContent; } }
		public ManagedContent ActiveContent { get { return DockManager.ActiveContent; } }
		public Editor ActiveDocument { get { return (Editor) DockManager.ActiveDocument; } }
		public ManagedContentCollection<DocumentContent> Documents
		{
			get
			{
				if (DockManager != null)
					return DockManager.Documents;
				return null;
			}
		}

		public WabbitcodeStatusBarService StatusBar { get; private set; }
		public LabelList LabelList { get; private set; }
		public ProjectViewer ProjectViewer { get; private set; }
		public OutputWindow OutputWindow { get; private set; }
		public ErrorList ErrorList { get; private set; }

		private IPathsService pathsService;

		public void InitService(params Object[] objects)
		{
			DockingManager DockManager = objects[0] as DockingManager;
			if (DockManager == null)
				throw new ArgumentException("First parameter is not of type DockingManager");
			MainWindow mainWindow = objects[1] as MainWindow;
			if (mainWindow == null)
				throw new ArgumentException("Second parameter is not of type MainWindow");

			pathsService = ServiceFactory.Instance.GetServiceInstance<PathsService>();

			this.DockManager = DockManager;
			this.MainWindow = mainWindow;
			Application.Current.Resources["ThemeDictionary"] = new ResourceDictionary();
			ThemeFactory.ChangeTheme("dev2010");

			MainWindow.WindowState = PropertyService.GetWabbitcodeProperty("WindowState");
			MainWindow.Top = PropertyService.GetWabbitcodeProperty("Top");
			MainWindow.Left = PropertyService.GetWabbitcodeProperty("Left");
			MainWindow.Width = PropertyService.GetWabbitcodeProperty("Width");
			MainWindow.Height = PropertyService.GetWabbitcodeProperty("Height");

			DockManager.DeserializationCallback = new DockingManager.DeserializationCallbackHandler(DeserializationCallback);
		}

		private void DeserializationCallback(object s, DeserializationCallbackEventArgs e)
		{
			if (e.Name != "EditorDocument")
				return;
			e.Content = new Editor();
		}

		public void InitPanels(WabbitcodeStatusBar statusBar)
		{
			StatusBar = new WabbitcodeStatusBarService(statusBar);

			//Init Normal Panels
			LabelList = new LabelList();
			LabelList.Show(DockManager, AnchorStyle.Right);

			ProjectViewer = new ProjectViewer();
			ProjectViewer.Show(DockManager, AnchorStyle.Left);

			OutputWindow = new OutputWindow();
			OutputWindow.Show(DockManager, AnchorStyle.Bottom);

			ErrorList = new ErrorList();
			ErrorList.Show(DockManager, AnchorStyle.Bottom);

			if (File.Exists(pathsService.DockConfig))
			{
				try
				{
					DockManager.RestoreLayout(pathsService.DockConfig);
				} catch (Exception) { }
			}
			IDocumentService documentService = ServiceFactory.Instance.GetServiceInstance<DocumentService>();
			try
			{
				DocumentContent[] docs = new DocumentContent[Documents.Count];
				Documents.CopyTo(docs, 0);
				foreach (Editor doc in docs)
					documentService.OpenDocument(doc);
			}
			catch (Exception ex)
			{
				ShowError("Error restoring documents", ex);
			}
		}

		public void DestroyService()
		{
			PropertyService.SaveWabbitcodeProperty("WindowState", MainWindow.WindowState);
			PropertyService.SaveWabbitcodeProperty("Top", MainWindow.Top);
			PropertyService.SaveWabbitcodeProperty("Left", MainWindow.Left);
			PropertyService.SaveWabbitcodeProperty("Width", MainWindow.Width);
			PropertyService.SaveWabbitcodeProperty("Height", MainWindow.Height);
			PropertyService.Save();

			DockManager.SaveLayout(pathsService.DockConfig);
		}

		public static void ShowError(string error)
		{
			MessageBox.Show(error, "Error", MessageBoxButton.OK, MessageBoxImage.Error);
		}

		public static void ShowError(string error, Exception ex)
		{
			ShowError(error + "\n" + ex.ToString());
		}

		public void ShowDockPanel(DockableContent doc)
		{
			doc.Show(DockManager);
		}

		public void ShowDockPanel(DocumentContent doc)
		{
			doc.Show(DockManager);
			doc.Activate();
		}
	}
}
