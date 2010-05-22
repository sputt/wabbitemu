using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class Macro : IMacro
	{
		public Macro(int counter, string macroName, List<string> args, string contents, string description, ParserInformation parent)
		{
			Offset = counter;
			Name = macroName;
			Contents = contents;
			Description = description;
			Arguments = args;
			Parent = parent;
			
		}
		public string Name
		{
			get;
			set;
		}
		public int Offset
		{
			get;
			set;
		}

		public string Contents
		{
			get;
			set;
		}

		public string Description
		{
			get;
			set;
		}

		public ParserInformation Parent
		{
			get;
			set;
		}

		public IList<string> Arguments
		{
			get;
			set;
		}
	}
}
