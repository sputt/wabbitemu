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
		}
		string output;
		public string OutputFile
		{
			get { return output; }
		}

		StepType stepType;
		public StepType StepType
		{
			get { return stepType; }
		}

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
					break;
				case Project.StepType.Listing:

					break;
				case Project.StepType.SymbolTable:

					break;
				default:
					errors |= AssemblerService.AssembleFile(input, output, true);
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
