namespace Revsoft.Wabbitcode.Exceptions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

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