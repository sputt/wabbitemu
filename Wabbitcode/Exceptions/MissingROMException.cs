namespace Revsoft.Wabbitcode.Exceptions
{
    using System;

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