using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public interface IMacro : IDefine
	{
		IList<string> Arguments { get; set; }
	}
}
