using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Interface.Services;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Services
{
	public class RecentFileService : IRecentFileService
	{
		List<FilePath> recentFileList;

		public void AddRecentFile(string filePath)
		{
			AddRecentFile(new FilePath(filePath));
		}

		public void AddRecentFile(FilePath filePath)
		{
			if (recentFileList.Contains(filePath))
				return;
			recentFileList.Add(filePath);
		}

		public void SaveRecentFileList()
		{
			
		}

		public IList<FilePath> GetRecentFiles()
		{
			return recentFileList;
		}

		public void InitService(params Object[] objects)
		{
			recentFileList = new List<FilePath>();
		}

		public void DestroyService() 
		{
			SaveRecentFileList();
		}
	}
}
