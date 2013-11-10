using System;

namespace Revsoft.Wabbitcode.Services
{
    public class ParserEventArgs : EventArgs
    {
        public string FileName { get; private set; }

        public ParserEventArgs(string filename)
        {
            FileName = filename;
        }
    }
}