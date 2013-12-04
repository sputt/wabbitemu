using System.Drawing;
using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    public class DebugHighlightMarker : TextMarker
    {
        public DebugHighlightMarker(int offset, int length)
            : base(offset, length, TextMarkerType.SolidBlock, Color.Yellow)
        {
        }
    }
}