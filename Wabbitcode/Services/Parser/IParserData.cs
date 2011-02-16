using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public interface IParserData
	{
		string Name { get; }
		int Offset { get; set; }
		string Description { get; set; }
		ParserInformation Parent { get; set; }
	}
}
