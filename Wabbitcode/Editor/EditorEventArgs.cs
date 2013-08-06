using System;
using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.Editor
{
	public class EditorEventArgs : EventArgs
	{
		public IDocument Document { get; private set; }

		public EditorEventArgs(IDocument document)
		{
			Document = document;
		}
	}
}