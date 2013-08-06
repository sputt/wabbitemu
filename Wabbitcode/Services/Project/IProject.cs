using System.IO;

namespace Revsoft.Wabbitcode.Services.Project
{
	using System.Collections.Generic;

	public interface IProject
	{
		IBuildSystem BuildSystem
		{
			get;
		}

		IList<string> IncludeDirs
		{
			get;
		}

		ProjectFolder MainFolder
		{
			get;
		}

		bool NeedsSave
		{
			get;
		}

		string ProjectDirectory
		{
			get;
		}

		string ProjectFile
		{
			get;
		}

		string ProjectName
		{
			get;
			set;
		}

		bool IsInternal
		{
			get;
			set;
		}

		FileSystemWatcher ProjectWatcher { get; set; }

		ProjectFile AddFile(ProjectFolder parentFolder, string fullPath);

		ProjectFolder AddFolder(string dirName, ProjectFolder parentFolder);

		void DeleteFile(ProjectFolder parentDir, ProjectFile file);

		void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir);

		IEnumerable<ProjectFile> GetProjectFiles();

		void SaveProject();
	}
}