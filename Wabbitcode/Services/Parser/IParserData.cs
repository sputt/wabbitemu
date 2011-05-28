using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public interface IParserData
	{
		string Name { get; }
		DocLocation Location { get; set; }
		string Description { get; set; }
		ParserInformation Parent { get; set; }
	}

	public class DocLocation
	{
		public int Offset { get; set; }
		public int Line { get; set; }
		public DocLocation(int line, int offset)
		{
			Line = line;
			Offset = offset;
		}
	}
}
