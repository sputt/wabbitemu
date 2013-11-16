using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IAssemblerService : IService
	{
        event EventHandler<AssemblyFinishFileEventArgs> AssemblerFileFinished;
        event EventHandler<AssemblyFinishProjectEventArgs> AssemblerProjectFinished;

		AssemblerOutput AssembleFile(string inputFile, string outputFile, string originalDir, IEnumerable<string> includeDirs,
			AssemblyFlags flags = AssemblyFlags.Normal);

		void AssembleProject(IProject project);
		CodeCountInfo CountCode(string lines);
		string GetExtension(int outputFile);
	}
}