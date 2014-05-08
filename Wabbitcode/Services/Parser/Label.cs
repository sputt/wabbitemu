namespace Revsoft.Wabbitcode.Services.Parser
{

    internal class Label : ILabel
    {
        public Label(DocLocation loc, string labelName, string description, ParserInformation parent)
        {
            LabelName = labelName;
            Location = loc;
            Description = description;
            Parent = parent;
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
                return LabelName == "_";
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
                return LabelName;
            }
        }

        public ParserInformation Parent
        {
            get;
            set;
        }

        public static bool operator !=(Label label1, Label label2)
        {
            if ((object) label1 != null && (object) label2 != null)
            {
                return label1.Location.Offset != label2.Location.Offset || label1.LabelName != label2.LabelName;
            }

            return (object)label1 != null || (object)label2 != null;
        }

        public static bool operator ==(Label label1, Label label2)
        {
            if ((object) label1 != null && (object) label2 != null)
            {
                return label1.Location.Line == label2.Location.Line && label1.LabelName == label2.LabelName;
            }

            return (object)label1 == null && (object)label2 == null;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Label))
            {
                return false;
            }
            Label label2 = obj as Label;
            return Location.Line == label2.Location.Line && LabelName == label2.LabelName;
        }

        public override int GetHashCode()
        {
            return LabelName.GetHashCode() + Location.Line.GetHashCode();
        }

        public override string ToString()
        {
            return LabelName;
        }
    }
}