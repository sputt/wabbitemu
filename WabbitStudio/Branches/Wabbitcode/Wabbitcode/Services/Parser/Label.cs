﻿namespace Revsoft.Wabbitcode.Services.Parser
{

    internal class Label : ILabel
    {
        public Label(DocLocation loc, string labelName, string description, ParserInformation parent)
        {
            this.LabelName = labelName;
            this.Location = loc;
            this.Description = description;
            this.Parent = parent;
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

        public static bool operator !=(Label label1, Label label2)
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

        public static bool operator ==(Label label1, Label label2)
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
            if (!(obj is Label))
            {
                return false;
            }
            Label label2 = obj as Label;
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