namespace Revsoft.Wabbitcode.EditorExtensions
{
    public class FoldingItem
    {
        public int Offset { get; private set; }
        public string Text { get; private set; }

        public FoldingItem(int offset, string text)
        {
            Offset = offset;
            Text = text;
        }
    }
}