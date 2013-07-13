using System;

namespace Revsoft.Wabbitcode.Exceptions
{
    [Serializable]
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
