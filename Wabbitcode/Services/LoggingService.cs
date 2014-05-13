using System;
using System.Diagnostics;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class LoggingService : ILoggingService
    {
        public void Log(string message)
        {
            Debug.WriteLine(message);
        }

        public void Log(string message, Exception ex)
        {
            Debug.WriteLine("{0} {1}", message, ex);
        }
    }
}