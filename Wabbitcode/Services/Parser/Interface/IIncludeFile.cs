using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public interface IIncludeFile : IParserData
	{
		string IncludedFile { get; set; }
	}
}
