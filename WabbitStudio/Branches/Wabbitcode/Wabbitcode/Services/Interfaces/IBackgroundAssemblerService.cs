using System;
using Revsoft.Wabbitcode.Services.Assembler;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IBackgroundAssemblerService : IService
	{
        event EventHandler<AssemblyFinishEventArgs> OnBackgroundAssemblerComplete;

		void RequestAssemble();
	    void CountCode(string codeInfoLines);
	}
}