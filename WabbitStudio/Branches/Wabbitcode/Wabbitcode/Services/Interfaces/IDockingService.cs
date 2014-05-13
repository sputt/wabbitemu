using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IDockingService
	{
        event EventHandler ActiveDocumentChanged;
        event EventHandler<DockContentEventArgs> DocumentWindowAdded;
        event EventHandler<DockContentEventArgs> DocumentWindowRemoved;

	    void RegisterDockingWindow(ToolWindow dockingWindow);
	    ToolWindow GetDockingWindow(string name);

		IDockContent ActiveContent { get; }
		IDockContent ActiveDocument { get; }
		IEnumerable<IDockContent> Documents { get; }

        void HideDockPanel(DockContent panel);
		void HideDockPanel(string panelName);
        void ShowDockPanel(DockContent panel);
        void ShowDockPanel(string panelName);
        void ShowDockPanel(DockContent panel, IDockContent beforeContent);
        void ShowDockPanel(string panelName, string beforeContentName);
	    void ShowDockPanel(DockContent panel, IDockContent beforeContent, DockAlignment alignment);
        void ShowDockPanel(string panelName, string beforeContentName, DockAlignment alignment);
		void LoadConfig(DeserializeDockContent dockContent);
	    void InitPanels();

	    void SavePanels();
	}
}