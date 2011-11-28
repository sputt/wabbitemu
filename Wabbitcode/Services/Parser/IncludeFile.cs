using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class IncludeFile : IIncludeFile
	{
		public IncludeFile(int counter, string file, string description, ParserInformation parent)
		{
			Offset = counter;
			IncludedFile = file;
			Description = description;
			Parent = parent;
		}

		public string Name
		{
			get { return IncludedFile; }
		}

		public int Offset
		{
			get;
			set;
		}

		public string IncludedFile
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

        #region Overrides

        public override bool Equals(object obj)
        {
            if (!(obj is IIncludeFile))
            {
                return base.Equals(obj);
            }
            IIncludeFile newInclude = (IIncludeFile)obj;
            return newInclude.Parent.Equals(Parent) && newInclude.Name == Name;
        }

        public override int GetHashCode()
        {
            return Parent.GetHashCode() * Name.GetHashCode() + Offset.GetHashCode();
        }

        public override string ToString()
        {
            return Path.GetFileName(Name);
        }

        #endregion

        #region IComparable

        public int CompareTo(IParserData other)
        {
            if (Parent != null && !Parent.Equals(other.Parent))
            {
                return 1;
            }
            if (other is IIncludeFile)
            {
                return Name.CompareTo(other.Name);
            }
            else if (other.Offset < Offset)
            {
                return 1;
            }
            else if (other.Offset == Offset)
            {
                return 0;
            }
            else
            {
                return -1;
            }
        }

        #endregion
    }
}
