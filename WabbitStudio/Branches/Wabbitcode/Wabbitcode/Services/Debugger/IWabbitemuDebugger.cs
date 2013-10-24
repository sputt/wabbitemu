using System;
using System.Drawing;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public delegate void BreakpointDelegate(IWabbitemu sender, BreakpointEventArgs e);
	public delegate void CloseDelegate(IWabbitemu sender, EventArgs e);

    public interface IWabbitemuDebugger : IDisposable
    {
        event BreakpointDelegate OnBreakpoint;
		event CloseDelegate OnClose;

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

	    Calc_Model Model { get; }

	    void LoadFile(string fileName);

        byte ReadByte(CalcAddress addresss);
        ushort ReadShort(CalcAddress addresss);
        byte[] Read(CalcAddress addresss, int count);
        byte ReadByte(bool isRam, byte page, ushort address);
        ushort ReadShort(bool isRam, byte page, ushort address);
        byte[] Read(bool isRam, byte page, ushort address, int count);

	    void Write(bool isRam, byte page, ushort address, byte value);
	    void Write(CalcAddress address, byte value);
		void Write(bool isRam, byte page, ushort address, ushort value);
		void Write(CalcAddress address, ushort value);
		void Write(bool isRam, byte page, ushort address, byte[] value);
		void Write(CalcAddress address, byte[] value);

        void ClearBreakpoint(IBreakpoint breakpoint);
        IBreakpoint SetBreakpoint(CalcAddress address);
        IBreakpoint SetBreakpoint(bool isRam, byte page, ushort address);

        void Step();

        Image GetScreenImage();
	    void CancelDebug();
    }
}