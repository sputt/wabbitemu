namespace Revsoft.Wabbitcode.Exceptions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

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