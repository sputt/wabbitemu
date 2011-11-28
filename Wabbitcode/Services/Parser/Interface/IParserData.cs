using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public interface IParserData : IComparable<IParserData>
	{
		string Name { get; }
		int Offset { get; set; }
		string Description { get; set; }
		ParserInformation Parent { get; set; }
	}
}
