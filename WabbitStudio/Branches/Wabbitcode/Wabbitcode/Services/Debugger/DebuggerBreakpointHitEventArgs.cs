using System;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services
{
	public class DebuggerBreakpointHitEventArgs : DebuggerEventArgs
	{
		public DocumentLocation Location { get; private set; }

		public DebuggerBreakpointHitEventArgs(DocumentLocation location)
		{
			Location = location;
		}
	}
}