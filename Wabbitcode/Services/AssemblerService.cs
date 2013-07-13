using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services
{
    public class AssemblerService : IService, IDisposable
    {
        private static AssemblerService instance;

        private bool disposed = false;
        private IAssembler assembler;
        private string outputFormatting = "=================={0}==================" + Environment.NewLine +
                                          "Assembling {1}" + Environment.NewLine + "{2}";
        private bool projectSuppressEvents;

        private bool isAssembling = false;

        private bool projectAssembleRequests = false;

        public delegate void OnFinishAssemblyFile(object sender, AssemblyFinishFileEventArgs e);

        public delegate void OnFinishAssemblyProject(object sender, AssemblyFinishProjectEventArgs e);

        public event OnFinishAssemblyFile AssemblerFileFinished;

        public event OnFinishAssemblyProject AssemblerProjectFinished;

        public static AssemblerService Instance
        {
            get
            {
                if (instance == null)
                {
                    instance = new AssemblerService();
                }

                return instance;
            }
        }

        public AssemblerOutput AssembleFile(string inputFile)
        {
            string outputFile = Path.ChangeExtension(inputFile, this.GetExtension(Properties.Settings.Default.outputFile));
            return this.AssembleFile(inputFile, outputFile, AssemblyFlags.Normal, false);
        }

        public AssemblerOutput AssembleFile(string inputFile, string outputFile)
        {
            return this.AssembleFile(inputFile, outputFile, AssemblyFlags.Normal, false);
        }

        public AssemblerOutput AssembleFile(string inputFile, string outputFile, AssemblyFlags flags)
        {
            return this.AssembleFile(inputFile, outputFile, AssemblyFlags.Normal, this.projectSuppressEvents);
        }

        public AssemblerOutput AssembleFile(string inputFile, string outputFile, AssemblyFlags flags, bool suppressEvents)
        {
            this.assembler = new SpasmExeAssembler();

            string originalDir = ProjectService.IsInternal ? Path.GetDirectoryName(inputFile) : ProjectService.ProjectDirectory;
            this.assembler.SetWorkingDirectory(originalDir);

            // include dirs
            this.assembler.AddIncludeDir(Application.StartupPath);
            if (!string.IsNullOrEmpty(Properties.Settings.Default.includeDir) || !ProjectService.IsInternal)
            {
                List<string> dirs = ProjectService.IsInternal ?
                                    Properties.Settings.Default.includeDir.Split('\n').ToList<string>() :
                                    ProjectService.Project.IncludeDir;
                foreach (string dir in dirs)
                {
                    this.assembler.AddIncludeDir(dir);
                }
            }

            // setup files
            this.assembler.SetInputFile(inputFile);
            this.assembler.SetOutputFile(outputFile);

            // set flags
            this.assembler.SetFlags(flags);
            this.assembler.SetCaseSensitive(Properties.Settings.Default.caseSensitive);

            // assemble
            string rawOutput = this.assembler.Assemble();

            // lets write it to the output window so the user knows whats happening
            string outputText = string.Format(this.outputFormatting, Path.GetFileName(inputFile),  inputFile, rawOutput);
            bool errors = outputText.Contains("error");
            if (!suppressEvents)
            {
                this.OnAssemblerFileFinished(this, new AssemblyFinishFileEventArgs(inputFile, outputFile, outputText, !errors));
            }

            // tell if the assembly was successful
            return new AssemblerOutput(outputText, !errors);
        }

        public void AssembleProject(IProject project, bool suppressEvents = false)
        {
            if (projectAssembleRequests)
            {
                return;
            }
            if (this.isAssembling)
            {
                this.projectAssembleRequests = true;
                return;
            }
            this.isAssembling = true;
            ProjectService.ProjectWatcher.EnableRaisingEvents = false;

            this.projectSuppressEvents = suppressEvents;
            bool succeeded = project.BuildSystem.Build();

            ProjectService.ProjectWatcher.EnableRaisingEvents = true;
            if (!suppressEvents)
            {
                this.OnAssemblerProjectFinished(this, new AssemblyFinishProjectEventArgs(project, project.BuildSystem.OutputText, succeeded));
            }
            this.isAssembling = false;
            if (this.projectAssembleRequests)
            {
                this.projectAssembleRequests = false;
                AssembleProject(project, suppressEvents);
            }
        }

        public CodeCountInfo CountCode(string lines)
        {
            int size = 0;
            int min = 0;
            int max = 0;
            this.assembler = new SpasmExeAssembler();
            string[] outputLines = null;
            if (!string.IsNullOrEmpty(lines))
            {
                assembler.SetFlags(AssemblyFlags.CodeCounter | AssemblyFlags.Commandline);
                outputLines = assembler.Assemble(lines).Split('\n');
            }

            foreach (string line in outputLines)
            {
                if (string.IsNullOrWhiteSpace(line))
                {
                    continue;
                }
                else if (line.StartsWith("Size:"))
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
            this.assembler = null;
        }

        public void InitService(params object[] objects)
        {
            if (objects.Length != 1)
            {
                throw new ArgumentException("Invalid number of arguments");
            }

            this.assembler = objects[0] as IAssembler;
            if (this.assembler == null)
            {
                throw new ArgumentException("First is not of type of IAssembler");
            }
        }

        internal string GetExtension(int outputFile)
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

        protected void OnAssemblerFileFinished(object sender, AssemblyFinishFileEventArgs e)
        {
            if (this.AssemblerFileFinished != null)
            {
                this.AssemblerFileFinished(sender, e);
            }
        }

        protected void OnAssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
        {
            if (this.AssemblerProjectFinished != null)
            {
                this.AssemblerProjectFinished(sender, e);
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    if (assembler != null)
                    {
                        assembler.Dispose();
                    }
                }

                disposed = true;
            }
        }
    }
}