
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
        void AddBreakpoint(int lineNumber, string fileName);

        void CancelDebug();

        void GotoAddress(ushort address);

        bool IsBreakpointPossible(int lineNumber, string fileName);

        void RemoveBreakpoint(int lineNumber, string fileName);

        void SetPCToSelect(string fileName, int lineNumber);

        void Step();

        void StepOut();

        void StepOver();
    }
}