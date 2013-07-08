namespace Revsoft.Wabbitcode.Services.Debugger
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Drawing;
    using System.Linq;
    using System.Text;

    using Microsoft.Win32;

    using Revsoft.Wabbitcode.Exceptions;

    using WabbitemuLib;
    using System.Threading.Tasks;

    public class WabbitemuDebugger : IWabbitemuDebugger
    {
        private Wabbitemu debugger;

        public WabbitemuDebugger()
        {
            RegistryKey romKey = null;
            string romFile = string.Empty;
            try
            {
                romKey = Registry.CurrentUser.OpenSubKey("Software\\Wabbitemu");
                romFile = romKey.GetValue("rom_path").ToString();
            }
            catch (Exception)
            {
                throw new MissingROMException("Could not load Wabbitemu ROM");
            }
            finally
            {
                if (romKey != null)
                {
                    romKey.Close();
                }
            }

            this.debugger = new Wabbitemu();
            this.debugger.LoadFile(romFile);
            this.debugger.Breakpoint += this.debugger_Breakpoint;
        }

        public event BreakpointDelegate OnBreakpoint;

        public Array Apps
        {
            get
            {
                return this.debugger.Apps;
            }
        }

        public IZ80 CPU
        {
            get
            {
                return this.debugger.CPU;
            }
        }

        public IKeypad Keypad
        {
            get
            {
                return this.debugger.Keypad;
            }
        }

        public ILCD LCD
        {
            get
            {
                return this.debugger.LCD;
            }
        }

        public IMemoryContext Memory
        {
            get
            {
                return debugger.Memory;
            }
        }

        // public Array Symbols
        // {
        //    get { return debugger.Symbols; }
        // }
        public bool Running
        {
            get
            {
                return this.debugger.Running;
            }
            set
            {
                if (this.debugger.Running)
                {
                    this.debugger.Break();
                }
                else
                {
                    this.debugger.Run();
                }
            }
        }

        public bool Visible
        {
            get
            {
                return this.debugger.Visible;
            }
            set
            {
                this.debugger.Visible = value;
            }
        }

        public void ClearBreakpoint(IBreakpoint breakpoint)
        {
            this.debugger.Breakpoints.Remove(breakpoint);
        }

        public void LoadFile(string fileName)
        {
            this.debugger.LoadFile(fileName);
        }

        public byte ReadByte(bool isRam, byte page, ushort address)
        {
            IPage calcPage = null;
            if (isRam)
            {
                calcPage = this.debugger.Memory.RAM[page];
            }
            else
            {
                calcPage = this.debugger.Memory.Flash[page];
            }

            return calcPage.ReadByte(address);
        }

        public ushort ReadShort(bool isRam, byte page, ushort address)
        {
            IPage calcPage = null;
            if (isRam)
            {
                calcPage = this.debugger.Memory.RAM[page];
            }
            else
            {
                calcPage = this.debugger.Memory.Flash[page];
            }

            return calcPage.ReadByte(address);
        }

        public byte[] Read(bool isRam, byte page, ushort address, int count)
        {
            IPage calcPage = null;
            if (isRam)
            {
                calcPage = this.debugger.Memory.RAM[page];
            }
            else
            {
                calcPage = this.debugger.Memory.Flash[page];
            }

            return (byte []) calcPage.Read(address, (ushort) count);
        }

        public byte ReadByte(CalcAddress address)
        {
            return address.Page.ReadByte(address.Address);
        }

        public ushort ReadShort(CalcAddress address)
        {
            return address.Page.ReadByte(address.Address);
        }

        public byte[] Read(CalcAddress address, int count)
        {
            return (byte []) address.Page.Read(address.Address, (ushort) count);
        }

        public IBreakpoint SetBreakpoint(CalcAddress address)
        {
            return this.debugger.Breakpoints.Add(address);
        }

        public IBreakpoint SetBreakpoint(bool isRam, byte page, ushort address)
        {
            CalcAddress calcAddr = new CalcAddress();
            IPage calcPage = null;
            if (isRam)
            {
                calcPage = this.debugger.Memory.RAM[page];
            }
            else
            {
                calcPage = this.debugger.Memory.Flash[page];
            }

            calcAddr.Initialize(calcPage, address);
            return this.debugger.Breakpoints.Add(calcAddr);
        }

        public void Step()
        {
            this.debugger.Step();
        }

        public void StepOut()
        {
            throw new NotImplementedException();
        }

        public void StepOver()
        {
            this.debugger.StepOver();
        }

        public void Write(bool isRam, byte page, ushort address, object value)
        {
            CalcAddress calcAddr = new CalcAddress();
            IPage calcPage = null;
            if (isRam)
            {
                calcPage = this.debugger.Memory.RAM[page];
            }
            else
            {
                calcPage = this.debugger.Memory.Flash[page];
            }

            calcAddr.Initialize(calcPage, address);
            //calcAddr.Write(value);
        }

        public void Write(ICalcAddress address, object value)
        {
            //address.Write(value);
        }

        private void debugger_Breakpoint(IWabbitemu debugger, IBreakpoint breakpoint)
        {
            if (this.OnBreakpoint != null)
            {
                this.OnBreakpoint(breakpoint);
            }
        }
    }
}