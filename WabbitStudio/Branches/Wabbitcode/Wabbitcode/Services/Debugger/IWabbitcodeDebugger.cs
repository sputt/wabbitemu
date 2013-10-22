
namespace Revsoft.Wabbitcode.Services.Debugger
{
    public enum StepType
    {
        Step,
        StepOver,
        StepOut
    }

    public interface IWabbitcodeDebugger
    {
        bool SetBreakpoint(WabbitcodeBreakpoint breakpoint);

        void CancelDebug();

        void GotoAddress(ushort address);

        bool IsBreakpointPossible(int lineNumber, string fileName);

        void SetPCToSelect(string fileName, int lineNumber);

        void Step();

        void StepOut();

        void StepOver();
    }
}