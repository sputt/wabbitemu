using System;
using System.Drawing;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public delegate void BreakpointDelegate(IWabbitemu sender, BreakpointEventArgs e);

    public interface IWabbitemuDebugger : IDisposable
    {
        event BreakpointDelegate OnBreakpoint;

        Array Apps
        {
            get;
        }

        IZ80 CPU
        {
            get;
        }

        IKeypad Keypad
        {
            get;
        }

        ILCD LCD
        {
            get;
        }

        IMemoryContext Memory
        {
            get;
        }

        bool Running
        {
            get;
            set;
        }

        bool Visible
        {
            get;
            set;
        }

        void LoadFile(string fileName);

        byte ReadByte(CalcAddress addresss);
        ushort ReadShort(CalcAddress addresss);
        byte[] Read(CalcAddress addresss, int count);
        byte ReadByte(bool isRam, byte page, ushort address);
        ushort ReadShort(bool isRam, byte page, ushort address);
        byte[] Read(bool isRam, byte page, ushort address, int count);

        void ClearBreakpoint(IBreakpoint breakpoint);
        IBreakpoint SetBreakpoint(CalcAddress address);
        IBreakpoint SetBreakpoint(bool isRam, byte page, ushort address);

        void Step();
        void StepOut();
        void StepOver();

        Image GetScreenImage();
	    void CancelDebug();
    }
}