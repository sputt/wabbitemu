namespace Revsoft.Wabbitcode.Services.Parser
{
    using System.Collections.Generic;

    internal class Macro : IMacro
    {
        public Macro(DocLocation loc, string macroName, IList<string> args, string contents, string description, ParserInformation parent)
        {
            Location = loc;
            Name = macroName;
            Contents = contents;
            Description = description;
            Arguments = args;
            Parent = parent;
        }

        public IList<string> Arguments { get; set; }

        public string Contents { get; set; }

        public string Description { get; set; }

        public bool IsWord
        {
            get { return true; // assume that it returns a 16 bit val
            }
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