using System.Collections.Generic;
using Revsoft.Wabbitcode.Interface.Services;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using System.IO;

namespace Revsoft.Wabbitcode.Interface
{
	public interface IProject
	{
		string ProjectDirectory { get; }
		string ProjectFile { get; }
		string ProjectName { get; set; }
		IProjectFolder MainFolder { get; }
		IList<string> IncludeDirs { get; }
		bool NeedsSave { get; set; }
		bool IsInternal { get; }
		IBuildSystem BuildSystem { get; }
		IList<ParserInformation> ParseInfo { get; }
		IParserService ParserService { get; }

		IList<string> ProjectOutputs { get; }
		IList<string> LabelOutputs { get; }
		IList<string> ListOutputs { get; }

		IProjectFolder AddFolder(string dirName, IProjectFolder parentFolder);
		IProjectFile AddFile(IProjectFolder parentFolder, string fullPath);
		bool ContainsFile(string fullPath);
		void DeleteFolder(IProjectFolder parentDir, IProjectFolder dir);
		void DeleteFile(IProjectFile file);
		void DeleteFile(string fullPath);
		IProjectFile FindFile(string fullPath);

		ParserInformation GetParseInfo(string fullPath);
		IList<IProjectFile> GetProjectFiles();

		Project.AssemblerOutputType GetOutputType();

		void SaveProjectFile(Stream stream);
	}
}
