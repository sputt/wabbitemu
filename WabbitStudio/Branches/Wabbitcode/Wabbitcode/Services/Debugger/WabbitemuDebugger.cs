using System.Diagnostics;
using System.Drawing.Imaging;
using System.IO;
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
        private Wabbitemu _debugger;
        private bool _disposed;
        private readonly IntPtr _scan0 = Marshal.AllocHGlobal(128 * 64);

        public WabbitemuDebugger()
        {
            var romFile = GetRomFile();

            if (romFile == null || !File.Exists(romFile))
            {
                throw new MissingRomException("Could not load Wabbitemu ROM");
            }

            Debug.WriteLine("Creating wabbitemu");
            _debugger = new Wabbitemu();
            Debug.WriteLine("Loading rom file " + romFile);
            _debugger.LoadFile(romFile);
            _debugger.Breakpoint += debugger_Breakpoint;
            _debugger.Close += debugger_Close;
        }

        private static string GetRomFile()
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
                throw new MissingRomException("Could not load Wabbitemu ROM");
            }
            finally
            {
                if (romKey != null)
                {
                    romKey.Close();
                }
            }
            return romFile;
        }

        public event BreakpointEventHandler OnBreakpoint;
        public event CloseEventHandler OnClose;

        public ITIApplicationCollection Apps
        {
            get { return _debugger.Apps; }
        }

        public ITISymbolCollection Symbols
        {
            get { return _debugger.Symbols; }
        }

        public IZ80 CPU
        {
            get { return _debugger.CPU; }
        }

        public IKeypad Keypad
        {
            get { return _debugger.Keypad; }
        }

        public ILCD LCD
        {
            get { return _debugger.LCD; }
        }

        public IMemoryContext Memory
        {
            get { return _debugger.Memory; }
        }

        public bool Running
        {
            get { return _debugger.Running; }
            set
            {
                if (value)
                {
                    _debugger.Run();
                }
                else
                {
                    _debugger.Break();
                }
            }
        }

        public bool Visible
        {
            get { return _debugger.Visible; }
            set { _debugger.Visible = value; }
        }

        public void ClearBreakpoint(IBreakpoint breakpoint)
        {
            _debugger.Breakpoints.Remove(breakpoint);
        }

        public CalcModel Model
        {
            get { return _debugger.Model; }
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

            return (byte[]) calcPage.Read(address, (ushort) count);
        }

        public byte ReadByte(CalcAddress address)
        {
            return address == null ? (byte) 0 : address.Page.ReadByte(address.Address);
        }

        public ushort ReadShort(CalcAddress address)
        {
            return address == null ? (ushort) 0 : address.Page.ReadByte(address.Address);
        }

        public byte[] Read(CalcAddress address, int count)
        {
            if (address == null)
            {
                return null;
            }

            return (byte[]) address.Page.Read(address.Address, (ushort) count);
        }

        public IBreakpoint SetBreakpoint(CalcAddress address)
        {
            return _debugger.Breakpoints.Add(address);
        }

        public IBreakpoint SetBreakpoint(bool isRam, byte page, ushort address)
        {
            Debug.WriteLine("Setting breakpoint " + isRam + " " + page + " " + address);
            CalcAddress calcAddr = new CalcAddress();
            IPage calcPage = GetCalcPage(isRam, page);

            calcAddr.Initialize(calcPage, address);
            return _debugger.Breakpoints.Add(calcAddr);
        }

        public void Step()
        {
            _debugger.Step();
        }

        public void Write(bool isRam, byte page, ushort address, byte value)
        {
            IPage calcPage = isRam ? _debugger.Memory.RAM[page] : _debugger.Memory.Flash[page];
            calcPage.WriteByte(address, value);
        }

        public void Write(CalcAddress address, byte value)
        {
            if (address == null)
            {
                return;
            }

            address.Page.WriteByte(address.Address, value);
        }

        public void Write(bool isRam, byte page, ushort address, ushort value)
        {
            IPage calcPage = isRam ? _debugger.Memory.RAM[page] : _debugger.Memory.Flash[page];
            calcPage.WriteWord(address, value);
        }

        public void Write(CalcAddress address, ushort value)
        {
            if (address == null)
            {
                return;
            }

            address.Page.WriteWord(address.Address, value);
        }

        public void Write(bool isRam, byte page, ushort address, byte[] value)
        {
            IPage calcPage = isRam ? _debugger.Memory.RAM[page] : _debugger.Memory.Flash[page];
            calcPage.Write(address, value);
        }

        public void Write(CalcAddress address, byte[] value)
        {
            if (address == null)
            {
                return;
            }

            address.Page.Write(address.Address, value);
        }

        public Image GetScreenImage()
        {
            Marshal.Copy((byte[]) _debugger.LCD.Display, 0, _scan0, 128 * 64);
            using (Bitmap calcBitmap = new Bitmap(128, 64, 128, PixelFormat.Format8bppIndexed, _scan0))
            {
                var palette = calcBitmap.Palette;
                for (int i = 0; i <= 255; i++)
                {
                    palette.Entries[i] = Color.FromArgb(0x9e * (256 - i) / 255, (0xAB * (256 - i)) / 255, (0x88 * (256 - i)) / 255);
                }

                Rectangle rect = new Rectangle(0, 0, _debugger.LCD.Width, _debugger.LCD.Height);
                Bitmap cropped = calcBitmap.Clone(rect, calcBitmap.PixelFormat);
                cropped.Palette = palette;
                return cropped;
            }
        }

        public void EndDebug()
        {
            if (_debugger == null)
            {
                return;
            }
            try
            {
                _debugger.Break();
                _debugger.Visible = false;
                _debugger.Breakpoint -= debugger_Breakpoint;
                _debugger.Close -= debugger_Close;
            }
            catch (InvalidCastException)
            {
                Debug.WriteLine("COM exception canceling debug");
            }
            _debugger = null;
        }

        public void TurnCalcOn()
        {
            _debugger.TurnCalcOn();
        }

        private void debugger_Breakpoint(IWabbitemu debugger, IBreakpoint breakpoint)
        {
            if (OnBreakpoint != null)
            {
                OnBreakpoint(debugger, new BreakpointEventArgs(breakpoint));
            }
        }

        private void debugger_Close(Wabbitemu calc)
        {
            if (OnClose != null)
            {
                OnClose(calc, EventArgs.Empty);
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

                Marshal.FreeHGlobal(_scan0);
                _disposed = true;
            }
        }
    }
}