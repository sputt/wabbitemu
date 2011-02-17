using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Project
{
	public interface IProject
	{
		string ProjectDirectory { get; }
		string ProjectFile { get; }
		string ProjectName { get; set; }
		ProjectFolder MainFolder { get; }
		List<string> IncludeDirs { get; }
        bool NeedsSave { get; set; }
        bool IsInternal { get; }
        BuildSystem BuildSystem { get; }
        IList<ParserInformation> ParseInfo { get; }

        ProjectFolder AddFolder(string dirName, ProjectFolder parentFolder);
        ProjectFile AddFile(ProjectFolder parentFolder, string fullPath);
        void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir);
        void DeleteFile(ProjectFile file);
        void DeleteFile(string fullPath);
	}
}
