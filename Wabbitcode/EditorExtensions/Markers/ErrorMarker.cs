using System.Drawing;
using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.EditorExtensions.Markers
{
    class ErrorMarker : TextMarker
    {
        public ErrorMarker(int offset, int length, string description, bool isWarning)
            : base(offset, length, TextMarkerType.WaveLine, isWarning ? Color.Yellow : Color.Red)
        {
            Tag = "ErrorMarker";
            ToolTip = description;
        }
    }
}
