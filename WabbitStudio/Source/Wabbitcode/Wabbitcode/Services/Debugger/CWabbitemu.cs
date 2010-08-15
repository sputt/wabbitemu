using System;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services;
using System.Text;

namespace Revsoft.Wabbitcode.Classes
{
    public class CWabbitemu
    {
        readonly IWabbitemu pWabbitemu;
        readonly Process wabbit;
        public CWabbitemu(string file)
        {
#if !DEBUG
			try
			{
#endif
				Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
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
											 FileName = FileLocations.WabbitemuFile,
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
#if !DEBUG
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
#endif
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
#if !DEBUG
			try
			{
#endif
				pWabbitemu.KeyRelease(0, (uint)keyData);
#if !DEBUG
			}
			catch (COMException)
			{ }
#endif
        }
        public void releaseKeyPress(int keyData)
        {
            pWabbitemu.KeyRelease(0, (uint)keyData);
        }

        public Z80_State getState()
        {
#if !DEBUG
			try
			{
#endif
				Z80_State state;
				pWabbitemu.GetState(0, out state);
				return state;
#if !DEBUG
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
				return new Z80_State();
			}
			catch (Exception)
			{ }
			return new Z80_State();
#endif
        }

        public void setState(Z80_State state)
        {
#if !DEBUG
            try
            {
#endif
                pWabbitemu.SetState(0, ref state);
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
#endif
        }

        public void setBreakpoint(bool isInRAM, int page, int address)
        {
#if !DEBUG
            try
            {
#endif
                pWabbitemu.SetBreakpoint(0, DockingService.MainForm.Handle, Convert.ToInt16(isInRAM), (byte)page, (ushort)address);
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
#endif
        }
        public void setBreakpoint(breakpoint breakpoint)
        {
#if !DEBUG
            try
            {
#endif
				pWabbitemu.SetBreakpoint(0, DockingService.MainForm.Handle, breakpoint.IsRam, breakpoint.Page, breakpoint.Address);
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
#endif
        }

        public void clearBreakpoint(bool isInRAM, int page, int address)
        {
#if !DEBUG
			try
			{
#endif
				pWabbitemu.ClearBreakpoint(0, Convert.ToInt16(isInRAM), (byte)page, (ushort)address);
#if !DEBUG
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
			}
			catch (Exception ex)
			{
				return;
			}
#endif
        }
        public void clearBreakpoint(breakpoint breakpoint)
        {
#if !DEBUG
            try
            {
#endif
                pWabbitemu.ClearBreakpoint(0, breakpoint.IsRam, breakpoint.Page, breakpoint.Address);
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
#endif
        }

        public void step()
        {
#if !DEBUG
            try
            {
#endif
                pWabbitemu.Step(0);
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
#endif
        }

        public void stepOver()
        {
#if !DEBUG
            try
            {
#endif
                pWabbitemu.StepOver(0);
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
#endif
        }

		internal void stepOut()
		{
#if !DEBUG
			try
			{
#endif
				pWabbitemu.StepOut(0);
#if !DEBUG
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
			}
#endif
		}

		public void pause()
		{
#if !DEBUG
			try
			{
#endif
				pWabbitemu.Pause(0);
#if !DEBUG
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
			}
			catch (Exception ex)
			{
				return;
			}
#endif
		}

        public void run()
        {
#if !DEBUG
			try
			{
#endif
				pWabbitemu.Step(0);
				pWabbitemu.Run(0);
#if !DEBUG
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
			}
			catch (Exception ex)
			{
				return;
			}
#endif
        }

        public void getSymList()
        {

        }

        readonly AppEntry[] temp = new AppEntry[96];
        public AppEntry[] getAppList()
        {
#if !DEBUG
			try
			{
#endif
				uint count;
				pWabbitemu.GetAppList(0, temp, out count);
				return temp;
#if !DEBUG
			}
			catch (COMException ex)
			{
				return null;
			}
			catch (Exception ex)
			{
				return null;
			}
#endif
        }

        readonly byte[] screenArray = new byte[128 * 64];
        public Bitmap DrawScreen()
        {
#if !DEBUG
            try
            {
#endif
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
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
                return null;
            }
#endif
        }        

        public MEMSTATE getMemState()
        {
#if !DEBUG
			try
			{
#endif
				MEMSTATE memstate;
				pWabbitemu.GetMemState(0, out memstate);
				return memstate;
#if !DEBUG
			}
			catch (COMException ex)
			{
				MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
			}
			catch (Exception)
			{ }
			return new MEMSTATE();
#endif
        }

        public void setMemState(MEMSTATE state)
        {
#if !DEBUG
            try
            {
#endif
                pWabbitemu.SetMemState(0, ref state);
#if !DEBUG
            }
            catch (COMException ex)
            {
                MessageBox.Show("Error Talking to Wabbit!!!\n" + ex);
            }
#endif
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
            /*public override string ToString()
            {
                StringBuilder sb = new StringBuilder(name1);
                sb.Append(name2);
                sb.Append(name3);
                sb.Append(name4);
                sb.Append(name5);
                sb.Append(name6);
                sb.Append(name7);
                sb.Append(name8);
                return sb.ToString();
            }*/
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