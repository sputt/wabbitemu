using System;
using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode
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