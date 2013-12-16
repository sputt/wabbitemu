namespace Revsoft.Wabbitcode.Services.Assembler
{
    public class BuildError
    {
        public BuildError(string file, int lineNumber, string description, bool isWarning)
        {
            File = file;
            LineNumber = lineNumber;
            Description = description;
            IsWarning = isWarning;
        }

        public string Description
        {
            get;
            set;
        }

        public string File
        {
            get;
            private set;
        }

        public bool IsWarning
        {
            get;
            private set;
        }

        public int LineNumber
        {
            get;
            private set;
        }
    }
}