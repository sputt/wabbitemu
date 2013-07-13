using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
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
