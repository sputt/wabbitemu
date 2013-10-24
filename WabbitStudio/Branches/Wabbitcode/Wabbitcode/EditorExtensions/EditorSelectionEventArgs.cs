using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	public class EditorSelectionEventArgs : EditorEventArgs
	{
		public string SelectedLines { get; private set; }

		public EditorSelectionEventArgs(Editor editor, string selectedLines)
			: base(editor)
		{
			SelectedLines = selectedLines;
		}
	}
}