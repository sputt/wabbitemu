namespace Revsoft.Wabbitcode.Exceptions
{
    using System;

    public class MissingConfigException : Exception
    {
        public MissingConfigException()
        : base()
        {
        }

        public MissingConfigException(string message)
        : base(message)
        {
        }
    }
}