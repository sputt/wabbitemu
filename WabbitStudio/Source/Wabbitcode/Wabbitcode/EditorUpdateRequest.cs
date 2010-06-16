using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode
{
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
	}
}
