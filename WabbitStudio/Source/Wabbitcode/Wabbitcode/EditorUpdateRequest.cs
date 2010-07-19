using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode
{
    public class UpdateArgs : EventArgs
    {
        public enum UpdateEnum
        {
            UpdateCodeInfo = 0,
            UpdateLabels = 1,
            UpdateErrors = 2
        }
        public readonly UpdateEnum updateEnum;
        public UpdateArgs(UpdateEnum update)
        {
            updateEnum = update;
        }
    }
    public delegate void EditorUpdateEvent(object sender, UpdateArgs e);
	public class EditorUpdateRequest
	{
		public bool UpdateCodeInfo
		{
			get;
			set;
		}

		public bool UpdateLabels
		{
			get;
			set;
		}

		public bool UpdateErrors
		{
			get;
			set;
		}

		public bool RequestQuit
		{
			get;
			set;
		}

		public EditorUpdateRequest()
		{
			RequestQuit = false;
		}

		internal void UpdateAll()
		{
			UpdateCodeInfo = true;
			UpdateLabels = true;
			UpdateErrors = true;
		}

        public event EditorUpdateEvent UpdateRequested;
	}


}
