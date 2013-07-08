namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

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