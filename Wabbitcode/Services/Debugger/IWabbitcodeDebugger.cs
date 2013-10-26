using System;
using System.Collections.Generic;
using System.Drawing;
using Revsoft.Wabbitcode.Utils;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class StackEntry
    {
        public ushort Address { get; private set; }
        public ushort Data { get; private set; }
        public CallStackEntry CallStackEntry { get; set; }

        public StackEntry(ushort address, ushort data, CallStackEntry entry)
        {
            Address = address;
            Data = data;
            CallStackEntry = entry;
        }
    }

    public class CallStackEntry
    {
        public string CallType { get; private set; }
        public string CallName { get; private set; }
        public DocumentLocation CallLocation { get; private set; }

        public CallStackEntry(string callType, string callName, DocumentLocation location)
        {
            CallType = callType;
            CallName = callName;
            CallLocation = location;
        }
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
        Stack<StackEntry> MachineStack { get; }
        Stack<CallStackEntry> CallStack { get; }
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