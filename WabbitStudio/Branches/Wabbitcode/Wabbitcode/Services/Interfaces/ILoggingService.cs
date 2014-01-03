using System;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface ILoggingService
    {
        void Log(string message);
        void Log(string message, Exception ex);
    }
}