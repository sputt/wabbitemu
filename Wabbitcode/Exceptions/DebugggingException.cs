using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Exceptions
{
    class DebuggingException : Exception
    {
        public DebuggingException()
        : base()
        {
        }

        public DebuggingException(string message)
        : base(message)
        {
        }
    }
}
