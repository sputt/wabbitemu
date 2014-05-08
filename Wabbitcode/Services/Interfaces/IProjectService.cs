using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IProjectService : IService
	{
        event EventHandler ProjectOpened;
        event EventHandler ProjectClosed;
        event EventHandler ProjectFileAdded;
        event EventHandler ProjectFolderAdded;
        event EventHandler ProjectFileRemoved;
        event EventHandler ProjectFolderRemoved;

		IProject Project { get; }

        bool OpenProject(FilePath fileName);
		ProjectFile AddFile(ProjectFolder parent, FilePath fullPath);
        ProjectFolder AddFolder(string folderName, ProjectFolder parentDir);
		void CloseProject();
        bool ContainsFile(FilePath file);
		IProject CreateInternalProject();
        IProject CreateNewProject(FilePath projectFile, string projectName);
        void DeleteFile(FilePath fullPath);
		void DeleteFile(ProjectFolder parentDir, ProjectFile file);
		void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir);
        void RemoveParseData(FilePath fullPath);
        ParserInformation GetParseInfo(FilePath file);
		void SaveProject();
		IEnumerable<List<Reference>> FindAllReferences(string refString);
	}
}