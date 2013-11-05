using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IProjectService : IService
	{
		IProject Project { get; }

		bool OpenProject(string fileName);
		ProjectFile AddFile(ProjectFolder parent, string fullPath);
		ProjectFolder AddFolder(string dirName, ProjectFolder parentDir);
		void CloseProject();
		bool ContainsFile(string file);
		IProject CreateInternalProject();
		IProject CreateNewProject(string projectFile, string projectName);
		void DeleteFile(string fullPath);
		void DeleteFile(ProjectFolder parentDir, ProjectFile file);
		void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir);
		void RemoveParseData(string fullPath);
		ParserInformation GetParseInfo(string file);
		void SaveProject();
		IEnumerable<List<Reference>> FindAllReferences(string refString);
	}
}