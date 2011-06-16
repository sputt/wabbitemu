using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services
{
	public class Reference
	{
		private string file;
		public string File
		{
			get { return file; }
			set { file = value; }
		}

		private int line;
		public int Line
		{
			get { return line; }
			set { line = value; }
		}
		private int col;
		public int Col
		{
			get { return col; }
			set { col = value; }
		}

		private string lineContents;
		public string LineContents
		{
			get { return lineContents; }
			set { lineContents = value; }
		}

		private string reference;
		public string ReferenceString
		{
			get { return reference; }
		}


		public Reference(string file, int line, int col, string reference)
		{
			this.file = file;
			this.line = line;
			this.col = col;
			this.reference = reference;
		}
		public Reference(string file, int line, int col, string reference, string lineContents)
		{
			this.file = file;
			this.line = line;
			this.col = col;
			this.reference = reference;
			this.lineContents = lineContents;
		}
	}
}
