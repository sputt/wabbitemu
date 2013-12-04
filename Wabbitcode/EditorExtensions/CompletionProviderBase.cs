using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Gui.CompletionWindow;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    internal abstract class CompletionProviderBase : ICompletionDataProvider
    {
        protected readonly WabbitcodeTextEditor TextEditor;

        protected CompletionProviderBase(WabbitcodeTextEditor textEditor)
        {
            TextEditor = textEditor;
        }

        public ImageList ImageList
        {
            get
            {
                return new ImageList();
            }
        }

        public string PreSelection
        {
            get
            {
                Caret caret = TextEditor.ActiveTextAreaControl.Caret;
                var segment = TextEditor.Document.GetLineSegment(caret.Line);
                var word = segment.GetWord(caret.Column - 1);
                return word == null ? string.Empty : word.Word;
            }
        }

        public int DefaultIndex
        {
            get
            {
                return -1;
            }
        }

        public CompletionDataProviderKeyResult ProcessKey(char key)
        {
            CompletionDataProviderKeyResult res;
            if (char.IsLetterOrDigit(key) || key == '_' || key == '.' || key == '#')
            {
                res = CompletionDataProviderKeyResult.NormalKey;
            }
            else
            {
                // do not reset insertSpace when doing an insertion!
                res = CompletionDataProviderKeyResult.InsertionKey;
            }

            return res;
        }

        public bool InsertAction(ICompletionData data, TextArea textArea, int insertionOffset, char key)
        {
            textArea.Caret.Position = textArea.Document.OffsetToPosition(insertionOffset);

            return data.InsertAction(textArea, key);
        }

        public abstract ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped);
    }
}