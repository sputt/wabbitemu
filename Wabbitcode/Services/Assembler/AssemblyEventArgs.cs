namespace Revsoft.Wabbitcode.Services.Assembler
{
    using Revsoft.Wabbitcode.Services.Project;
    using System;

    public abstract class AssemblyFinishEventArgs : EventArgs
    {
        public AssemblyFinishEventArgs(string output, bool succeeded)
        {
            this.Output = new AssemblerOutput(output, succeeded);
            this.AssemblySucceeded = succeeded;
        }

        public bool AssemblySucceeded
        {
            get;
            private set;
        }

        public AssemblerOutput Output
        {
            get;
            private set;
        }
    }

    public class AssemblyFinishFileEventArgs : AssemblyFinishEventArgs
    {
        public AssemblyFinishFileEventArgs(string input, string output, string outputText, bool succeeded)
        : base(outputText, succeeded)
        {
            InputFile = input;
            OutputFile = output;
        }

        public string InputFile
        {
            get;
            private set;
        }

        public string OutputFile
        {
            get;
            private set;
        }
    }

    public class AssemblyFinishProjectEventArgs : AssemblyFinishEventArgs
    {
        public AssemblyFinishProjectEventArgs(IProject project, string outputText, bool succeeded)
        : base(outputText, succeeded)
        {
            Project = project;
        }

        public IProject Project
        {
            get;
            private set;
        }
    }
}