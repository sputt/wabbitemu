using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode
{
	public class ListBoxItem
	{
		public string Text { get; set; }
		public string File { get; set; }
		public string ext { get; set; }

		public override string ToString()
		{
			return Text;
		}

	}
}
