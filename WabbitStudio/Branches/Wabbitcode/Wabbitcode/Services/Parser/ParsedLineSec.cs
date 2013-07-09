namespace Revsoft.Wabbitcode.Services.Parser
{

    internal class ParsedLineSec
    {
        private readonly string line;

        public ParsedLineSec(string line)
        {
            this.line = line;
        }

        public string Arg1
        {
            get;
            set;
        }

        public string Arg2
        {
            get;
            set;
        }

        public string Command
        {
            get;
            set;
        }

        public bool Error
        {
            get;
            set;
        }

        public string Label
        {
            get;
            set;
        }

        public string Line
        {
            get
            {
                return this.line;
            }
        }
    }
}