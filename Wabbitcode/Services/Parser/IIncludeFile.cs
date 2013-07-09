namespace Revsoft.Wabbitcode.Services.Parser
{

    public interface IIncludeFile : IParserData
    {
        string IncludedFile
        {
            get;
            set;
        }
    }
}