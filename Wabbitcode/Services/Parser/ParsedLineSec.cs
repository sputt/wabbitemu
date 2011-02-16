using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
    internal class ParsedLineSec
    {
        readonly string line;
        public string Line
        {
            get { return line; }
        }

        public string Command { get; set; }

        public string Arg1 { get; set; }

        public string Arg2 { get; set; }

        public bool Error { get; set; }

        public string Label { get; set; }

        public ParsedLineSec(string line)
        {
            this.line = line;
        }
    }
}
