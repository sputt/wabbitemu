using Revsoft.TextEditor.Document;
using Revsoft.TextEditor.Util;

namespace Revsoft.TextEditor.Src.Document.TextBufferStrategy
{
    class RopeTextBufferStrategy : ITextBufferStrategy
    {
        private Rope<char> _rope = new Rope<char>();

        public int Length
        {
            get { return _rope.Length; }
        }

        public void Insert(int offset, string text)
        {
            _rope.InsertRange(offset, text);
        }

        public void Remove(int offset, int length)
        {
            _rope.RemoveRange(offset, length);
        }

        public void Replace(int offset, int length, string text)
        {
            Remove(offset, length);
            Insert(offset, text);
        }

        public string GetText(int offset, int length)
        {
            return _rope.GetRange(offset, length).ToString();
        }

        public char GetCharAt(int offset)
        {
            return _rope[offset];
        }

        public void SetContent(string text)
        {
            _rope = new Rope<char>(text);
        }
    }
}
