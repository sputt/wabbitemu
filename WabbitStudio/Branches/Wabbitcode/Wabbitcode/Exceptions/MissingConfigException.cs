using System;

namespace Revsoft.Wabbitcode.Exceptions
{
    [Serializable]
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