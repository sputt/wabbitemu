using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency(typeof(ISymbolService))]
    [ServiceDependency(typeof(IFileReaderService))]
	public sealed class AssemblerService : IAssemblerService, IDisposable
	{
		#region Events

        public event EventHandler<AssemblyFinishFileEventArgs> AssemblerFileFinished;
        public event EventHandler<AssemblyFinishProjectEventArgs> AssemblerProjectFinished;

		#endregion 

		private static readonly object AssemblyLock = new object();

		private bool _disposed;
		private IAssembler _assembler;
		private readonly string _outputFormatting = "=================={0}==================" + Environment.NewLine +
										  "Assembling {1}" + Environment.NewLine + "{2}";

		private readonly ISymbolService _symbolService;
	    private readonly IFileReaderService _fileReaderService;

		public AssemblerService(IFileReaderService fileReaderService, ISymbolService symbolService)
		{
		    _fileReaderService = fileReaderService;
			_symbolService = symbolService;
		}

	    public AssemblerOutput AssembleFile(string inputFile, string outputFile, string originalDir, 
            IEnumerable<string> includeDirs, AssemblyFlags flags = AssemblyFlags.Normal)
		{
			_assembler = new SpasmComAssembler();

		    AssemblerHelper.SetupAssembler(_assembler, inputFile, outputFile, originalDir, includeDirs, flags);	
			string rawOutput = _assembler.Assemble();

			// lets write it to the output window so the user knows whats happening
			string outputText = string.Format(_outputFormatting, Path.GetFileName(inputFile),  inputFile, rawOutput);

			bool errors = outputText.Contains("error");
            OnAssemblerFileFinished(this, new AssemblyFinishFileEventArgs(inputFile, outputFile, outputText, !errors));

			// tell if the assembly was successful
			return new AssemblerOutput(outputText, !errors);
		}

		public void AssembleProject(IProject project)
		{
			lock (AssemblyLock)
			{
				project.ProjectWatcher.EnableRaisingEvents = false;

				bool succeeded = project.BuildSystem.Build();

				if (!string.IsNullOrEmpty(project.BuildSystem.ListOutput))
				{
				    string fileText = _fileReaderService.GetFileText(project.BuildSystem.ListOutput);
                    _symbolService.ParseListFile(fileText);
				}

				if (!string.IsNullOrEmpty(project.BuildSystem.LabelOutput))
				{
                    string fileText = _fileReaderService.GetFileText(project.BuildSystem.LabelOutput);
                    _symbolService.ParseSymbolFile(fileText);
				}

				project.ProjectWatcher.EnableRaisingEvents = true;
                OnAssemblerProjectFinished(this, new AssemblyFinishProjectEventArgs(project, project.BuildSystem.OutputText, succeeded));
			}
		}

		public CodeCountInfo CountCode(string lines)
		{
			int size = 0;
			int min = 0;
			int max = 0;
			_assembler = new SpasmComAssembler();
			string[] outputLines = null;
			if (!string.IsNullOrEmpty(lines))
			{
				_assembler.SetFlags(AssemblyFlags.CodeCounter | AssemblyFlags.Commandline);
				outputLines = _assembler.Assemble(lines).Split('\n');
			}

			if (outputLines == null)
			{
				return new CodeCountInfo(size, min, max);
			}

			foreach (string line in outputLines.Where(line => !string.IsNullOrWhiteSpace(line)))
			{
				if (line.StartsWith("Size:"))
				{
					if (!int.TryParse(line.Substring(6, line.Length - 7), out size))
					{
						size = 0;
					}
				}
				else if (line.StartsWith("Min. execution time:"))
				{
					if (!int.TryParse(line.Substring(20, line.Length - 21), out min))
					{
						min = 0;
					}
				}
				else if (line.StartsWith("Max. execution time:"))
				{
					if (!int.TryParse(line.Substring(20, line.Length - 21), out max))
					{
						max = 0;
					}
				}
			}
			return new CodeCountInfo(size, min, max);
		}


		public void DestroyService()
		{
			_assembler = null;
		}

		public void InitService(params object[] objects)
		{
		}

		public string GetExtension(int outputFile)
		{
			string outputFileExt = "bin";
			switch (outputFile)
			{
			case 1:
				outputFileExt = "73p";
				break;
			case 2:
				outputFileExt = "82p";
				break;
			case 3:
				outputFileExt = "83p";
				break;
			case 4:
				outputFileExt = "8xp";
				break;
			case 5:
				outputFileExt = "8xk";
				break;
			case 6:
				outputFileExt = "85p";
				break;
			case 7:
				outputFileExt = "85s";
				break;
			case 8:
				outputFileExt = "86p";
				break;
			case 9:
				outputFileExt = "86s";
				break;
			}

			return outputFileExt;
		}

		private void OnAssemblerFileFinished(object sender, AssemblyFinishFileEventArgs e)
		{
			if (AssemblerFileFinished != null)
			{
				AssemblerFileFinished(sender, e);
			}
		}

		private void OnAssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
		{
			if (AssemblerProjectFinished != null)
			{
				AssemblerProjectFinished(sender, e);
			}
		}

		public void Dispose()
		{
			Dispose(true);
		}

		private void Dispose(bool disposing)
		{
		    if (_disposed)
		    {
		        return;
		    }

		    if (disposing)
		    {
		        if (_assembler != null)
		        {
		            _assembler.Dispose();
		        }
		    }

		    _disposed = true;
		}
	}
}