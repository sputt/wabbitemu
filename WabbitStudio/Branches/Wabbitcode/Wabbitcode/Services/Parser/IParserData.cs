namespace Revsoft.Wabbitcode.Services.Parser
{

    public interface IParserData
    {
        string Description
        {
            get;
            set;
        }

        DocLocation Location
        {
            get;
            set;
        }

        string Name
        {
            get;
        }

        ParserInformation Parent
        {
            get;
            set;
        }
    }

    public class DocLocation
    {
        public DocLocation(int line, int offset)
        {
            this.Line = line;
            this.Offset = offset;
        }

        public int Line
        {
            get;
            set;
        }

        public int Offset
        {
            get;
            set;
        }
    }
}