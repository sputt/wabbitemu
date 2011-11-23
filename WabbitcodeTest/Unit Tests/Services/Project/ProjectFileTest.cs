using Revsoft.Wabbitcode.Services.Project;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Interface;
using System.IO;
using Moq;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for ProjectFileTest and is intended
	///to contain all ProjectFileTest Unit Tests
	///</summary>
	[TestClass()]
	public class ProjectFileTest
	{
		/// <summary>
		///A test for Remove
		///</summary>
		[TestMethod()]
		public void RemoveTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string fullPath = @"C:\Users\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			ProjectFolder projectFolder = new ProjectFolder(project, "Folder");
			projectFolder.AddFile(target);
			target.Remove();

			Assert.AreEqual(projectFolder.Files.Count, 0);
		}

		/// <summary>
		///A test for ToString
		///</summary>
		[TestMethod()]
		public void ProjectFile_ToStringTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string fullPath = @"C:\Users\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			string actual = target.ToString();
			Assert.AreEqual(fullPath, actual);
		}

		/// <summary>
		///A test for FileFullPath
		///</summary>
		[TestMethod()]
		public void FileFullPathTest_FullPath()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string fullPath = @"C:\Users\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			target.FileFullPath = fullPath;
			string actual = target.FileFullPath;
			Assert.AreEqual(fullPath, actual);
		}

		[TestMethod()]
		public void FileFullPathTest_RelativePath()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string fullPath = @"..\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			target.FileFullPath = fullPath;
			string actual = target.FileFullPath;
			Assert.AreEqual(@"C:\Users\Project\Test\file.8xp", actual);
		}

		/// <summary>
		///A test for FileRelativePath
		///</summary>
		[TestMethod()]
		public void FileRelativePathTest_RelativePath()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string fullPath = @"..\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			target.FileRelativePath = fullPath;
			string actual = target.FileRelativePath;
			Assert.AreEqual(fullPath, actual);
		}

		[TestMethod()]
		public void FileRelativePathTest_FullPath()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string fullPath = @"C:\Users\Project\Test\file.8xp";
			string relativePath = @"..\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			target.FileRelativePath = fullPath;
			string actual = target.FileRelativePath;
			Assert.AreEqual(relativePath, actual);
		}

		[TestMethod]
		public void ProjectFile_Relative_ToString()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string fullPath = @"C:\Users\Project\Test\file.8xp";
			string relativePath = @"..\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			target.FileRelativePath = fullPath;
			string actual = target.ToString();
			Assert.AreEqual(relativePath, actual);
		}

		[TestMethod]
		public void ProjectFile_Absolute_ToString()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string fullPath = @"C:\Users\Project\Test\file.8xp";
			ProjectFile target = new ProjectFile(project, fullPath);
			target.FileFullPath = fullPath;
			string actual = target.ToString();
			Assert.AreEqual(fullPath, actual);
		}
	}
}
