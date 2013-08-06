using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Utils
{
	public class DocumentLocation
	{
		public string FileName { get; private set; }
		public int LineNumber { get; private set; }

		public DocumentLocation(string fileName, int lineNumber)
		{
			FileName = fileName;
			LineNumber = lineNumber;
		}
	}
}
