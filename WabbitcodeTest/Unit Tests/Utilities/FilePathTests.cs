using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Revsoft.Wabbitcode.Utilities;

namespace WabbitcodeTest.Utilities
{
	[TestClass]
	public class FilePathTests
	{
		[TestMethod]
		public void FilePaths_EqualFilePath()
		{
			FilePath path1 = new FilePath(@"C:\Test\Project");
			FilePath path2 = new FilePath(@"C:\Test\Project");
			Assert.AreEqual(path1, path2);
		}

		[TestMethod]
		public void FilePaths_EqualInt()
		{
			FilePath path = new FilePath(@"C:\Test\Project");
			int intFive = 5;
			Assert.AreNotEqual(path, intFive);
		}

		[TestMethod]
		public void FilePaths_NotEqualFilePath()
		{
			FilePath path1 = new FilePath(@"C:\Test\Project");
			FilePath path2 = new FilePath(@"C:\Test\Project\NotSame");
			Assert.AreNotEqual(path1, path2);
		}

		[TestMethod]
		public void FilePaths_ToString()
		{
			string pathString = @"C:\Test\Project";
			FilePath path = new FilePath(pathString);
			Assert.AreEqual(path.ToString(), pathString);
		}

		[TestMethod]
		public void FilePaths_ImplicitString()
		{
			string pathString = @"C:\Test\Project";
			FilePath path = new FilePath(pathString);
			Assert.AreEqual(path, pathString);
		}

		[TestMethod]
		public void FilePaths_ImplicitStringNull()
		{
			string pathString = null;
			FilePath path = new FilePath(pathString);
			Assert.AreEqual(path, pathString);
		}

		[TestMethod]
		public void FilePaths_GetHashCode_Equal()
		{
			FilePath path1 = new FilePath(@"C:\Test\Project");
			FilePath path2 = new FilePath(@"C:\Test\Project");
			Assert.AreEqual(path1.GetHashCode(), path2.GetHashCode());
		}

		[TestMethod]
		public void FilePaths_GetHashCode_NotEqual()
		{
			FilePath path1 = new FilePath(@"C:\Test\Project");
			FilePath path2 = new FilePath(@"C:\Test\Project\NotSame");
			Assert.AreNotEqual(path1.GetHashCode(), path2.GetHashCode());
		}
	}
}
