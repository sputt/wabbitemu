namespace Revsoft.Wabbitcode.Services.Project
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Linq;
    using System.Text;

    using Revsoft.Wabbitcode.Classes;

    public class ExternalBuildStep : IBuildStep
    {
        private const int processTimeout = 30 * 1000;

        private string args;
        private string input;
        private string outputText = String.Empty;
        private int stepNumber;

        public ExternalBuildStep(int number, string commandLine)
        {
            this.stepNumber = number;
            int spaceIndex = commandLine.IndexOf(' ');
            if (spaceIndex == -1)
            {
                this.input = commandLine;
                this.args = "";
            }
            else
            {
                this.input = commandLine.Substring(0, spaceIndex);
                this.args = commandLine.Substring(spaceIndex, commandLine.Length - spaceIndex);
            }
        }

        public ExternalBuildStep(int number, string program, string arguments)
        {
            this.stepNumber = number;
            this.input = program;
            this.args = arguments;
        }

        public string Arguments
        {
            get
            {
                return this.args;
            }
        }

        public string Description
        {
            get
            {
                return "Run " + Path.GetFileName(this.input);
            }
        }

        public string InputFile
        {
            get
            {
                return this.input;
            }
            set
            {
                this.input = value;
            }
        }

        public string OutputText
        {
            get
            {
                return this.outputText;
            }
        }

        public int StepNumber
        {
            get
            {
                return this.stepNumber;
            }
            set
            {
                this.stepNumber = value;
            }
        }

        public bool Build()
        {
            try
            {
                Process cmd = new Process
                {
                    StartInfo =
                    {
                        FileName = this.input,
                        WorkingDirectory = ProjectService.ProjectDirectory,
                        UseShellExecute = false,
                        CreateNoWindow = true,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        Arguments = this.args,
                    }
                };
                cmd.Start();
                cmd.WaitForExit(processTimeout);
                this.outputText = cmd.StandardOutput.ReadToEnd();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        public object Clone()
        {
            return new ExternalBuildStep(this.stepNumber, this.input, this.args);
        }

        public override string ToString()
        {
            return this.Description;
        }
    }
}