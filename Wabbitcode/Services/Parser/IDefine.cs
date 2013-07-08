namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public interface IDefine : IParserData
    {
        string Contents
        {
            get;
            set;
        }

        bool IsWord
        {
            get;
        }

        int Value
        {
            get;
            set;
        }
    }
}