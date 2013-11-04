using System.Linq;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Extensions;
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

		    if (OnBreakpointAdded == null)
		    {
		        return;
		    }

            var eventArgs = new WabbitcodeBreakpointEventArgs(newBreak);
            foreach (BreakpointAdded handler in OnBreakpointAdded.GetInvocationList())
            {
                if (!eventArgs.Cancel)
                {
                    handler(null, eventArgs);
                }
            }
		}

		public static void RemoveBreakpoint(string fileName, int lineNumber)
		{
			WabbitcodeBreakpoint newBreak = Breakpoints.FirstOrDefault(b => FileOperations.CompareFilePath(fileName, b.File) && b.LineNumber == lineNumber);
			if (newBreak == null)
			{
				return;
			}
			_breakpoints.Remove(newBreak);

		    if (OnBreakpointRemoved == null)
		    {
		        return;
		    }

		    var eventArgs = new WabbitcodeBreakpointEventArgs(newBreak);
            foreach (BreakpointRemoved handler in OnBreakpointRemoved.GetInvocationList())
		    {
		        if (!eventArgs.Cancel)
		        {
		            handler(null, eventArgs);
		        }
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
