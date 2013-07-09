namespace Revsoft.Wabbitcode.Services.Project
{
    using Revsoft.Wabbitcode.Services.Assembler;
    using System;
    using System.IO;

    public enum BuildStepType
    {
        All,
        Assemble,
        Listing,
        SymbolTable,
    }

    public class InternalBuildStep : IBuildStep
    {
        private string inputFile;
        private string outputFile;
        private string outputText = String.Empty;
        private int stepNumber;
        private BuildStepType stepType;

        /// <summary>
        /// Creates a new build step using the built in spasm functions
        /// </summary>
        /// <param name="number">Build step number (0 indexed)</param>
        /// <param name="type">Type of internal operation to peform</param>
        /// <param name="inputFile">File to input to spasm</param>
        /// <param name="outputFile">File expected to be received</param>
        public InternalBuildStep(int number, BuildStepType type, string inputFile, string outputFile)
        {
            this.stepNumber = number;
            this.stepType = type;
            this.inputFile = inputFile;
            this.outputFile = outputFile;
        }

        public string Description
        {
            get
            {
                string fileName = Path.GetFileName(this.inputFile);
                switch (this.stepType)
                {
                case Project.BuildStepType.All:
                    return "Assemble and list " + fileName;
                case Project.BuildStepType.Listing:
                    return "List " + fileName;
                case Project.BuildStepType.SymbolTable:
                    return "Symbolize " + fileName;
                default:
                    return "Assemble " + fileName;
                }
            }
        }

        public string InputFile
        {
            get
            {
                return this.inputFile;
            }
            set
            {
                this.inputFile = value;
            }
        }

        public string InputFileRelative
        {
            get
            {
                return Path.Combine(
                           FileOperations.GetRelativePath(Path.GetDirectoryName(this.inputFile),
                                   ProjectService.ProjectDirectory),
                           Path.GetFileName(this.inputFile));
            }
        }

        public string OutputFile
        {
            get
            {
                return this.outputFile;
            }
            set
            {
                this.outputFile = value;
            }
        }

        public string OutputFileRelative
        {
            get
            {
                return Path.Combine(
                           FileOperations.GetRelativePath(Path.GetDirectoryName(this.outputFile),
                                   ProjectService.ProjectDirectory),
                           Path.GetFileName(this.outputFile));
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

        public BuildStepType StepType
        {
            get
            {
                return this.stepType;
            }
            set
            {
                this.stepType = value;
            }
        }

        public bool Build()
        {
            AssemblerOutput output;
            switch (this.stepType)
            {
            case Project.BuildStepType.All:
                output = AssemblerService.Instance.AssembleFile(
                             this.inputFile,
                             this.outputFile,
                             AssemblyFlags.Normal | AssemblyFlags.Symtable | AssemblyFlags.List);
                ProjectService.Project.ProjectOutputs.Add(this.outputFile);
                ProjectService.Project.ListOutputs.Add(Path.ChangeExtension(this.outputFile, "lst"));
                ProjectService.Project.LabelOutputs.Add(Path.ChangeExtension(this.outputFile, "lab"));
                break;
            case Project.BuildStepType.Listing:
                output = AssemblerService.Instance.AssembleFile(
                             this.inputFile,
                             this.outputFile,
                             AssemblyFlags.Normal | AssemblyFlags.List);
                ProjectService.Project.ProjectOutputs.Add(this.outputFile);
                ProjectService.Project.ListOutputs.Add(Path.ChangeExtension(this.outputFile, "lst"));
                break;
            case Project.BuildStepType.SymbolTable:
                output = AssemblerService.Instance.AssembleFile(
                             this.inputFile,
                             this.outputFile,
                             AssemblyFlags.Normal | AssemblyFlags.Symtable);
                ProjectService.Project.ProjectOutputs.Add(this.outputFile);
                ProjectService.Project.LabelOutputs.Add(Path.ChangeExtension(this.outputFile, "lab"));
                break;
            default:
                output = AssemblerService.Instance.AssembleFile(this.inputFile, this.outputFile, AssemblyFlags.Normal);
                ProjectService.Project.ProjectOutputs.Add(this.outputFile);
                break;
            }

            this.outputText = output.OutputText;
            return !this.outputText.Contains("error");
        }

        public object Clone()
        {
            return new InternalBuildStep(this.stepNumber, this.stepType, this.inputFile, this.outputFile);
        }

        public override string ToString()
        {
            return this.Description;
        }
    }
}