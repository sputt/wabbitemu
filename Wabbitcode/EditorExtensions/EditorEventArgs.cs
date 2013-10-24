using System;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	public class EditorEventArgs : EventArgs
	{
		public Editor Editor { get; private set; }

		public EditorEventArgs(Editor editor)
		{
			Editor = editor;
		}
	}
}