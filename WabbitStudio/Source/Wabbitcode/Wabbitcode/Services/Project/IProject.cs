using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Project
{
	public interface IProject
	{
		string ProjectDirectory { get; }
		string ProjectFile { get; }
		string ProjectName { get; set; }
		ProjectFolder MainFolder { get; }
		List<string> IncludeDir { get; }
	}
}
