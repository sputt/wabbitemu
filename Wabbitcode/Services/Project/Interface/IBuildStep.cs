using System;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utilities;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services.Project.Interface
{
	public interface IBuildStep : ICloneable
	{
		IProject Project { get; }
		int StepNumber { get; set; }
		FilePath InputFile { get; set; }
		string Description { get; }
        bool IsMainOutput { get; }
		bool Build();
	}
}
