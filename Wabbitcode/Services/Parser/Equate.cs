namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    internal class Equate : ILabel
    {
        public Equate(DocLocation loc, string labelName, string value, string description, ParserInformation parent)
        {
            this.LabelName = labelName;
            this.Location = loc;
            this.Description = description;
            this.Parent = parent;
            this.Value = value;
        }

        public string Description
        {
            get;
            set;
        }

        public bool IsReusable
        {
            get
            {
                return this.LabelName == "_";
            }
        }

        public string LabelName
        {
            get;
            set;
        }

        public DocLocation Location
        {
            get;
            set;
        }

        public string Name
        {
            get
            {
                return this.LabelName;
            }
        }

        public ParserInformation Parent
        {
            get;
            set;
        }

        public string Value
        {
            get;
            set;
        }

        public static bool operator !=(Equate label1, Equate label2)
        {
            if ((object)label1 == null || (object)label2 == null)
            {
                if ((object)label1 != null && (object)label2 != null)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            return label1.Location.Offset != label2.Location.Offset || label1.LabelName != label2.LabelName;
        }

        public static bool operator ==(Equate label1, Equate label2)
        {
            if ((object)label1 == null || (object)label2 == null)
            {
                if ((object)label1 == null && (object)label2 == null)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            return label1.Location.Offset == label2.Location.Offset && label1.LabelName == label2.LabelName;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Equate))
            {
                return base.Equals(obj);
            }
            Equate label2 = obj as Equate;
            return this.Location.Offset == label2.Location.Offset && this.LabelName == label2.LabelName;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public override string ToString()
        {
            return this.LabelName;
        }
    }
}