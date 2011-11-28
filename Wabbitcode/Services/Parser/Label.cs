using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class Label : ILabel
	{
		public Label(int offset, string labelName, bool isEquate, string description, ParserInformation parent)
		{
			LabelName = labelName;
			Offset = offset;
			IsEquate = isEquate;
			Description = description;
			Parent = parent;
		}

		public string Name
		{
			get { return LabelName; }
		}

		public string LabelName
		{
			get;
			set;
		}

		public bool IsReusable
		{
			get
			{
				return LabelName == "_";
			}
		}

		public bool IsEquate
		{
			get;
			set;
		}

		public int Offset
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
			return LabelName;
		}

        #region Overrides

        public override int GetHashCode()
        {
            return Offset.GetHashCode() * LabelName.GetHashCode();
        }

		public override bool Equals(object obj)
		{
            if (!(obj is Label))
            {
                return false;
            }
			Label label2 = (Label)obj;
			return Offset == label2.Offset && LabelName == label2.LabelName;
		}

		public static bool operator ==(Label label1, Label label2)
		{
            if (label1 == null || label2 == null)
            {
                if (label1 == null && label2 == null)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
			return label1.Offset == label2.Offset && label1.LabelName == label2.LabelName;
		}

		public static bool operator !=(Label label1, Label label2)
		{
            if (label1 == null || label2 == null)
            {
                if (label1 != null && label2 != null)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
			return label1.Offset != label2.Offset || label1.LabelName != label2.LabelName;
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
