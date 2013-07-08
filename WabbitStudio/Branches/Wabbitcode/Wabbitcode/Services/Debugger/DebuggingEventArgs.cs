using Revsoft.Wabbitcode.Services;
using System;
using System.Linq;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class DebuggingEventArgs : EventArgs
    {
        public WabbitcodeDebugger Debugger { get; private set; }

        public DebuggingEventArgs(WabbitcodeDebugger debugger)
        {
            Debugger = debugger;
        }
    }
}
