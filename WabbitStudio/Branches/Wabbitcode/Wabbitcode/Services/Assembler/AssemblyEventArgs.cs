using Revsoft.Wabbitcode.Services.Project;
using System;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    public abstract class AssemblyFinishEventArgs : EventArgs
    {
        protected AssemblyFinishEventArgs(string output, bool succeeded)
        {
            Output = new AssemblerOutput(output, succeeded);
        }

        public AssemblerOutput Output { get; private set; }
    }

    public class AssemblyFinishFileEventArgs : AssemblyFinishEventArgs
    {
        public AssemblyFinishFileEventArgs(string input, string output, string outputText, bool succeeded)
            : base(outputText, succeeded)
        {
            InputFile = input;
            OutputFile = output;
        }

        public string InputFile { get; private set; }

        public string OutputFile { get; private set; }
    }

    public class AssemblyFinishProjectEventArgs : AssemblyFinishEventArgs
    {
        public AssemblyFinishProjectEventArgs(IProject project, string outputText, bool succeeded)
            : base(outputText, succeeded)
        {
            Project = project;
        }

        public IProject Project { get; private set; }
    }
}