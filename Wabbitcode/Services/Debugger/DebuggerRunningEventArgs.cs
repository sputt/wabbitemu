using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class DebuggerRunningEventArgs : DebuggerEventArgs
    {
        public DocumentLocation Location { get; private set; }
        public bool Running { get; private set; }

        public DebuggerRunningEventArgs(DocumentLocation location, bool running)
        {
            Location = location;
            Running = running;
        }
    }
}