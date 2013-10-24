using System;
using System.Collections.Generic;
using System.Drawing;
using Revsoft.Wabbitcode.Utils;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public enum StepType
    {
        Step,
        StepOver,
        StepOut
    }

    public delegate void DebuggerRunning(object sender, DebuggerRunningEventArgs e);
    public delegate void DebuggerStep(object sender, DebuggerStepEventArgs e);
    public delegate void DebuggerClosed(object sender, EventArgs e);

    public interface IWabbitcodeDebugger : IDisposable
    {
        event DebuggerRunning OnDebuggerRunningChanged;
        event DebuggerStep OnDebuggerStep;
        event DebuggerClosed OnDebuggerClosed;

        bool IsAnApp { get; }
        bool IsRunning { get; }
        Stack<Tuple<ushort, ushort>> MachineStack { get; }
        Stack<Tuple<ushort, string, string, DocumentLocation>> CallStack { get; }
        IZ80 CPU { get; }
        Image ScreenImage { get; }
        

        void InitDebugger(string outputFile);
        void CancelDebug();

        void GotoAddress(ushort address);

        bool SetBreakpoint(WabbitcodeBreakpoint breakpoint);
        void ClearBreakpoint(WabbitcodeBreakpoint newBreakpoint);

        void SetPCToSelect(string fileName, int lineNumber);

        void LaunchApp(string name);
        TIApplication? VerifyApp(string createdName);
        void ResetRom();

        void Step();
        void StepOut();
        void StepOver();
        void Run();
        void Pause();

        byte ReadByte(ushort address);
        ushort ReadShort(ushort address);
        ushort? GetRegisterValue(string wordHovered);
    }
}