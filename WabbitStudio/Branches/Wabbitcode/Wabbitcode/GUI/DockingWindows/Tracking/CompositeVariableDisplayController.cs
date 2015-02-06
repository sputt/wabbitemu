using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    internal class CompositeVariableDisplayController : IVariableDisplayController
    {
        internal DebuggingStructure DebuggingStructure { get; private set; }

        public IEnumerable<IVariableDisplayController> ChildControllers
        {
            get { return DebuggingStructure.Properties.Select(p => p.Controller); }
        }

        public string Name
        {
            get { return DebuggingStructure.Name; }
        }

        public int Size { get; private set; }

        public CompositeVariableDisplayController(DebuggingStructure debuggingStructure)
        {
            DebuggingStructure = debuggingStructure;

            Size = debuggingStructure.Properties.Sum(prop => prop.Controller.Size);
        }

        public string GetDisplayValue(IWabbitcodeDebugger debugger, string address, string size)
        {
            return string.Empty;
        }
        public object GetActualValue(IWabbitcodeDebugger debugger, string address, string size)
        {
            return null;
        }
    }
}