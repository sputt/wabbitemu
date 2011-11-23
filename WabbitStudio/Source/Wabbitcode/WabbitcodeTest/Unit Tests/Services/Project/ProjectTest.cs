using Revsoft.Wabbitcode.Services.Project;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using System.Collections.Generic;
using System.Xml;
using System.IO;
using Revsoft.Wabbitcode.Services;
using WabbitcodeTest.Helpers;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for ProjectTest and is intended
	///to contain all ProjectTest Unit Tests
	///</summary>
	[TestClass()]
	public class ProjectTest
	{
		private TestContext testContextInstance;

		/// <summary>
		///Gets or sets the test context which provides
		///information about and functionality for the current test run.
		///</summary>
		public TestContext TestContext
		{
			get
			{
				return testContextInstance;
			}
			set
			{
				testContextInstance = value;
			}
		}

		#region Additional test attributes
		// 
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		//[ClassInitialize()]
		//public static void MyClassInitialize(TestContext testContext)
		//{
		//}
		//
		//Use ClassCleanup to run code after all tests in a class have run
		//[ClassCleanup()]
		//public static void MyClassCleanup()
		//{
		//}
		//
		//Use TestInitialize to run code before running each test
		//[TestInitialize()]
		//public void MyTestInitialize()
		//{
		//}
		//
		//Use TestCleanup to run code after each test has run
		//[TestCleanup()]
		//public void MyTestCleanup()
		//{
		//}
		//
		#endregion

		/// <summary>
		///A test for AddFile
		///</summary>
		[TestMethod()]
		public void AddFileTest()
		{
			string folderName = "Folder";

			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProjectFolder parentFolder = new ProjectFolder(target, folderName);
			string fullPath = @"C:\Users\Test\Projects\Test.8xp";
			IProjectFile expected = new ProjectFile(target, fullPath);
			IProjectFile actual;
			actual = target.AddFile(parentFolder, fullPath);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for AddFolder
		///</summary>
		[TestMethod()]
		public void AddFolderTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			string dirName = "Folder";
			string childFolderName = "Child";
			IProjectFolder parentFolder = new ProjectFolder(target, dirName);
			IProjectFolder expected = new ProjectFolder(target, childFolderName);
			IProjectFolder actual;
			actual = target.AddFolder(childFolderName, parentFolder);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for BuildXMLFile
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void BuildXMLFileTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			Stream stream = Helper.GetResource("TestProject1.wcodeproj");
			Project project = new Project(stream, projectFile);

			byte[] buffer = new byte[16384];
			Stream memStream = new MemoryStream(buffer);
			project.SaveProjectFile(memStream);
			Project actual = new Project(new MemoryStream(buffer), projectFile);

			Assert.AreEqual(Path.GetDirectoryName(projectFile), actual.ProjectDirectory);
			Assert.AreEqual(projectFile, actual.ProjectFile);
			Assert.AreEqual("TestProject1", actual.ProjectName);
			Assert.AreEqual(false, actual.IsInternal);
			Assert.AreEqual(9, actual.IncludeDirs.Count);
			Assert.AreEqual(1, actual.MainFolder.Folders.Count);
		}

		/// <summary>
		///A test for ContainsFile
		///</summary>
		[TestMethod()]
		public void ContainsFileTest_True()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			string fullPath = @"C:\Users\Test\Projects\Test.8xp";
			bool expected = true;
			target.AddFile(target.MainFolder, fullPath);
			bool actual = target.ContainsFile(fullPath);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void ContainsFileTest_False()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			string fullPath = @"C:\Users\Test\Projects\Test.8xp";
			bool expected = false;
			bool actual = target.ContainsFile(fullPath);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for CreateNewProject
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void CreateNewProjectTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProject actual = new Project(projectFile, projectName);
			Assert.AreEqual(target.ProjectFile, actual.ProjectFile);
			Assert.AreEqual(target.ProjectName, actual.ProjectName);
		}

		/// <summary>
		///A test for DeleteFile
		///</summary>
		[TestMethod()]
		public void DeleteFileTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string fullPath = @"C:\Users\Test\file.8xp";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProjectFile file = target.AddFile(target.MainFolder, fullPath);
			target.DeleteFile(fullPath);

			Assert.IsFalse(target.MainFolder.Files.Contains(file));
		}

		/// <summary>
		///A test for DeleteFile
		///</summary>
		[TestMethod()]
		public void DeleteFileTest1()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string fullPath = @"C:\Users\Test\file.8xp";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProjectFile file = target.AddFile(target.MainFolder, fullPath);
			target.DeleteFile(file);

			Assert.IsFalse(target.MainFolder.Files.Contains(file));
		}

		/// <summary>
		///A test for DeleteFolder
		///</summary>
		[TestMethod()]
		public void DeleteFolderTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			string folderName = "Folder";
			string parentName = "Parent";
			IProject target = new Project(projectFile, projectName);

			IProjectFolder parentDir = new ProjectFolder(target, parentName);
			IProjectFolder dir = target.AddFolder(folderName, parentDir);
			target.DeleteFolder(parentDir, dir);

			Assert.IsFalse(target.MainFolder.Folders.Contains(dir));
		}

		/// <summary>
		///A test for FindFile
		///</summary>
		[TestMethod()]
		public void FindFileTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string fullPath = @"C:\Users\Test\file.8xp";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProjectFile expected = target.AddFile(target.MainFolder, fullPath);
			IProjectFile actual = target.FindFile(fullPath);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void FindFileTest_NotFound()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string fullPath = @"C:\Users\Test\file.8xp";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProjectFile expected = null;
			IProjectFile actual = target.FindFile(fullPath);
			Assert.AreEqual(expected, actual);
		}
		
		[TestMethod()]
		public void FindFileTest_SameFile()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string fullPath = @"C:\Users\Test\file.8xp";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProjectFile expected = target.AddFile(target.MainFolder, fullPath);
			target.FindFile(fullPath);
			IProjectFile actual = target.FindFile(fullPath);
			Assert.AreEqual(expected, actual);
		}

		#region GetOutputType
		
		/// <summary>
		///A test for GetOutputType
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void GetOutputTypeTest_Bin()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			Project.AssemblerOutputType expected = new Project.AssemblerOutputType(".bin");
			Project.AssemblerOutputType actual;
			actual = target.GetOutputType();
			Assert.AreEqual(expected, actual);
		}

		#endregion


		/// <summary>
		///A test for GetProjectFiles
		///</summary>
		[TestMethod()]
		public void GetProjectFilesTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			string filePath1 = @"C:\Users\Test\file1.8xp";
			IProject target = new Project(projectFile, projectName);

			IList<IProjectFile> expected = new List<IProjectFile>();
			IProjectFile file1 = target.AddFile(target.MainFolder, filePath1);
			expected.Add(file1);
			IList<IProjectFile> actual;
			actual = target.GetProjectFiles();
			Assert.AreEqual(expected.Count, actual.Count);
			for (int i = 0; i < expected.Count; i++)
			{
				Assert.AreEqual(expected[i], actual[i]);
			}
		}

		/// <summary>
		///A test for OpenProject
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void OpenProjectTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			
			Stream stream = Helper.GetResource("TestProject1.wcodeproj");
			Project actual = new Project(stream, projectFile);
			Assert.AreEqual(Path.GetDirectoryName(projectFile), actual.ProjectDirectory);
			Assert.AreEqual(projectFile, actual.ProjectFile);
			Assert.AreEqual("TestProject1", actual.ProjectName);
			Assert.AreEqual(false, actual.IsInternal);
			Assert.AreEqual(9, actual.IncludeDirs.Count);
			Assert.AreEqual(1, actual.MainFolder.Folders.Count);
		}

		/// <summary>
		///A test for BuildSystem
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void BuildSystemTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IBuildSystem expected = new BuildSystem(target, true);
			IBuildSystem actual = target.BuildSystem;
			Assert.AreEqual(expected.BuildConfigs.Count, actual.BuildConfigs.Count);
			Assert.AreEqual(expected.MainFile, actual.MainFile);
			Assert.AreEqual(expected.Project.ProjectFile, actual.Project.ProjectFile);
		}

		/// <summary>
		///A test for IsInternal
		///</summary>
		[TestMethod()]
		public void IsInternalTest()
		{
			IProject target = new Project(true);

			bool expected = true;
			Assert.AreEqual(expected, target.IsInternal);
		}

		/// <summary>
		///A test for MainFolder
		///</summary>
		[TestMethod()]
		public void MainFolderTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			IProjectFolder expected = new ProjectFolder(target, projectName);
			IProjectFolder actual;
			actual = target.MainFolder;
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for ProjectDirectory
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectDirectoryTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			Assert.AreEqual(System.IO.Path.GetDirectoryName(projectFile), target.ProjectDirectory);
		}

		/// <summary>
		///A test for ProjectFile
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectFileTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			string expected = projectFile;
			string actual = target.ProjectFile;
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for ProjectName
		///</summary>
		[TestMethod()]
		public void ProjectNameTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject target = new Project(projectFile, projectName);

			string expected = projectName;
			target.ProjectName = expected;
			string actual = target.ProjectName;
			Assert.AreEqual(expected, actual);
		}
	}
}
