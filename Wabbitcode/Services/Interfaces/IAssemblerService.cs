using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IAssemblerService : IService
	{
        event EventHandler<AssemblyFinishFileEventArgs> AssemblerFileFinished;
        event EventHandler<AssemblyFinishProjectEventArgs> AssemblerProjectFinished;

        AssemblerOutput AssembleFile(FilePath inputFile, FilePath outputFile, FilePath originalDir,
            IEnumerable<FilePath> includeDirs, AssemblyFlags flags = AssemblyFlags.Normal);

		void AssembleProject(IProject project);
		CodeCountInfo CountCode(string lines);
		string GetExtension(int outputFile);
	}
}