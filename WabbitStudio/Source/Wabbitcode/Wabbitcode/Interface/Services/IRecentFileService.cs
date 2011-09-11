using System.Collections.Generic;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Interface.Services
{
	interface IRecentFileService : IService
	{
		void AddRecentFile(string filePath);
		void AddRecentFile(FilePath filePath);
		IList<FilePath> GetRecentFiles();
		void SaveRecentFileList();
	}
}
