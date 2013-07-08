namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    using Revsoft.Wabbitcode.Services.Parser;

    internal class Define : IDefine
    {
        public Define(DocLocation loc, string macroName, string contents, string description, ParserInformation parent, int value)
        {
            this.Location = loc;
            this.Name = macroName;
            this.Contents = contents;
            this.Description = description;
            this.Parent = parent;
            this.Value = value;
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
                return this.Value < 256;
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