﻿namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public interface IIncludeFile : IParserData
    {
        string IncludedFile
        {
            get;
            set;
        }
    }
}