using System;
using Revsoft.Wabbitcode.Services.Assembler;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IBackgroundAssemblerService
    {
        event EventHandler<AssemblyFinishEventArgs> BackgroundAssemblerComplete;

        void RequestAssemble();
        void CountCode(string codeInfoLines);
    }
}