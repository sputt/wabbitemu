using System.Drawing.Imaging;
using Microsoft.Win32;
using Revsoft.Wabbitcode.Exceptions;
using System;
using System.Drawing;
using System.Runtime.InteropServices;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public sealed class WabbitemuDebugger : IWabbitemuDebugger
    {
        private readonly Wabbitemu _debugger;
        private bool _disposed;

        public WabbitemuDebugger()
        {
            RegistryKey romKey = null;
            string romFile = null;
            try
            {
	            romKey = Registry.CurrentUser.OpenSubKey("Software\\Wabbitemu");
				if (romKey != null)
				{
					romFile = romKey.GetValue("rom_path").ToString();
				}
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

	        if (romFile == null)
			{
				throw new MissingROMException("Could not load Wabbitemu ROM");
			}

            _debugger = new Wabbitemu();
            _debugger.LoadFile(romFile);
            _debugger.Breakpoint += debugger_Breakpoint;
        }

        public event BreakpointDelegate OnBreakpoint;

        public Array Apps
        {
            get
            {
                return _debugger.Apps;
            }
        }

        public IZ80 CPU
        {
            get
            {
                return _debugger.CPU;
            }
        }

        public IKeypad Keypad
        {
            get
            {
                return _debugger.Keypad;
            }
        }

        public ILCD LCD
        {
            get
            {
                return _debugger.LCD;
            }
        }

        public IMemoryContext Memory
        {
            get
            {
                return _debugger.Memory;
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
                return _debugger.Running;
            }
            set
            {
                if (_debugger.Running)
                {
                    _debugger.Break();
                }
                else
                {
                    _debugger.Run();
                }
            }
        }

        public bool Visible
        {
            get
            {
                return _debugger.Visible;
            }
            set
            {
                _debugger.Visible = value;
            }
        }

        public void ClearBreakpoint(IBreakpoint breakpoint)
        {
            _debugger.Breakpoints.Remove(breakpoint);
        }

        public void LoadFile(string fileName)
        {
            _debugger.LoadFile(fileName);
        }

        public byte ReadByte(bool isRam, byte page, ushort address)
        {
	        IPage calcPage = GetCalcPage(isRam, page);

	        return calcPage.ReadByte(address);
        }

	    public ushort ReadShort(bool isRam, byte page, ushort address)
        {
			IPage calcPage = GetCalcPage(isRam, page);

	        return calcPage.ReadByte(address);
        }

	    public byte[] Read(bool isRam, byte page, ushort address, int count)
        {
			IPage calcPage = GetCalcPage(isRam, page);

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
            return _debugger.Breakpoints.Add(address);
        }

        public IBreakpoint SetBreakpoint(bool isRam, byte page, ushort address)
        {
            CalcAddress calcAddr = new CalcAddress();
	        IPage calcPage = GetCalcPage(isRam, page);

            calcAddr.Initialize(calcPage, address);
            return _debugger.Breakpoints.Add(calcAddr);
        }

        public void Step()
        {
            _debugger.Step();
        }

        public void StepOut()
        {
            throw new NotImplementedException();
        }

        public void StepOver()
        {
            _debugger.StepOver();
        }

        public void Write(bool isRam, byte page, ushort address, object value)
        {
            CalcAddress calcAddr = new CalcAddress();
            IPage calcPage = null;
            if (isRam)
            {
                calcPage = _debugger.Memory.RAM[page];
            }
            else
            {
                calcPage = _debugger.Memory.Flash[page];
            }

            calcAddr.Initialize(calcPage, address);
            //calcAddr.Write(value);
        }

        public void Write(ICalcAddress address, object value)
        {
            //address.Write(value);
        }

        IntPtr scan0 = Marshal.AllocHGlobal(128 * 64);
        public Image GetScreenImage()
        {
            Marshal.Copy((byte[])_debugger.LCD.Display, 0, scan0, 128 * 64);
            Bitmap calcBitmap = new Bitmap(128, 64, 128, PixelFormat.Format8bppIndexed, scan0);
            var palette = calcBitmap.Palette;
            for (int i = 0; i < 255; i++)
            {
                palette.Entries[i] = Color.FromArgb(0x9e * (256 - i) / 255, (0xAB * (256 - i)) / 255, (0x88 * (256 - i)) / 255);
            }

            calcBitmap.Palette = palette;
            return calcBitmap;
        }

        private void debugger_Breakpoint(IWabbitemu debugger, IBreakpoint breakpoint)
        {
            if (OnBreakpoint != null)
            {
                OnBreakpoint(debugger, new BreakpointEventArgs(breakpoint));
            }
        }

		private IPage GetCalcPage(bool isRam, byte page)
		{
			return isRam ? _debugger.Memory.RAM[page] : _debugger.Memory.Flash[page];
		}

        ~WabbitemuDebugger()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

	    private void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                if (disposing)
                {

                }

                Marshal.FreeHGlobal(scan0);
                _disposed = true;
            }
        }
    }
}