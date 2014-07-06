namespace Revsoft.Wabbitcode.EditorExtensions
{
    public class EditorToolTipRequestEventArgs : EditorEventArgs
    {
        public string Tooltip { get; set; }

        public string WordHovered { get; private set; }

        public EditorToolTipRequestEventArgs(Editor editor, string wordHovered) : base(editor)
        {
            WordHovered = wordHovered;
        }
    }
}