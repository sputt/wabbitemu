using System.Collections.Generic;
using System.IO;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Services
{
	public interface IProjectService : IService
	{
		WabbitcodeProject Project { get; }
		bool OpenProject(string fileName);
		ProjectFile AddFile(ProjectFolder parent, string fullPath);
		ProjectFolder AddFolder(string dirName, ProjectFolder parentDir);
		void CloseProject();
		bool ContainsFile(string file);
		void CreateInternalProject();
		void CreateNewProject(string projectFile, string projectName);
		void DeleteFile(string fullPath);
		void DeleteFile(ProjectFolder parentDir, ProjectFile file);
		void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir);
		void RemoveParseData(string fullPath);
		ParserInformation GetParseInfo(string file);
		void SaveProject();
		IEnumerable<List<Reference>> FindAllReferences(string refString);
	}
}