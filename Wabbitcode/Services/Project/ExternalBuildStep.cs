using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;
using Revsoft.Wabbitcode.Classes;

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

		public bool Build(bool silent)
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
				cmd.Start();
                cmd.WaitForExit(30 * 1000);
                var output = cmd.StandardOutput.ReadToEnd();
                DockingService.MainForm.Invoke(() => DockingService.OutputWindow.AddText(output));
				return true;
#if !DEBUG
			}
			catch (Exception)
			{
				return false;
			}
#endif
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
