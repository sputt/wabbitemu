using System;
using Revsoft.Wabbitcode.Services.Assembler;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Project
{
	public enum BuildStepType
	{
		All,
		Assemble,
		Listing,
		SymbolTable,
	}

	public class InternalBuildStep : IBuildStep
	{
		private string _inputFile;
		private string _outputFile;
		private string _outputText = string.Empty;
		private int _stepNumber;
		private BuildStepType _stepType;

		/// <summary>
		/// Creates a new build step using the built in spasm functions
		/// </summary>
		/// <param name="number">Build step number (0 indexed)</param>
		/// <param name="type">Type of internal operation to peform</param>
		/// <param name="inputFile">File to input to spasm</param>
		/// <param name="outputFile">File expected to be received</param>
		public InternalBuildStep(int number, BuildStepType type, string inputFile, string outputFile)
		{
			_stepNumber = number;
			_stepType = type;
			_inputFile = inputFile;
			_outputFile = outputFile;
		}

		public string Description
		{
			get
			{
				string fileName = Path.GetFileName(_inputFile);
				switch (_stepType)
				{
					case BuildStepType.All:
						return "Assemble and list " + fileName;
					case BuildStepType.Listing:
						return "List " + fileName;
					case BuildStepType.SymbolTable:
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
				return _inputFile;
			}
			set
			{
				_inputFile = value;
			}
		}

		/*
				public string InputFileRelative
				{
					get
					{
						return Path.Combine(
								   FileOperations.GetRelativePath(Path.GetDirectoryName(_inputFile),
										   ProjectService.ProjectDirectory),
								   Path.GetFileName(_inputFile));
					}
				}
		*/

		public string OutputFile
		{
			get
			{
				return _outputFile;
			}
			set
			{
				_outputFile = value;
			}
		}

		/*
				public string OutputFileRelative
				{
					get
					{
						return Path.Combine(
								   FileOperations.GetRelativePath(Path.GetDirectoryName(_outputFile),
										   ProjectService.ProjectDirectory),
								   Path.GetFileName(_outputFile));
					}
				}
		*/

		public string OutputText
		{
			get
			{
				return _outputText;
			}
		}

		public int StepNumber
		{
			get
			{
				return _stepNumber;
			}
			set
			{
				_stepNumber = value;
			}
		}

		public BuildStepType StepType
		{
			get
			{
				return _stepType;
			}
			set
			{
				_stepType = value;
			}
		}

		public bool Build(IProject project)
		{
		    IAssembler assembler = AssemblerFactory.CreateAssembler();
			AssemblerOutput output;
		    string outputString;
			switch (_stepType)
			{
				case BuildStepType.All:
                    AssemblerHelper.SetupAssembler(assembler, _inputFile, _outputFile, project.ProjectDirectory,
                        project.IncludeDirs, AssemblyFlags.Normal | AssemblyFlags.SymbolTable | AssemblyFlags.List);
			        outputString = assembler.Assemble();
			        output = new AssemblerOutput(outputString, !outputString.Contains("error") && !outputString.Contains("Couldn't"));
					project.BuildSystem.ProjectOutput = _outputFile;
					project.BuildSystem.ListOutput = Path.ChangeExtension(_outputFile, "lst");
					project.BuildSystem.LabelOutput = Path.ChangeExtension(_outputFile, "lab");
					break;
                case BuildStepType.Assemble:
                    AssemblerHelper.SetupAssembler(assembler, _inputFile, _outputFile, project.ProjectDirectory,
                        project.IncludeDirs, AssemblyFlags.Normal);
			        outputString = assembler.Assemble();
			        output = new AssemblerOutput(outputString, !outputString.Contains("error") && !outputString.Contains("Couldn't"));
					project.BuildSystem.ProjectOutput = _outputFile;
					break;
				case BuildStepType.Listing:
                    AssemblerHelper.SetupAssembler(assembler, _inputFile, _outputFile, project.ProjectDirectory,
                        project.IncludeDirs, AssemblyFlags.Normal | AssemblyFlags.List);
			        outputString = assembler.Assemble();
                    output = new AssemblerOutput(outputString, !outputString.Contains("error") && !outputString.Contains("Couldn't"));
					project.BuildSystem.ProjectOutput = _outputFile;
					project.BuildSystem.ListOutput = Path.ChangeExtension(_outputFile, "lst");
					break;
				case BuildStepType.SymbolTable:
					AssemblerHelper.SetupAssembler(assembler, _inputFile, _outputFile, project.ProjectDirectory,
                        project.IncludeDirs, AssemblyFlags.Normal | AssemblyFlags.SymbolTable);
			        outputString = assembler.Assemble();
                    output = new AssemblerOutput(outputString, !outputString.Contains("error") && !outputString.Contains("Couldn't"));
					project.BuildSystem.ProjectOutput = _outputFile;
					project.BuildSystem.LabelOutput = Path.ChangeExtension(_outputFile, "lab");
					break;
				default:
					throw new InvalidOperationException("Unknown step type");
			}

			_outputText = output.OutputText;
			return output.Succeeded;
		}

		public object Clone()
		{
			return new InternalBuildStep(_stepNumber, _stepType, _inputFile, _outputFile);
		}

		public override string ToString()
		{
			return Description;
		}

        public int CompareTo(object obj)
        {
            if (obj == null)
            {
                return -1;
            }

            IBuildStep step = obj as IBuildStep;
            if (step == null)
            {
                return -1;
            }

            return StepNumber.CompareTo(step.StepNumber);
        }
	}
}