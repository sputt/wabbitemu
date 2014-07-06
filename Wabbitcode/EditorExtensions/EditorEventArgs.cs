using System;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	public class EditorEventArgs : EventArgs
	{
		public AbstractFileEditor Editor { get; private set; }

        public EditorEventArgs(AbstractFileEditor editor)
		{
			Editor = editor;
		}
	}
}