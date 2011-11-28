using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class Define : IDefine
	{
		public Define(int counter, string macroName, string contents, string description, ParserInformation parent, int? value = null)
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

        public int? Value
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

        #region Override

        public override string ToString()
        {
            return Name;
        }

        #endregion

        #region IComparable

        public int CompareTo(IParserData other)
        {
            if (other == null)
            {
                return 1;
            }
            if (Offset == other.Offset)
            {
                return 0;
            }
            if (Offset > other.Offset)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }

        #endregion
    }
}
