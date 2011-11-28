using System;
using System.Collections.Generic;

namespace Revsoft.Wabbitcode.Services.Project.Interface
{
	public interface IBuildConfig
	{
		void Build();
		int GetHashCode();
		string Name { get; }
		IProject Project { get; }
		void SortSteps();
		List<IBuildStep> Steps { get; }
	}
}
