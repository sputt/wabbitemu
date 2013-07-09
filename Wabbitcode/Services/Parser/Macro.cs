namespace Revsoft.Wabbitcode.Services.Parser
{
    using System.Collections.Generic;

    internal class Macro : IMacro
    {
        public Macro(DocLocation loc, string macroName, List<string> args, string contents, string description, ParserInformation parent)
        {
            this.Location = loc;
            this.Name = macroName;
            this.Contents = contents;
            this.Description = description;
            this.Arguments = args;
            this.Parent = parent;
        }

        public IList<string> Arguments
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

        public bool IsWord
        {
            get
            {
                return true;    // assume that it returns a 16 bit val
            }
        }

        public DocLocation Location
        {
            get;
            set;
        }

        public string Name
        {
            get;
            set;
        }

        public ParserInformation Parent
        {
            get;
            set;
        }

        public int Value
        {
            get;
            set;
        }

        public override string ToString()
        {
            return this.Name;
        }
    }
}