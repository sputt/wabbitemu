using System;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Classes
{
    public class CWabbitemu
    {
        readonly IWabbitemu pWabbitemu;
        readonly Process wabbit;
        public CWabbitemu(string file)
        {
            Resources.GetResource("Revsoft.Wabbitcode.Resources.Wabbitemu.exe", "Wabbitemu.exe");
            wabbit = null;
            foreach (Process potential in Process.GetProcesses())
            {
                if (!potential.ProcessName.ToLower().Contains("wabbitemu")) 
                    continue;
                wabbit = potential;
                break;
            }
            if (wabbit == null)
            {
                wabbit = new Process
                             {
                                 StartInfo =
                                     {
                                         Arguments = "\"" + file + "\"",
                                         FileName = "wabbitemu.exe"
                                     },
                                     EnableRaisingEvents = true
                             };
                wabbit.Start();
            }
            wabbit.Exited += wabbit_Exited;
            wabbit.WaitForInputIdle();
            Guid CLSID_Wabbitemu = new Guid("8cc953bc-a879-492b-ad22-a2f4dfcd0e19");
            //Guid IID_IWabbitemu = new Guid("13b5c004-4377-4c94-a8f9-efc1fdaeb31c");

            Type dcomType = Type.GetTypeFromCLSID(CLSID_Wabbitemu);
            Object dcomObj = Activator.CreateInstance(dcomType);
            pWabbitemu = (IWabbitemu)dcomObj;
        }

        delegate void CancelDebugCallback();
        private static void wabbit_Exited(object sender, EventArgs e)
        {
            CancelDebugCallback d = DebuggerService.CancelDebug;
            DockingService.MainForm.Invoke(d, null);
        }

        public void Close()
        {
			wabbit.Exited -= wabbit_Exited;
            if (!wabbit.HasExited)
                wabbit.Kill();
        }

        public byte readMem(ushort address)
        {
            byte value;
            pWabbitemu.MemRead(0, address, out value);
            return value;
        }

        public void sendKeyPress(Keys keyData)
        {
            Thread.Sleep(100);
            pWabbitemu.KeyPress(0, (uint)keyData);
        }
        public void sendKeyPress(int keyData)
        {
            Thread.Sleep(100);
            pWabbitemu.KeyPress(0, (uint)keyData);
        }

        public void releaseKeyPress(Keys keyData)
        {
			try
			{
				pWabbitemu.KeyRelease(0, (uint)keyData);
			}
			catch (COMException)
			{ }
        }
        public void releaseKeyPress(int keyData)
        {
            pWabbitemu.KeyRelease(0, (uint)keyData);
        }

        public Z80_State getState()
        {
			try
			{
				Z80_State state;
				pWabbitemu.GetState(0, out state);
				return state;
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
				return new Z80_State();
			}
			catch (Exception)
			{ }
			return new Z80_State();
        }

        public void setState(Z80_State state)
        {
            try
            {
                pWabbitemu.SetState(0, ref state);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }

        public void setBreakpoint(bool isInRAM, int page, int address)
        {
            try
            {
                pWabbitemu.SetBreakpoint(0, DockingService.MainForm.Handle, Convert.ToInt16(isInRAM), (byte)page, (ushort)address);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }
        public void setBreakpoint(breakpoint breakpoint)
        {
            try
            {
				pWabbitemu.SetBreakpoint(0, DockingService.MainForm.Handle, breakpoint.IsRam, breakpoint.Page, breakpoint.Address);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }

        public void clearBreakpoint(bool isInRAM, int page, int address)
        {
            try
            {
                pWabbitemu.ClearBreakpoint(0, Convert.ToInt16(isInRAM), (byte)page, (ushort)address);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }
        public void clearBreakpoint(breakpoint breakpoint)
        {
            try
            {
                pWabbitemu.ClearBreakpoint(0, breakpoint.IsRam, breakpoint.Page, breakpoint.Address);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }

        public void step()
        {
            try
            {
                pWabbitemu.Step(0);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }

        public void stepOver()
        {
            try
            {
                pWabbitemu.StepOver(0);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }

		internal void stepOut()
		{
			try
			{
				pWabbitemu.StepOut(0);
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
			}
		}

		public void pause()
		{
			try
			{
				pWabbitemu.Pause(0);
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
			}
		}

        public void run()
        {
            try
            {
                pWabbitemu.Step(0);
                pWabbitemu.Run(0);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }

        public void getSymList()
        {

        }

        readonly AppEntry[] temp = new AppEntry[96];
        public AppEntry[] getAppList()
        {
            try
            {
                uint count;
                pWabbitemu.GetAppList(0, temp, out count);
                return temp;
            }
            catch (COMException ex)
            {
                return null;
            }
        }

        readonly byte[] screenArray = new byte[128 * 64];
        public Bitmap DrawScreen()
        {
            try
            {
                int counter = 0;
                pWabbitemu.DrawScreen(0, screenArray);
                Bitmap screen = new Bitmap(128, 64);
                for (int y = 0; y < 64; y++)
                {
                    for (int x = 0; x < 128; x++)
                    {

                        int pixColor = 255 - screenArray[counter];
                        int alpha = pixColor == 231 ? 0 : 255;
                        screen.SetPixel(x, y, Color.FromArgb(alpha, pixColor, pixColor, pixColor));
                        counter++;
                    }
                }
                Bitmap result = screen.ResizeImage(256, 128);
                return result;
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
                return null;
            }
        }        

        public MEMSTATE getMemState()
        {
			try
			{
				MEMSTATE memstate;
				pWabbitemu.GetMemState(0, out memstate);
				return memstate;
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
				return new MEMSTATE();
			}
			catch (Exception)
			{ }
			return new MEMSTATE();
        }

        public void setMemState(MEMSTATE state)
        {
            try
            {
                pWabbitemu.SetMemState(0, ref state);
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
        }

        public struct Z80_State
        {
            public ushort AF, BC, HL, DE;
            public ushort AFP, BCP, HLP, DEP;
            public ushort IX, IY, PC, SP;
            public byte I, R, Bus, IFF1, IFF2;
            public byte IMode;
            public double secondsElapsed;
            public int Freq;
            public int Halt;
        }
        public struct breakpoint
        {
            public int IsRam;
            public byte Page;
            public ushort Address;
        }

        public struct AppEntry
        {
            public char name1;
            public char name2;
            public char name3;
            public char name4;
            public char name5;
            public char name6;
            public char name7;
            public char name8;
            char name9;
            char name10;
            char name11;
            char name12;
            //[MarshalAs(UnmanagedType.LPStr, SizeConst=12)]
            //public string name;
            public int page;
            public int page_count;
        }

        public struct SymEntry
        {
            Int32 TypeID;
            string name;
            Int32 page;
            Int32 addresss;
        }

        public struct MEMSTATE
        {
            public int ram0;
            public byte page0;
            public int ram1;
            public byte page1;
            public int ram2;
            public byte page2;
            public int ram3;
            public byte page3;
        }
        [Guid("13b5c004-4377-4c94-a8f9-efc1fdaeb31c"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        interface IWabbitemu
        {
            void Run([In] uint uSlot);
            void GetState([In] uint uSlot, [ComAliasName("WabbitemuLib.Z80State_t")] out Z80_State pZ80State);
            void SetState([In] uint uSlot, [In, ComAliasName("WabbitemuLib.Z80State_t")] ref Z80_State pZ80State);
            void KeyPress([In] uint uSlot, [In] uint vk);
            void KeyRelease([In] uint uSlot, [In] uint vk);
            void MemRead([In] uint uSlot, [In] ushort Address, out byte Value);
            void MemWrite([In] uint uSlot, [In] ushort Address, [In] byte Value);
            void DrawScreen([In] uint uSlot, [Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0, SizeConst = 0x2000)] byte[] Image);
            void SetBreakpoint([In] uint uSlot, [In] IntPtr hwndCallback, [In] int IsRam, [In] byte Page, [In] ushort Address);
            void ClearBreakpoint([In] uint uSlot, [In] int IsRam, [In] byte Page, [In] ushort Address);
            void GetAppList([In] uint uSlot, [Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0, SizeConst = 0x60)] AppEntry[] AppList, out uint Count);
            void GetSymList([In] uint uSlot, [Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0, SizeConst = 0x100)] SymEntry[] SymList, out uint Count);
            void GetMemState([In] uint uSlot, [ComAliasName("WabbitemuLib.MEMSTATE")] out MEMSTATE pMemstate);
            void SetMemState([In] uint uSlot, [In, ComAliasName("WabbitemuLib.MEMSTATE")] ref MEMSTATE pMemstate);
            void Step([In] uint uSlot);
            void StepOver([In] uint uSlot);
            void StepOut([In] uint uSlot);
			void Pause([In] uint uSlot);
        }
	}
}