using Revsoft.Wabbitcode.Services.Debugger;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public interface IVariableDisplayController
    {
        string GetDisplayValue(IWabbitcodeDebugger debugger, string address, string size);

        string Name { get; }

        int Size { get; }
    }
}