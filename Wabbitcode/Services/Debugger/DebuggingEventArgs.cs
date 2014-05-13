using System;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class DebuggingEventArgs : EventArgs
    {
        public IWabbitcodeDebugger Debugger { get; private set; }

        public DebuggingEventArgs(IWabbitcodeDebugger debugger)
        {
            Debugger = debugger;
        }
    }
}