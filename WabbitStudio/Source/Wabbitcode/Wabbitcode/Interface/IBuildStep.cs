using System;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Interface
{
	public interface IBuildStep : ICloneable
	{
		IProject Project { get; }
		int StepNumber { get; set; }
		FilePath InputFile { get; set; }
		string Description { get; }
		bool Build();
	}
}
