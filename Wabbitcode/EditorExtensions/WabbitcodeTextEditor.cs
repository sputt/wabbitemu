using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    internal sealed class WabbitcodeTextEditor : TextEditorControl
    {
        public WabbitcodeTextEditor()
        {
            AllowDrop = true;
            Dock = DockStyle.Fill;
            Font = Settings.Default.EditorFont;
            IsIconBarVisible = Settings.Default.IconBar;
            IsReadOnly = false;
            LineViewerStyle = Settings.Default.LineEnabled ?
                LineViewerStyle.FullRow : LineViewerStyle.None;
            ShowLineNumbers = Settings.Default.LineNumbers;
            ShowVRuler = false;
            TextEditorProperties.MouseWheelScrollDown = !Settings.Default.InverseScrolling;
            TextRenderingHint = Settings.Default.AntiAlias ?
                TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
        }
    }
}
