using System;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class BreakpointEventArgs : EventArgs
    {
        public IBreakpoint Breakpoint { get; private set; }

        public BreakpointEventArgs(IBreakpoint breakpoint)
        {
            Breakpoint = breakpoint;
        }
    }
}