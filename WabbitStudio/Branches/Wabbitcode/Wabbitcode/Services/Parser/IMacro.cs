namespace Revsoft.Wabbitcode.Services.Parser
{
    using System.Collections.Generic;

    public interface IMacro : IDefine
    {
        IList<string> Arguments { get; }
    }
}