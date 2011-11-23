using System.IO;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class InternalBuildStep : IBuildStep
	{
		public IProject Project { get; private set; }

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

		public StepType StepType { get; set; }

		public InternalBuildStep(IProject project, int number, StepType type, string inputFile, string outputFile)
		{
			StepNumber  = number;
			StepType = type;
			InputFile = new FilePath(inputFile);
			OutputFile = new FilePath(outputFile);
			Project = project;
		}

		public bool Build()
		{
			bool errors = false;
			switch (StepType)
			{
				case StepType.All:
					errors |= Project.AssemblerService.AssembleFile(InputFile, OutputFile, flags: AssemblyFlags.Assemble | AssemblyFlags.Label | AssemblyFlags.List);
					Project.ProjectOutputs.Add(OutputFile);
					Project.ListOutputs.Add(Path.ChangeExtension(OutputFile, "lst"));
					Project.LabelOutputs.Add(Path.ChangeExtension(OutputFile, "lab"));
					break;
				case StepType.Listing:
					errors |= Project.AssemblerService.AssembleFile(InputFile, OutputFile, flags: AssemblyFlags.Assemble | AssemblyFlags.List);
					Project.ProjectOutputs.Add(OutputFile);
					Project.ListOutputs.Add(Path.ChangeExtension(OutputFile, "lst"));
					break;
				case StepType.SymbolTable:
					errors |= Project.AssemblerService.AssembleFile(InputFile, OutputFile, flags: AssemblyFlags.Assemble | AssemblyFlags.Label);
					Project.ProjectOutputs.Add(OutputFile);
					Project.LabelOutputs.Add(Path.ChangeExtension(OutputFile, "lab"));
					break;
				default:
					errors |= Project.AssemblerService.AssembleFile(InputFile, OutputFile);
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
				switch (StepType)
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

		public override bool Equals(object obj)
		{
			if (obj is IBuildStep)
			{
				var step = obj as IBuildStep;
				return StepNumber == step.StepNumber;
			}
			else
			{
				return base.Equals(obj);
			}
		}

		public override int GetHashCode()
		{
			if (Project == null)
			{
				return StepNumber.GetHashCode();
			}
			return StepNumber.GetHashCode() + Project.GetHashCode();
		}

		public override string ToString()
		{
			return Description;
		}

		public object Clone()
		{
			return new InternalBuildStep(Project, StepNumber, StepType, InputFile, OutputFile);
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
