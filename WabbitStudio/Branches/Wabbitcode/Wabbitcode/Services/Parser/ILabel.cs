using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public interface ILabel : IParserData
	{
		string LabelName { get; set; }
		bool IsReusable { get; }
	}
}
