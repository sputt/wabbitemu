using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class InternalBuildStep : IBuildStep
	{
		int stepNumber;
		public int StepNumber
		{
			get { return stepNumber; }
			set { stepNumber = value; }
		}

		string input;
		public string InputFile
		{
			get { return input; }
            set { input = value; }
		}

        public string InputFileRelative
        {
            get
            {
                return Path.Combine(FileOperations.GetRelativePath(Path.GetDirectoryName(input),
                    ProjectService.ProjectDirectory), Path.GetFileName(input));
            }
        }

		string output;
		public string OutputFile
		{
			get { return output; }
            set { output = value; }
		}

        public string OutputFileRelative
        {
            get
            {
                return Path.Combine(FileOperations.GetRelativePath(Path.GetDirectoryName(output),
                    ProjectService.ProjectDirectory), Path.GetFileName(output));
            }
        }

		StepType stepType;
		public StepType StepType
		{
			get { return stepType; }
            set { stepType = value; }
		}

		/// <summary>
		/// Creates a new build step using the built in spasm functions
		/// </summary>
		/// <param name="number">Build step number (0 indexed)</param>
		/// <param name="type">Type of internal operation to peform</param>
		/// <param name="inputFile">File to input to spasm</param>
		/// <param name="outputFile">File expected to be received</param>
		public InternalBuildStep(int number, StepType type, string inputFile, string outputFile)
		{
			stepNumber = number;
			stepType = type;
			input = inputFile;
			output = outputFile;
		}

		public bool Build()
		{
			bool errors = false;
			switch (stepType)
			{
				case Project.StepType.All:
					errors |= AssemblerService.AssembleFile(input, output, true);
					ProjectService.Project.ProjectOutputs.Add(output);
                    ProjectService.Project.ListOutputs.Add(Path.ChangeExtension(output, "lst"));
                    ProjectService.Project.LabelOutputs.Add(Path.ChangeExtension(output, "lab"));
					break;
				case Project.StepType.Listing:

					break;
				case Project.StepType.SymbolTable:

					break;
				default:
					errors |= AssemblerService.AssembleFile(input, output, true);
					ProjectService.Project.ProjectOutputs.Add(output);
					break;
			}
			return errors;
		}


		public string Description
		{
			get
			{
				string fileName = Path.GetFileName(input);
				switch (stepType)
				{
					case Project.StepType.All:
						return "Assemble and list " + fileName;
					case Project.StepType.Listing:
						return "List " + fileName;
					case Project.StepType.SymbolTable:
						return "Symbolize " + fileName;
					default:
						return "Assemble " + fileName;
				}
			}
		}

		public override string ToString()
		{
			return Description;
		}

		public object Clone()
		{
			return new InternalBuildStep(this.stepNumber, this.stepType, this.input, this.output);
		}
	}

	public enum StepType
	{
		All,
		Assemble,
		Listing,
		SymbolTable,
	}
}
