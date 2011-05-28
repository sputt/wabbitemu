using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class Define : IDefine
	{
		public Define(DocLocation loc, string macroName, string contents, string description, ParserInformation parent, int value)
		{
			Location = loc;
			Name = macroName;
			Contents = contents;
			Description = description;
			Parent = parent;
            Value = value;
		}

		public string Name
		{
			get;
			set;
		}

		public DocLocation Location
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

        public int Value
        {
            get;
            set;
        }

        public bool IsWord
        {
            get
            {
                return Value < 256;
            }
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
