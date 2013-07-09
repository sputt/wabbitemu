namespace Revsoft.Wabbitcode.Services.Parser
{

    public interface ILabel : IParserData
    {
        bool IsReusable
        {
            get;
        }

        string LabelName
        {
            get;
            set;
        }
    }
}