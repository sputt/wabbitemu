using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IDockingService
    {
        event EventHandler ActiveDocumentChanged;
        event EventHandler<DockContentEventArgs> DocumentWindowAdded;
        event EventHandler<DockContentEventArgs> DocumentWindowRemoved;

        DockPanel DockPanel { get; }
        ToolStripContainer ToolStripContainer { get; }
        IDockContent ActiveContent { get; }
        IDockContent ActiveDocument { get; }
        IEnumerable<IDockContent> Documents { get; }


        void RegisterDockingWindow<T>() where T : ToolWindow;
        ToolWindow GetDockingWindow(Type windowType);
        T GetDockingWindow<T>() where T : ToolWindow;

        void HideDockPanel(DockContent panel);
        void HideDockPanel<T>() where T : ToolWindow;
        void HideDockPanel(Type panelType);
        void ShowDockPanel(DockContent panel);
        void ShowDockPanel<T>() where T : ToolWindow;
        void ShowDockPanel(Type panelType);
        void ShowDockPanel(DockContent panel, IDockContent beforeContent);

        void ShowDockPanel<T, TBefore>()
            where T : ToolWindow
            where TBefore : ToolWindow;
        void ShowDockPanel(DockContent panel, IDockContent beforeContent, DockAlignment alignment);
        void ShowDockPanel<T, TBefore>(DockAlignment alignment)
            where T : ToolWindow
            where TBefore : ToolWindow;
        void LoadConfig(DeserializeDockContent dockContent);

        void SavePanels();
    }
}