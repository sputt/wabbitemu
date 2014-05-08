using System;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Parser
{
    public class ParserEventArgs : EventArgs
    {
        public FilePath FileName { get; private set; }

        public ParserEventArgs(FilePath filename)
        {
            FileName = filename;
        }
    }
}