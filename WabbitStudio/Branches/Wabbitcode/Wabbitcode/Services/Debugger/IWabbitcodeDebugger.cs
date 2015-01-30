using System;
using System.Collections.Generic;
using System.Drawing;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class StackEntry
    {
        public ushort Address { get; private set; }
        public ushort Data { get; private set; }
        public CallStackEntry CallStackEntry { get; private set; }

        public StackEntry(ushort address, ushort data, CallStackEntry entry)
        {
            Address = address;
            Data = data;
            CallStackEntry = entry;
        }
    }

    public class CallStackEntry
    {
        public CallerInformation CallerInformation { get; private set; }
        public DocumentLocation CallLocation { get; private set; }

        public CallStackEntry(CallerInformation callerInfo, DocumentLocation location)
        {
            CallerInformation = callerInfo;
            CallLocation = location;
        }
    }

    public delegate void DebuggerRunning(object sender, DebuggerRunningEventArgs e);

    public delegate void DebuggerStep(object sender, DebuggerStepEventArgs e);

    public interface IWabbitcodeDebugger : IDisposable
    {
        event DebuggerRunning DebuggerRunningChanged;
        event DebuggerStep DebuggerStep;

        bool IsRunning { get; }
        Stack<StackEntry> MachineStack { get; }
        Stack<CallStackEntry> CallStack { get; }
        IZ80 CPU { get; }
        Image ScreenImage { get; }

        void StartDebug();
        void EndDebug();

        DocumentLocation GetAddressLocation(ushort address);

        bool SetBreakpoint(WabbitcodeBreakpoint breakpoint);
        void ClearBreakpoint(WabbitcodeBreakpoint newBreakpoint);

        void SetPCToSelect(FilePath fileName, int lineNumber);

        void Step();
        void StepOut();
        void StepOver();
        void Run();
        void Pause();

        byte ReadByte(ushort address);
        ushort ReadShort(ushort address);
        byte[] ReadMemory(ushort address, ushort count);
        ushort? GetRegisterValue(string wordHovered);
    }
}