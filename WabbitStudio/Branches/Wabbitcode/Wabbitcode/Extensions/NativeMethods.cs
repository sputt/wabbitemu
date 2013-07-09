namespace Revsoft.Wabbitcode.Classes
{
    using System;
    using System.Drawing;
    using System.Runtime.InteropServices;

    internal static partial class NativeMethods
    {
        internal const int VK_LSHIFT = 0xA0;
        internal const int VK_RSHIFT = 0xA1;
        internal const int VK_SHIFT = 16;
        internal const int WM_KEYDOWN = 0x0100;
        internal const int WM_KEYUP = 0x0101;

        private const int EM_GETEVENTMASK = WM_USER + 59;
        private const int EM_GETSCROLLPOS = WM_USER + 221;
        private const int EM_SETEVENTMASK = WM_USER + 69;
        private const int EM_SETSCROLLPOS = WM_USER + 222;
        private const int WM_SETREDRAW = 0x000B;
        private const int WM_USER = 0x400;

        [DllImport("SPASM.dll", BestFitMapping = true, ThrowOnUnmappableChar = true)]
        internal static extern int AddDefine(
            [In, MarshalAs(UnmanagedType.LPStr)] string name,
            [In, MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport("SPASM.dll", BestFitMapping = true, ThrowOnUnmappableChar = true)]
        internal static extern int AddInclude([In, MarshalAs(UnmanagedType.LPStr)] string directory);

        [DllImport("SPASM.dll")]
        internal static extern int ClearDefines();

        [DllImport("SPASM.dll")]
        internal static extern int ClearIncludes();

        [DllImport("gdi32.dll")]
        internal static extern bool DeleteObject(IntPtr hObject);

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        internal static extern IntPtr FindWindow(string strClassName, string nptWindowName);

        [DllImport("user32.dll")]
        internal static extern short GetKeyState(int key);

        internal static Point GetScrollPos(IntPtr handle)
        {
            Point scrollPos = new Point(0, 0);
            SendMessage(handle, EM_GETSCROLLPOS, (IntPtr)0, ref scrollPos);
            return scrollPos;
        }

        [DllImport("kernel32.dll")]
        internal static extern IntPtr GetStdHandle(uint nStdHandle);

        [DllImport("SPASM.dll")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string GetStdOut();

        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern bool ImageList_BeginDrag(
            IntPtr himlTrack,
            int
            iTrack,
            int dxHotspot,
            int dyHotspot);

        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern bool ImageList_DragEnter(IntPtr hwndLock, int x, int y);

        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern bool ImageList_DragLeave(IntPtr hwndLock);

        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern bool ImageList_DragMove(int x, int y);

        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern bool ImageList_DragShowNolock(bool fShow);

        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern void ImageList_EndDrag();

        [DllImport("comctl32.dll")]
        internal static extern bool InitCommonControls();

        [DllImport("SPASM.dll")]
        internal static extern int RunAssembly();

        internal static void SaveScrollPos(IntPtr handle, Point scrollPos)
        {
            SendMessage(handle, EM_SETSCROLLPOS, (IntPtr)0, ref scrollPos);
        }

        [DllImport("user32.dll")]
        internal static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("SPASM.dll", BestFitMapping = true, ThrowOnUnmappableChar = true)]
        internal static extern int SetInputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        internal static extern bool SetMenuInfo(HandleRef hMenu, MENUINFO lpcmi);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        internal static extern bool SetMenuItemInfo(HandleRef hMenu, int uItem, bool fByPosition, MENUITEMINFO_T_RW lpmii);

        [DllImport("SPASM.dll")]
        internal static extern int SetMode(int mode);

        [DllImport("SPASM.dll", BestFitMapping = true, ThrowOnUnmappableChar = true)]
        internal static extern int SetOutputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        // SetLastError = true
        [DllImport("Kernel32.dll")]
        internal static extern bool SetStdHandle(uint device, IntPtr handle);

        [DllImport("SPASM.dll")]
        internal static extern int ShowMessage();

        // [DllImport("Kernel32.dll")]// SetLastError = true
        // internal static extern uint GetStdHandle(uint device);
        internal static void TurnOffDrawing(IntPtr handle)
        {
            Point temp = new Point(0, 0);
            SendMessage(handle, WM_SETREDRAW, (IntPtr)0, ref temp);
            SendMessage(handle, EM_GETEVENTMASK, (IntPtr)0, ref temp);
        }

        internal static void TurnOnDrawing(IntPtr handle)
        {
            Point temp = new Point(0, 0);
            SendMessage(handle, EM_SETEVENTMASK, (IntPtr)0, ref temp);
            SendMessage(handle, WM_SETREDRAW, (IntPtr)1, ref temp);
        }

        [DllImport("libWabbitemu.dll")]
        private static extern byte ReadMem(int slot, ushort address);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(IntPtr hWnd, int msg, IntPtr wParam, ref Point lParam);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        internal class MENUINFO
        {
            internal int cbSize = Marshal.SizeOf(typeof(MENUINFO));
            internal int fMask = 0x00000010; // MIM_STYLE;
            internal int dwStyle = 0x04000000; // MNS_CHECKORBMP;
            internal uint cyMax;
            internal IntPtr hbrBack = IntPtr.Zero;
            internal int dwContextHelpID;
            internal IntPtr dwMenuData = IntPtr.Zero;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
        internal class MENUITEMINFO_T_RW
        {
            internal int cbSize = Marshal.SizeOf(typeof(MENUITEMINFO_T_RW));
            internal int fMask = 0x00000080; // MIIM_BITMAP = 0x00000080
            internal int fType;
            internal int fState;
            internal int wID;
            internal IntPtr hSubMenu = IntPtr.Zero;
            private IntPtr hbmpChecked = IntPtr.Zero;
            internal IntPtr hbmpUnchecked = IntPtr.Zero;
            internal IntPtr dwItemData = IntPtr.Zero;
            private IntPtr dwTypeData = IntPtr.Zero;
            internal int cch;
            internal IntPtr hbmpItem = IntPtr.Zero;
        }
    }
}