using System.Drawing;
using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.EditorExtensions.Markers
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
