using System;

namespace Revsoft.Wabbitcode.Exceptions
{
    [Serializable]
    public class MissingRomException : Exception
    {
        public MissingRomException(string message)
            : base(message)
        {
        }
    }
}