using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Symbols
{
    public class CallerInformation
    {
        public string CallName { get; private set; }
        public string Condition { get; private set; }
        public string Command { get; private set; }
        public DocumentLocation DocumentLocation { get; private set; }

        public CallerInformation(string callName, string condition, string command, DocumentLocation location)
        {
            CallName = callName;
            Condition = condition;
            Command = command;
            DocumentLocation = location;
        }
    }
}
