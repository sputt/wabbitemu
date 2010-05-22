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

		public EditorUpdateRequest()
		{
			UpdateAll();	
		}

		internal void UpdateAll()
		{
			UpdateCodeInfo = true;
			UpdateLabels = true;
			UpdateErrors = true;
		}
	}
}
