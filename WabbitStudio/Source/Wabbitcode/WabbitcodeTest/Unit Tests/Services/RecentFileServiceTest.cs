using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Utilities;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for RecentFileServiceTest and is intended
	///to contain all RecentFileServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class RecentFileServiceTest
	{
		/// <summary>
		///A test for AddRecentFile
		///</summary>
		[TestMethod()]
		public void AddRecentFileTest_FilePath()
		{
			RecentFileService target = new RecentFileService();
			target.InitService();
			FilePath filePath = new FilePath(@"C:\TestPath\Project");
			FilePath filePath2 = new FilePath(@"C:\TestPath\Project\Path 2");
			target.AddRecentFile(filePath);
			target.AddRecentFile(filePath2);
			Assert.IsTrue(target.GetRecentFiles().Contains(filePath));
			Assert.IsTrue(target.GetRecentFiles().Contains(filePath2));
		}

		[TestMethod()]
		public void AddRecentFileTest_Duplicate()
		{
			RecentFileService target = new RecentFileService();
			target.InitService();
			FilePath filePath = new FilePath(@"C:\TestPath\Project");
			target.AddRecentFile(filePath);
			target.AddRecentFile(filePath);
			Assert.IsTrue(target.GetRecentFiles().Count == 1);
		}

		[TestMethod()]
		public void AddRecentFileTest_String()
		{
			RecentFileService target = new RecentFileService();
			target.InitService();
			string file1 = @"C:\TestPath\Project";
			string file2 = @"C:\TestPath\Project\Path 2";
			FilePath filePath = new FilePath(file1);
			FilePath filePath2 = new FilePath(file2);
			target.AddRecentFile(file1);
			target.AddRecentFile(file2);
			Assert.IsTrue(target.GetRecentFiles().Contains(filePath));
			Assert.IsTrue(target.GetRecentFiles().Contains(filePath2));
		}

		/// <summary>
		///A test for DestroyService
		///</summary>
		[TestMethod()]
		public void DestroyServiceTest()
		{
			RecentFileService target = new RecentFileService(); // TODO: Initialize to an appropriate value
			target.DestroyService();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest()
		{
			RecentFileService target = new RecentFileService();
			target.InitService();
			Assert.IsTrue(target.GetRecentFiles().Count == 0);
		}

		/// <summary>
		///A test for SaveRecentFileList
		///</summary>
		[TestMethod()]
		public void SaveRecentFileListTest()
		{
			RecentFileService target = new RecentFileService(); // TODO: Initialize to an appropriate value
			target.SaveRecentFileList();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}
	}
}
