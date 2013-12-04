using System.Drawing;

namespace Revsoft.TextEditor.Document
{
    public class BreakpointTextMarker : TextMarker
    {
        public BreakpointTextMarker(int offset, int length) 
            : base(offset, length, TextMarkerType.SolidBlock, Color.Maroon, Color.White)
        {
        }
    }
}