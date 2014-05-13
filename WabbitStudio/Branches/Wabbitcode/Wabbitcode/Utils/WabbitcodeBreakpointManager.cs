using System.Linq;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Utils
{
	public static class WabbitcodeBreakpointManager
	{
		#region Private Members

		private static readonly List<WabbitcodeBreakpoint> BreakList = new List<WabbitcodeBreakpoint>();
		public static IEnumerable<WabbitcodeBreakpoint> Breakpoints
		{
			get
			{
				return BreakList;
			}
		} 

		#endregion

		#region Events
		public delegate void BreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e);
		public static event BreakpointAdded OnBreakpointAdded;

		public delegate void BreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e);
		public static event BreakpointRemoved OnBreakpointRemoved;
		#endregion

		public static void AddBreakpoint(FilePath fileName, int lineNumber)
		{
			WabbitcodeBreakpoint newBreak = new WabbitcodeBreakpoint(fileName, lineNumber);
			if (BreakList.Contains(newBreak))
			{
				return;
			}
			BreakList.Add(newBreak);

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

		    if (!eventArgs.Cancel)
		    {
		        return;
		    }

		    IStatusBarService statusBarService = DependencyFactory.Resolve<IStatusBarService>();
		    statusBarService.SetText("A breakpoint could not be set at this location");
		}

		public static void RemoveBreakpoint(FilePath fileName, int lineNumber)
		{
			WabbitcodeBreakpoint newBreak = Breakpoints.FirstOrDefault(b => fileName == b.File && b.LineNumber == lineNumber);
			if (newBreak == null)
			{
				return;
			}
			BreakList.Remove(newBreak);

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
