using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public interface IDefine : IParserData
	{
		string Contents { get; set; }
        bool IsWord { get; }
        int Value { get; set; }
	}
}
