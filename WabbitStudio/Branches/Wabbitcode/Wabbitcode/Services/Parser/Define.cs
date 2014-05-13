namespace Revsoft.Wabbitcode.Services.Parser
{
    internal class Define : IDefine
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

        public string Contents { get; set; }

        public string Description { get; set; }

        public bool IsWord
        {
            get { return Value < 256; }
        }

        public DocLocation Location { get; set; }

        public string Name { get; set; }

        public ParserInformation Parent { get; set; }

        public int Value { get; set; }

        public override string ToString()
        {
            return Name;
        }
    }
}