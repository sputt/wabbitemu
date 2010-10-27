using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class Define : IDefine
	{
		public Define(int counter, string macroName, string contents, string description, ParserInformation parent, int value)
		{
			Offset = counter;
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
