namespace Revsoft.Wabbitcode.Services.Parser
{
    public interface IDefine : IParserData
    {
        string Contents { get; set; }

        bool IsWord { get; }

        int Value { get; set; }
    }
}