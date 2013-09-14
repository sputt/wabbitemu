namespace Revsoft.Wabbitcode.Classes
{
    using System;
    using System.Drawing;
    using System.Runtime.InteropServices;

    internal static class NativeMethods
    {
        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern bool ImageList_DragShowNolock(bool fShow);
    }
}