using System.Collections.Generic;

namespace Revsoft.Wabbitcode.Services.Project.Interface
{
	public interface IProjectFolder
	{
		IList<IProjectFile> Files { get; }
		IList<IProjectFolder> Folders { get; }
		string Name { get; set; }
		IProject Parent { get; }
		IProjectFolder ParentFolder { get; set; }

		void AddFolder(IProjectFolder subFolder);
		void AddFile(IProjectFile file);
		IProjectFile FindFile(string file);
		IProjectFolder FindFolder(string folder);
		void Remove();
	}
}
