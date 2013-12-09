using System;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Debugger
{
	public class DebuggerStepEventArgs : EventArgs
	{
		public DocumentLocation Location { get; private set; }

		public DebuggerStepEventArgs(DocumentLocation location)
		{
			Location = location;
		}
	}
}
