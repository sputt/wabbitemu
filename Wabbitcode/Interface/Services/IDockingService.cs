using System;
using Revsoft.Wabbitcode.Panels;
using Revsoft.Wabbitcode.Interface;
using AvalonDock;
namespace Revsoft.Wabbitcode.Services
{
	public interface IDockingService : IService
	{
		ManagedContent ActiveContent { get; }
		Editor ActiveDocument { get; }
		IWabbitcodePanel ActivePanel { get; }
		ManagedContentCollection<DocumentContent> Documents { get; }
		ErrorList ErrorList { get; }
		LabelList LabelList { get; }
		MainWindow MainWindow { get; }
		OutputWindow OutputWindow { get; }
		ProjectViewer ProjectViewer { get; }
		WabbitcodeStatusBarService StatusBar { get; }

		void InitPanels(WabbitcodeStatusBar statusBar);

		void ShowDockPanel(DockableContent newFile);
		void ShowDockPanel(DocumentContent newFile);
	}
}
