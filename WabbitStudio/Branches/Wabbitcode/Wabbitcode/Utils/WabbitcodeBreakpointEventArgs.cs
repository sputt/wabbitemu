using System.ComponentModel;
using Revsoft.Wabbitcode.Services.Debugger;

namespace Revsoft.Wabbitcode.Utils
{
	public class WabbitcodeBreakpointEventArgs : CancelEventArgs
	{
		public WabbitcodeBreakpoint Breakpoint { get; private set; }

		public WabbitcodeBreakpointEventArgs(WabbitcodeBreakpoint breakpoint)
		{
			Breakpoint = breakpoint;
		}
	}
}