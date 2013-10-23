using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	public class EditorSelectionEventArgs : EditorEventArgs
	{
		public string SelectedLines { get; private set; }

		public EditorSelectionEventArgs(IDocument document, string fileName, string selectedLines)
			: base(document, fileName)
		{
			SelectedLines = selectedLines;
		}
	}
}