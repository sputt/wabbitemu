using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class Equate : ILabel
	{
		public Equate(DocLocation loc, string labelName, string value, string description, ParserInformation parent)
		{
			LabelName = labelName;
			Location = loc;
			Description = description;
			Parent = parent;
            Value = value;
		}

        public string Value
        {
            get;
            set;
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

		public DocLocation Location
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

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

		public override bool Equals(object obj)
		{
			if (obj.GetType() != typeof(Equate))
				return base.Equals(obj);
			Equate label2 = obj as Equate;
			return Location.Offset == label2.Location.Offset && LabelName == label2.LabelName;
		}

		public static bool operator ==(Equate label1, Equate label2)
		{
			if ((object)label1 == null || (object)label2 == null)
				if ((object)label1 == null && (object)label2 == null)
					return true;
				else 
					return false;
			return label1.Location.Offset == label2.Location.Offset && label1.LabelName == label2.LabelName;
		}

		public static bool operator !=(Equate label1, Equate label2)
		{
			if ((object)label1 == null || (object)label2 == null)
				if ((object)label1 != null && (object)label2 != null)
					return false;
				else 
					return true;
			return label1.Location.Offset != label2.Location.Offset || label1.LabelName != label2.LabelName;
		}
    }
}
