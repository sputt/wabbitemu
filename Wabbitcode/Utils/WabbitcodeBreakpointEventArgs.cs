using System;
using Revsoft.Wabbitcode.Services.Debugger;

namespace Revsoft.Wabbitcode.Utils
{
	public class WabbitcodeBreakpointEventArgs : EventArgs
	{
		public WabbitcodeBreakpoint Breakpoint { get; private set; }

		public WabbitcodeBreakpointEventArgs(WabbitcodeBreakpoint breakpoint)
		{
			Breakpoint = breakpoint;
		}
	}
}