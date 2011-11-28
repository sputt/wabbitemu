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

        public bool IsWord
        {
            get { return true; }                    //assume that it returns a 16 bit val
        }

        public int? Value
        {
            get;
            set;
        }

        #region Overrides

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
