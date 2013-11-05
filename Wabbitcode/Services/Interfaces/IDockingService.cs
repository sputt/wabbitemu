using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.DockingWindows;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IDockingService : IService
	{
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
		bool HasBeenInited { get; }
		LabelList LabelList { get; }
		MacroManager MacroManager { get; }
        ExpressionWindow ExpressionWindow { get; }
		IDockContent ActiveContent { get; }
		Editor ActiveDocument { get; }
		IEnumerable<Editor> Documents { get; }

		void HideDockPanel(DockContent panel);
		void ShowDockPanel(DockContent panel);
        void ShowDockPanel(DockContent panel, IDockContent beforeContent);
	    void ShowDockPanel(DockContent panel, IDockContent beforeContent, DockAlignment alignment);
		void LoadConfig(DeserializeDockContent handleDeserializeDockContent);
		void Invoke(Action action);

		void InitPanels(ProjectViewer projectViewer, ErrorList errorList, TrackingWindow trackingWindow,
			DebugPanel debugPanel, CallStack callStack, LabelList labelList, OutputWindow outputWindow,
			FindAndReplaceForm findAndReplaceForm, FindResultsWindow findResults, MacroManager macroManager,
            BreakpointManagerWindow breakpointManagerWindow, StackViewer stackViewer, ExpressionWindow expressionWindow);
	}
}