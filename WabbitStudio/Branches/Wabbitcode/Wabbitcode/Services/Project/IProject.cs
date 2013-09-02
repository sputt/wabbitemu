using System;
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

		IEnumerable<ProjectFile> GetProjectFiles();
		bool ContainsFile(string file);
		ProjectFile FindFile(string fullPath);


		void CreateNewProject(string projectFile, string projectName);
		void OpenProject(string fileName);
		void SaveProject();

		void InitWatcher(FileSystemEventHandler changedHandler, RenamedEventHandler renamedHandler);
	}
}