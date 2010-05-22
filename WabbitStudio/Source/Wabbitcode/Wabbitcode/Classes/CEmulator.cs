using System;
using System.Runtime.InteropServices;
using System.Drawing;

namespace Revsoft.Wabbitcode
{
    public class Emulator
    {
        private int slot;
        public Emulator(string romFile)
        {
            slot = calc_new();
            calc_load(slot, romFile);
        }

        ~Emulator()
        {
            calc_free(slot);
        }

        public void LoadFile(string filePath)
        {
            calc_load(slot, filePath);
        }

        public void SetLCD(IntPtr hwndLCD)
        {
            calc_set_lcd(slot, hwndLCD);
        }

        public void RunCalc()
        {
            calc_run(slot);
        }

        public void PauseCalc()
        {
            calc_pause(slot);
        }

        public void SendKeypress(int keycode)
        {
            calc_key_press(slot, keycode);
        }

        public void ReleaseKeypress(int keycode)
        {
            calc_key_release(slot, keycode);
        }

        public byte ReadMem(ushort address)
        {
            return calc_read_mem(slot, address);
        }

        public void WriteMem(ushort address, byte value)
        {
            calc_write_mem(slot, address, value);
        }

        public void SetBreakpoint(IntPtr handle, bool isRam, byte page, ushort address)
        {
            calc_set_breakpoint(slot, handle, isRam, page, address);
        }

        public void ClearBreakpoint(bool isRam, byte page, ushort address)
        {
            calc_clear_breakpoint(slot, isRam, page, address);
        }

        byte[] screenArray = new byte[128 * 64];
        public Image DrawScreen()
        {
            IntPtr screenSource = calc_draw_screen(slot);
            Marshal.Copy(screenSource, screenArray, 0, 128 * 64);
            return DrawScreen(screenArray);
        }
        public void DrawScreen(IntPtr handle)
        {
            calc_set_draw_screen(slot, handle);
        }
        private Image DrawScreen(byte[] screenArray)
        {
            int counter = 0;
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
            return screen;
        }

        public Applist GetApplist()
        {
            IntPtr listPointer = calc_get_applist(slot);
            Applist list = new Applist();
            byte[] tempArray = new byte[(20 * 96)+1];
            Marshal.Copy(listPointer, tempArray, 0, 96*20);
            list.count = (uint)(tempArray[0] + tempArray[1]*256 + tempArray[2]*4096 + tempArray[3]*65536);
            return list;
        }

        public Z80_State GetState()
        {
            return calc_get_state(slot);
        }

        public void SetState(Z80_State state)
        {
            calc_set_state(slot, state);
        }

        public MEMSTATE GetMemState()
        {
            return calc_get_memstate(slot);
        }

        public void SetMemState(MEMSTATE state)
        {
            calc_set_memstate(slot, state);
        }

        public void Step()
        {
            calc_step(slot);
        }

        public void StepOver()
        {
            calc_step_over(slot);
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

        public struct MEMSTATE
        {
            public int ram0;
            public uint page0;
            public int ram1;
            public uint page1;
            public int ram2;
            public uint page2;
            public int ram3;
            public uint page3;
        }

        #region WABBITEMU_DEFINES
        [DllImport("libWabbitemu.dll", EntryPoint = "calc_new@0")]
        private static extern int calc_new();

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_load@8")]
        public static extern void calc_load(int slot, [In, MarshalAs(UnmanagedType.LPStr)] string fileName);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_run@4")]
        public static extern void calc_run(int slot);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_free@4")]
        public static extern void calc_free(int slot);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_pause@4")]
        public static extern void calc_pause(int slot);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_set_lcd@8")]
        public static extern void calc_set_lcd(int slot, IntPtr hwndLCD);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_key_press@8")]
        public static extern int calc_key_press(int slot, int keycode);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_key_release@8")]
        public static extern int calc_key_release(int slot, int keycode);

        [DllImport("libWabbitemu.dll")]
        public static extern Z80_State calc_get_state(int slot);

        [DllImport("libWabbitemu.dll")]
        public static extern void calc_set_state(int slot, Z80_State zs);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_read_mem@8")]
        public static extern byte calc_read_mem(int slot, UInt16 address);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_write_mem@12")]
        public static extern void calc_write_mem(int slot, UInt16 address, byte value);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_draw_screen@4")]
        public static extern IntPtr calc_draw_screen(int slot);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_set_draw_screen@8")]
        public static extern void calc_set_draw_screen(int slot, IntPtr handle);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_set_breakpoint@20")]
        public static extern int calc_set_breakpoint(int slot, IntPtr hwndCallBack, bool isRam, byte page, UInt16 address);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_clear_breakpoint@16")]
        public static extern int calc_clear_breakpoint(int slot, bool isRam, byte page, UInt16 address);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_get_applist@4")]
        public static extern IntPtr calc_get_applist(int slot);

        /*[DllImport("libWabbitemu.dll")]
        public static extern symlist_t calc_get_symlist(int slot);*/

        [DllImport("libWabbitemu.dll")]
        public static extern MEMSTATE calc_get_memstate(int slot);

        [DllImport("libWabbitemu.dll")]
        public static extern void calc_set_memstate(int slot, MEMSTATE mem);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_step@4")]
        public static extern void calc_step(int slot);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_step_out@4")]
        public static extern void calc_step_out(int slot);

        [DllImport("libWabbitemu.dll", EntryPoint = "calc_step_over@4")]
        public static extern void calc_step_over(int slot);
        #endregion
    }

    public class Applist
    {
        public uint count { get; set; }
        public AppHeader[] apps = new AppHeader[96];

    }

    public class AppHeader
    {
        public char[] name {
            get { return s_name; }
            set { s_name = value; }
        }
        private char[] s_name = new char[12];
        public uint page { get; set; }
        public uint page_count { get; set; }
    }
}
