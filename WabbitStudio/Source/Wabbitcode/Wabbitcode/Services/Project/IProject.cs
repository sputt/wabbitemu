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
        bool NeedsSave { get; set; }

        ProjectFolder AddFolder(string dirName, ProjectFolder parentFolder);
        ProjectFile AddFile(ProjectFolder parentFolder, string fullPath);
        void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir);
        void DeleteFile(ProjectFolder parentDir, ProjectFile file);
	}
}
