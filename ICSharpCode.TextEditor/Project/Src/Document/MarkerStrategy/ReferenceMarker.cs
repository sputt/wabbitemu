using System.Drawing;

namespace Revsoft.TextEditor.Document
{
    public class ReferenceMarker : TextMarker
    {
        public ReferenceMarker(int offset, int length)
            : base(offset, length, TextMarkerType.SolidBlock, Color.LightGray)
        {
            Tag = "Reference";
        }
    }
}