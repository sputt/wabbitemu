namespace Wabbitcode
{
    using System;
    using System.Runtime;
    using System.Runtime.InteropServices;

    using HWND = System.IntPtr;

    /// <summary>
    /// Summary description for Win32.
    /// </summary>
    public class Win32
    {
        public const int EM_GETSCROLLPOS = WM_USER + 221;
        public const int EM_SETSCROLLPOS = WM_USER + 222;
        public const short KS_KEYDOWN = 0x80;
        public const short KS_ON = 0x01;
        public const int VK_CONTROL = 0x11;
        public const int VK_DOWN = 0x28;
        public const int VK_NUMLOCK = 0x90;
        public const int VK_UP = 0x26;
        public const int WM_CHAR = 0x102;
        public const int WM_KEYDOWN = 0x100;
        public const int WM_KEYUP = 0x101;
        public const int WM_PAINT = 0xF;
        public const int WM_USER = 0x400;

        private Win32()
        {
        }

        [DllImport("user32")]
        public static extern short GetKeyState(int nVirtKey);

        [DllImport("user32")]
        public static extern int LockWindowUpdate(HWND hwnd);

        [DllImport("user32")]
        public static extern int PostMessage(HWND hwnd, int wMsg, int wParam, int lParam);

        [DllImport("user32")]
        public static extern int SendMessage(HWND hwnd, int wMsg, int wParam, IntPtr lParam);

        [StructLayout(LayoutKind.Sequential)]
        public struct POINT
        {
            public int x;
            public int y;
        }
    }
}