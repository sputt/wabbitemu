using System;

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
