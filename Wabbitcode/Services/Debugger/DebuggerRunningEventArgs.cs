using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services
{
	public class DebuggerRunningEventArgs : DebuggerEventArgs
	{
		public DocumentLocation Location { get; set; }
		public bool Running { get; set; }

		public DebuggerRunningEventArgs(DocumentLocation location, bool running)
		{
			Location = location;
			Running = running;
		}
	}
}
