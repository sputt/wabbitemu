using System;
using Revsoft.Wabbitcode.Services.Debugger;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IDebuggerService : IService
    {
        event EventHandler<DebuggingEventArgs> OnDebuggingStarted;
        event EventHandler<DebuggingEventArgs> OnDebuggingEnded;

        IWabbitcodeDebugger CurrentDebugger { get; }

        void StartDebugging();
        void EndDebugging();
    }
}