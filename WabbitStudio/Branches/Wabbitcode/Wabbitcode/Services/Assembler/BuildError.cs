using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    public class BuildError
    {
        public BuildError(FilePath file, int lineNumber, string description, bool isWarning)
        {
            File = file;
            LineNumber = lineNumber;
            Description = description;
            IsWarning = isWarning;
        }

        public string Description { get; set; }

        public FilePath File { get; private set; }

        public bool IsWarning { get; private set; }

        public int LineNumber { get; private set; }
    }
}