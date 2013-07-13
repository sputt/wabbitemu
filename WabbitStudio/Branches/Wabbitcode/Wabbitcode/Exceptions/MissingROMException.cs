using System;

namespace Revsoft.Wabbitcode.Exceptions
{
    [Serializable]
    public class MissingROMException : Exception
    {
        public MissingROMException()
        : base()
        {
        }

        public MissingROMException(string message)
        : base(message)
        {
        }
    }
}