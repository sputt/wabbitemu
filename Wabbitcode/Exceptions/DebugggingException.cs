using System;

namespace Revsoft.Wabbitcode.Exceptions
{
    [Serializable]
    internal class DebuggingException : Exception
    {
        public DebuggingException(string message)
            : base(message)
        {
        }
    }
}