using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class Define : IDefine
	{
		public Define(int counter, string macroName, string contents, string description, ParserInformation parent)
		{
			Offset = counter;
			Name = macroName;
			Contents = contents;
			Description = description;
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

        public override string ToString()
        {
            return Name;
        }
	}
}
