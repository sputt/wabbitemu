using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
	using System;
	using System.Diagnostics;
	using System.IO;


	public class ExternalBuildStep : IBuildStep
	{
		private const int ProcessTimeout = 30 * 1000;

		private readonly string _args;
		private string _input;
		private string _outputText = string.Empty;
		private int _stepNumber;

		public ExternalBuildStep(int number, string program, string arguments)
		{
			_stepNumber = number;
			_input = program;
			_args = arguments;
		}

		public string Arguments
		{
			get
			{
				return _args;
			}
		}

		public string Description
		{
			get
			{
				return "Run " + Path.GetFileName(_input);
			}
		}

		public string InputFile
		{
			get
			{
				return _input;
			}
			set
			{
				_input = value;
			}
		}

		public string OutputText
		{
			get
			{
				return _outputText;
			}
		}

		public int StepNumber
		{
			get
			{
				return _stepNumber;
			}
			set
			{
				_stepNumber = value;
			}
		}

		public bool Build(IAssemblerService assemblerService, IProject project)
		{
			try
			{
				Process cmd = new Process
				{
					StartInfo =
					{
						FileName = _input,
						WorkingDirectory = project.ProjectDirectory,
						UseShellExecute = false,
						CreateNoWindow = true,
						RedirectStandardOutput = true,
						RedirectStandardError = true,
						Arguments = _args,
					}
				};
				cmd.Start();
				cmd.WaitForExit(ProcessTimeout);
				_outputText = cmd.StandardOutput.ReadToEnd();
				return true;
			}
			catch (Exception)
			{
				return false;
			}
		}

		public object Clone()
		{
			return new ExternalBuildStep(_stepNumber, _input, _args);
		}

		public override string ToString()
		{
			return Description;
		}
	}
}