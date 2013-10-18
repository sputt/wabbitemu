using System.Linq;
using Revsoft.TextEditor.Document;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Debugger;

namespace Revsoft.Wabbitcode.Utils
{
	public static class WabbitcodeBreakpointManager
	{
		#region Private Members

		private static readonly List<WabbitcodeBreakpoint> _breakpoints = new List<WabbitcodeBreakpoint>();
		public static IEnumerable<WabbitcodeBreakpoint> Breakpoints
		{
			get
			{
				return _breakpoints;
			}
		} 

		#endregion

		#region Events
		public delegate void BreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e);
		public static event BreakpointAdded OnBreakpointAdded;

		public delegate void BreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e);
		public static event BreakpointRemoved OnBreakpointRemoved;
		#endregion


		public static void AddBreakpoint(string fileName, int lineNumber)
		{
			WabbitcodeBreakpoint newBreak = new WabbitcodeBreakpoint(fileName, lineNumber);
			if (_breakpoints.Contains(newBreak))
			{
				return;
			}
			_breakpoints.Add(newBreak);

			if (OnBreakpointAdded != null)
			{
				OnBreakpointAdded(null, new WabbitcodeBreakpointEventArgs(newBreak));
			}
		}

		public static void RemoveBreakpoint(string fileName, int lineNumber)
		{
			WabbitcodeBreakpoint newBreak = Breakpoints.FirstOrDefault(b => b.File == fileName && b.LineNumber == lineNumber);
			if (newBreak == null)
			{
				return;
			}
			_breakpoints.Remove(newBreak);

			if (OnBreakpointRemoved != null)
			{
				OnBreakpointRemoved(null, new WabbitcodeBreakpointEventArgs(newBreak));
			}
		}

		public static void RemoveAllBreakpointsInFile(string fileName)
		{
			var breakpointsToRemove = Breakpoints.Where(b => b.File == fileName);

			foreach (var breakpoint in breakpointsToRemove)
			{
				RemoveBreakpoint(breakpoint.File, breakpoint.LineNumber);
			}
		}

		public static void RemoveAllBreakpoints()
		{
			foreach (var breakpoint in Breakpoints)
			{
				RemoveBreakpoint(breakpoint.File, breakpoint.LineNumber);
			}
		}
	}
}
