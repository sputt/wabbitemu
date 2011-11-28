using System;

namespace Revsoft.Wabbitcode.Services.Project.Interface
{
	public interface IProjectFile
	{
		string FileFoldings { get; set; }
		string FileFullPath { get; set; }
		string FileRelativePath { get; set; }
		IProjectFolder Folder { get; set; }
		IProject Parent { get; }

		void Remove();
	}
}
