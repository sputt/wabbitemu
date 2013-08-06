using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.Services.Interface;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services
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
		IDockContent ActiveContent { get; }
		NewEditor ActiveDocument { get; }
		IEnumerable<NewEditor> Documents { get; }
		MainFormRedone MainForm { get; }

		void HideDockPanel(DockContent panel);
		void ShowDockPanel(DockContent panel);
		void LoadConfig();
		IDockContent GetContentFromPersistString(string persistString);
		void Invoke(Action action);

		void InitPanels(ProjectViewer projectViewer, ErrorList errorList, TrackingWindow trackingWindow,
			DebugPanel debugPanel, CallStack callStack, LabelList labelList, OutputWindow outputWindow,
			FindAndReplaceForm findAndReplaceForm, FindResultsWindow findResults, MacroManager macroManager,
			BreakpointManagerWindow breakpointManagerWindow, StackViewer stackViewer);
	}
}