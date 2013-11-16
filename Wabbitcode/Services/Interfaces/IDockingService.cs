using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.DockingWindows;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IDockingService : IService
	{
        event EventHandler OnActiveDocumentChanged;

		BreakpointManagerWindow BreakManagerWindow { get; }
		CallStack CallStack { get; }
		DebugPanel DebugPanel { get; }
		ErrorList ErrorList { get; }
		FindAndReplaceForm FindForm { get; }
		FindResultsWindow FindResults { get; }
		OutputWindow OutputWindow { get; }
		ProjectViewer ProjectViewer { get; }
		StackViewer StackViewer { get; }
		TrackingWindow TrackWindow { get; }
		LabelList LabelList { get; }
		MacroManager MacroManager { get; }
        ExpressionWindow ExpressionWindow { get; }
		IDockContent ActiveContent { get; }
		IDockContent ActiveDocument { get; }
		IEnumerable<IDockContent> Documents { get; }

		void HideDockPanel(DockContent panel);
		void ShowDockPanel(DockContent panel);
        void ShowDockPanel(DockContent panel, IDockContent beforeContent);
	    void ShowDockPanel(DockContent panel, IDockContent beforeContent, DockAlignment alignment);
		void LoadConfig(DeserializeDockContent handleDeserializeDockContent);
	    void InitPanels();
	}
}