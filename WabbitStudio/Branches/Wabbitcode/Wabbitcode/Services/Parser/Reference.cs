namespace Revsoft.Wabbitcode.Services.Parser
{
    public class Reference
    {
	    public Reference(string file, int line, int col, string reference, string lineContents)
        {
            File = file;
            Line = line;
            Col = col;
            ReferenceString = reference;
            LineContents = lineContents;
        }

	    public int Col { get; private set; }

	    public string File { get; private set; }

	    public int Line { get; private set; }

	    public string LineContents { get; private set; }

	    public string ReferenceString { get; private set; }
    }
}