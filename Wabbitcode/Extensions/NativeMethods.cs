using System.Runtime.InteropServices;

namespace Revsoft.Wabbitcode.Extensions
{
    internal static class NativeMethods
    {
        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        internal static extern bool ImageList_DragShowNolock(bool fShow);
    }
}