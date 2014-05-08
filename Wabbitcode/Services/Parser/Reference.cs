using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Parser
{
    public class Reference
    {
        public Reference(FilePath file, int line, int col, int offset, string reference, string lineContents)
        {
            File = file;
            Line = line;
            Col = col;
	        Offset = offset;
            ReferenceString = reference;
            LineContents = lineContents;
        }

	    public int Col { get; private set; }

        public FilePath File { get; private set; }

	    public int Line { get; private set; }

        public int Offset { get; private set; }

	    public string LineContents { get; private set; }

	    public string ReferenceString { get; private set; }
    }
}