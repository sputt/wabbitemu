using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	public class EditorSelectionEventArgs : EditorEventArgs
	{
		public string SelectedLines { get; private set; }
	    public TextEditor.Caret Caret { get; private set; }

	    public EditorSelectionEventArgs(Editor editor, TextEditor.Caret caret, string selectedLines)
			: base(editor)
		{
			SelectedLines = selectedLines;
	        Caret = caret;
		}
	}
}