using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Debugger
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