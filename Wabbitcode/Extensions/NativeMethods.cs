using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Drawing;

namespace Revsoft.Wabbitcode.Classes
{
	public static class NativeMethods
	{
		[DllImport("user32.dll")]
		public static extern bool SetForegroundWindow(IntPtr hWnd);

		[DllImport("user32.dll", CharSet = CharSet.Auto)]
		public static extern IntPtr FindWindow(string strClassName, string nptWindowName);

		[DllImport("user32.dll")]
		public static extern short GetKeyState(int key);

		private const int WM_SETREDRAW = 0x000B;
		private const int WM_USER = 0x400;
		private const int EM_GETEVENTMASK = (WM_USER + 59);
		private const int EM_SETEVENTMASK = (WM_USER + 69);
		private const int EM_GETSCROLLPOS = (WM_USER + 221);
		private const int EM_SETSCROLLPOS = (WM_USER + 222);
		[DllImport("user32.dll", CharSet = CharSet.Auto)]
		private extern static int SendMessage(IntPtr hWnd, int msg, int wParam, ref Point lParam);

        [DllImport("kernel32.dll")]
        public static extern IntPtr GetStdHandle(uint nStdHandle);

        [DllImport("Kernel32.dll")]//SetLastError = true
        public static extern bool SetStdHandle(uint device, IntPtr handle);
        //[DllImport("Kernel32.dll")]//SetLastError = true
        //public static extern uint GetStdHandle(uint device);

		public static void TurnOffDrawing(IntPtr handle)
		{
			Point temp = new Point(0, 0);
			SendMessage(handle, WM_SETREDRAW, 0, ref temp);
			SendMessage(handle, EM_GETEVENTMASK, 0, ref temp);
		}

		public static void TurnOnDrawing(IntPtr handle)
		{
			Point temp = new Point(0, 0);
			SendMessage(handle, EM_SETEVENTMASK, 0, ref temp);
			SendMessage(handle, WM_SETREDRAW, 1, ref temp);
		}

		public static Point GetScrollPos(IntPtr handle)
		{
			Point scrollPos = new Point(0, 0);
			SendMessage(handle, EM_GETSCROLLPOS, 0, ref scrollPos);
			return scrollPos;
		}

		public static void SaveScrollPos(IntPtr handle, Point scrollPos)
		{
			SendMessage(handle, EM_SETSCROLLPOS, 0, ref scrollPos);
		}
	}

    public static class SpasmMethods
    {
        [DllImport("SPASM.dll")]
        public static extern int ShowMessage();

        [DllImport("SPASM.dll")]
        public static extern int SetInputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll")]
        public static extern int SetOutputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll")]
        public static extern int RunAssembly();

        [DllImport("SPASM.dll")]
        public static extern int ClearDefines();

        [DllImport("SPASM.dll")]
        public static extern int AddDefine([In, MarshalAs(UnmanagedType.LPStr)] string name,
                                           [In, MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport("SPASM.dll")]
        public static extern int ClearIncludes();

        [DllImport("SPASM.dll")]
        public static extern int AddInclude([In, MarshalAs(UnmanagedType.LPStr)] string directory);

        [DllImport("SPASM.dll")]
        public static extern int SetMode(int mode);

        [DllImport("SPASM.dll")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        public static extern string GetStdOut();
    }
}
