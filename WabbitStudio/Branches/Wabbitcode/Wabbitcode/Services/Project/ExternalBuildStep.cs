using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class ExternalBuildStep : IBuildStep
	{
		string args;
		public string Arguments
		{
			get { return args; }
		}

		string input;
		public string InputFile
		{
			get { return input; }
            set { input = value; }
		}

		int stepNumber;
		public int StepNumber
		{
			get { return stepNumber; }
			set { stepNumber = value; }
		}

		public ExternalBuildStep(int number, string commandLine)
		{
			stepNumber = number;
			int spaceIndex = commandLine.IndexOf(' ');
			if (spaceIndex == -1)
			{
				input = commandLine;
				args = "";
			}
			else
			{
				input = commandLine.Substring(0, spaceIndex);
				args = commandLine.Substring(spaceIndex, commandLine.Length - spaceIndex);
			}

		}

		public ExternalBuildStep(int number, string program, string arguments)
		{
			stepNumber = number;
			input = program;
			args = arguments;
		}

		public bool Build()
		{
#if !DEBUG
			try
			{
#endif
				Process cmd = new Process
				{
					StartInfo =
					{
						FileName = input,
						WorkingDirectory = ProjectService.ProjectDirectory,
						UseShellExecute = false,
						CreateNoWindow = true,
						RedirectStandardOutput = true,
						RedirectStandardError = true,
						Arguments = args,
					}
				};
				cmd.Exited += new EventHandler(externalProgramExit);
				cmd.Start();
				return true;
#if !DEBUG
			}
			catch (Exception)
			{
				return false;
			}
#endif
		}

		private void externalProgramExit(object sender, EventArgs e)
		{
			Process cmd = (Process)sender;
			DockingService.OutputWindow.AddText(cmd.StandardOutput.ReadToEnd());
		}


		public string Description
		{
			get { return "Run " + Path.GetFileName(input); }
		}

		public override string ToString()
		{
			return Description;
		}

		public object Clone()
		{
			return new ExternalBuildStep(this.stepNumber, this.input, this.args);
		}
	}
}
