using Microsoft.VisualStudio.TestTools.UnitTesting;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Project.Interface;

namespace WabbitcodeTest
{
	/// <summary>
	///This is a test class for ProjectFolderTest and is intended
	///to contain all ProjectFolderTest Unit Tests
	///</summary>
	[TestClass()]
	public class ProjectFolderTest
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
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			string filePath = @"C:\Folder";
			ProjectFolder target = new ProjectFolder(project, folderName);
			IProjectFile file = new ProjectFile(project, filePath);
			target.AddFile(file);

			Assert.AreEqual(target.Files.Count, 1);
			Assert.IsTrue(project.NeedsSave);
			Assert.AreEqual(file.Folder.Name, target.Name);
		}

		/// <summary>
		///A test for AddFolder
		///</summary>
		[TestMethod()]
		public void AddFolderTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			string folderName2 = "Folder 2";
			ProjectFolder target = new ProjectFolder(project, folderName);
			IProjectFolder subFolder = new ProjectFolder(project, folderName2);
			target.AddFolder(subFolder);

			Assert.AreEqual(target.Folders.Count, 1);
			Assert.IsTrue(project.NeedsSave);
			Assert.AreEqual(subFolder.Name, target.Folders[0].Name);
			Assert.AreEqual(subFolder.ParentFolder.Name, target.Name);
		}

		/// <summary>
		///A test for FindFile
		///</summary>
		[TestMethod()]
		public void FindFileTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			ProjectFolder target = new ProjectFolder(project, folderName);
			string file = @"C:\Folder\File.8xp";
			IProjectFile expected = new ProjectFile(project, file);
			target.AddFile(expected);
			IProjectFile actual = target.FindFile(System.IO.Path.GetFileName(file));
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void FindFileTest_NoMatch()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			ProjectFolder target = new ProjectFolder(project, folderName);
			string file = @"C:\Folder\File.8xp";
			IProjectFile expected = null;
			IProjectFile actual = target.FindFile(System.IO.Path.GetFileName(file));
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for FindFolder
		///</summary>
		[TestMethod()]
		public void FindFolderTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			string folderName2 = "Folder 2";
			ProjectFolder target = new ProjectFolder(project, folderName);
			IProjectFolder expected = new ProjectFolder(project, folderName2); // TODO: Initialize to an appropriate value
			IProjectFolder actual;
			target.AddFolder(expected);
			actual = target.FindFolder(folderName2);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for FindFolder
		///</summary>
		[TestMethod()]
		public void FindFolderTest_NoMatch()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			string folderName2 = "Folder 2";
			ProjectFolder target = new ProjectFolder(project, folderName);
			IProjectFolder expected = null;
			IProjectFolder actual = target.FindFolder(folderName2);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Remove
		///</summary>
		[TestMethod()]
		public void RemoveTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			string parentName = "Parent Folder";
			ProjectFolder target = new ProjectFolder(project, folderName);
			ProjectFolder parentFolder = new ProjectFolder(project, parentName);
			parentFolder.AddFolder(target);
			target.Remove();

			Assert.AreEqual(parentFolder.Folders.Count, 0);
			Assert.AreEqual(true, project.NeedsSave);
		}

		/// <summary>
		///A test for ToString
		///</summary>
		[TestMethod()]
		public void ProjectFolder_ToStringTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			ProjectFolder target = new ProjectFolder(project, folderName);
			string actual = target.ToString();
			Assert.AreEqual(folderName, actual);
		}

		/// <summary>
		///A test for Name
		///</summary>
		[TestMethod()]
		public void NameTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			ProjectFolder target = new ProjectFolder(project, folderName);
			string actual;
			target.Name = folderName;
			actual = target.Name;
			Assert.AreEqual(folderName, actual);
		}

		/// <summary>
		///A test for Parent
		///</summary>
		[TestMethod()]
		public void ParentTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);

			string folderName = "Folder";
			ProjectFolder target = new ProjectFolder(project, folderName);
			IProject actual = target.Parent;
			Assert.AreEqual(project, actual);
		}
	}
}
