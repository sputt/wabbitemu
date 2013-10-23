using System;
using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	public class EditorEventArgs : EventArgs
	{
        public string FileName { get; private set; }
		public IDocument Document { get; private set; }

		public EditorEventArgs(IDocument document, string fileName)
		{
		    FileName = fileName;
			Document = document;
		}
	}
}