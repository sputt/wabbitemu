using System;
using System.Collections.Generic;

namespace Revsoft.Wabbitcode.Services.Project
{

	public interface IProject
	{
	    event EventHandler<FileModifiedEventArgs> FileModifiedExternally;

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

		bool IsInternal{ get; }

	    IEnumerable<ProjectFile> GetProjectFiles();
		bool ContainsFile(string file);
		ProjectFile FindFile(string fullPath);
	    string GetFilePathFromRelativePath(string relativePath);


		void CreateNewProject(string projectFile, string projectName);
		void OpenProject(string projectFile);
		void SaveProject();
	}

    public class FileModifiedEventArgs : EventArgs
    {
        public ProjectFile File { get; set; }

        public FileModifiedEventArgs(ProjectFile file)
        {
            File = file;
        }
    }
}