using Revsoft.TextEditor.Document;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Debugger;

namespace Revsoft.Wabbitcode.Utils
{
	public static class WabbitcodeBreakpointManager
	{
		#region Private Members

		private readonly static List<WabbitcodeBreakpoint> Breakpoints = new List<WabbitcodeBreakpoint>();

		#endregion

		#region Events
		public delegate void BreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e);
		public static event BreakpointAdded OnBreakpointAdded;

		public delegate void BreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e);
		public static event BreakpointRemoved OnBreakpointRemoved;
		#endregion


		public static void AddBreakpoint(string fileName, Breakpoint breakpoint)
		{
			WabbitcodeBreakpoint newBreak = new WabbitcodeBreakpoint(fileName, breakpoint.LineNumber);
			Breakpoints.Add(newBreak);

			if (OnBreakpointAdded != null)
			{
				OnBreakpointAdded(null, new WabbitcodeBreakpointEventArgs());
			}
		}

		public static void RemoveBreakpoint(string fileName, Breakpoint breakpoint)
		{
			WabbitcodeBreakpoint newBreak = new WabbitcodeBreakpoint(fileName, breakpoint.LineNumber);
			Breakpoints.Remove(newBreak);

			if (OnBreakpointRemoved != null)
			{
				OnBreakpointRemoved(null, new WabbitcodeBreakpointEventArgs());
			}
		}
	}
}
