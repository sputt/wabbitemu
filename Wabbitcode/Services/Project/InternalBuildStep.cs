using System.IO;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class InternalBuildStep : IBuildStep
	{
		public IProject Project
		{
			get;
			private set;
		}

		public int StepNumber { get; set; }

		public FilePath InputFile  { get; set; }

		public string InputFileRelative
		{
			get
			{
				return Path.Combine(FileOperations.GetRelativePath(Path.GetDirectoryName(InputFile),
					Project.ProjectDirectory), Path.GetFileName(InputFile));
			}
		}

		public FilePath OutputFile { get; set; }

		public string OutputFileRelative
		{
			get
			{
				return Path.Combine(FileOperations.GetRelativePath(Path.GetDirectoryName(OutputFile),
					Project.ProjectDirectory), Path.GetFileName(OutputFile));
			}
		}

		StepType stepType;
		public StepType StepType
		{
			get { return stepType; }
			set { stepType = value; }
		}

		public InternalBuildStep(IProject project, int number, StepType type, string inputFile, string outputFile)
		{
			StepNumber  = number;
			stepType = type;
			InputFile = new FilePath(inputFile);
			OutputFile = new FilePath(outputFile);
		}

		public bool Build()
		{
			bool errors = false;
			switch (stepType)
			{
				case StepType.All:
					errors |= AssemblerService.Instance.AssembleFile(InputFile, OutputFile, flags: AssemblyFlags.Assemble | AssemblyFlags.Label | AssemblyFlags.List);
					Project.ProjectOutputs.Add(OutputFile);
					Project.ListOutputs.Add(Path.ChangeExtension(OutputFile, "lst"));
					Project.LabelOutputs.Add(Path.ChangeExtension(OutputFile, "lab"));
					break;
				case StepType.Listing:
					errors |= AssemblerService.Instance.AssembleFile(InputFile, OutputFile, flags: AssemblyFlags.Assemble | AssemblyFlags.List);
					Project.ProjectOutputs.Add(OutputFile);
					Project.ListOutputs.Add(Path.ChangeExtension(OutputFile, "lst"));
					break;
				case StepType.SymbolTable:
					errors |= AssemblerService.Instance.AssembleFile(InputFile, OutputFile, flags: AssemblyFlags.Assemble | AssemblyFlags.Label);
					Project.ProjectOutputs.Add(OutputFile);
					Project.LabelOutputs.Add(Path.ChangeExtension(OutputFile, "lab"));
					break;
				default:
					errors |= AssemblerService.Instance.AssembleFile(InputFile, OutputFile);
					Project.ProjectOutputs.Add(OutputFile);
					break;
			}
			return errors;
		}


		public string Description
		{
			get
			{
				string fileName = Path.GetFileName(InputFile);
				switch (stepType)
				{
					case StepType.All:
						return "Assemble and list " + fileName;
					case StepType.Listing:
						return "List " + fileName;
					case StepType.SymbolTable:
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
			return new InternalBuildStep(Project, StepNumber, stepType, InputFile, OutputFile);
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
