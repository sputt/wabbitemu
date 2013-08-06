using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.Editor
{
	public class EditorSelectionEventArgs : EditorEventArgs
	{
		public IDocument Document { get; private set; }
		public string SelectedLines { get; private set; }

		public EditorSelectionEventArgs(IDocument document, string selectedLines)
			: base(document)
		{
			Document = document;
			SelectedLines = selectedLines;
		}
	}
}