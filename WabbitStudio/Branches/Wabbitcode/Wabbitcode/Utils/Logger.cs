using System;
using System.Diagnostics;

namespace Revsoft.Wabbitcode.Utils
{
    public static class Logger
    {
        public static void Log(string message)
        {
            Debug.WriteLine(message);
        }

        public static void Log(string message, Exception ex)
        {
            Debug.WriteLine("{0} {1}", message, ex);
        }
    }
}