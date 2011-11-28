using Microsoft.VisualStudio.TestTools.UnitTesting;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Project.Interface;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for BuildConfigTest and is intended
	///to contain all BuildConfigTest Unit Tests
	///</summary>
	[TestClass()]
	public class BuildConfigTest
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
		///A test for Build
		///</summary>
		[TestMethod()]
		public void BuildTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			string name = string.Empty; // TODO: Initialize to an appropriate value
			BuildConfig target = new BuildConfig(project, name); // TODO: Initialize to an appropriate value
			target.Build();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for Clone
		///</summary>
		[TestMethod()]
		public void CloneTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);
			target.Steps.Add(new InternalBuildStep(project, 1, StepType.All, null, null));
			target.Steps.Add(new InternalBuildStep(project, 0, StepType.Listing, null, null));

			IBuildConfig actual = (IBuildConfig) target.Clone();

			Assert.AreEqual(target, actual);
		}

		/// <summary>
		///A test for Equals
		///</summary>
		[TestMethod()]
		public void EqualsTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);
			BuildConfig target2 = new BuildConfig(project, name);

			bool expected = true;
			bool actual = target.Equals(target2);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Equals
		///</summary>
		[TestMethod()]
		public void EqualsTest_NotEqual()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);
			string target2 = "test fail string";

			bool expected = false;
			bool actual = target.Equals(target2);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for GetHashCode
		///</summary>
		[TestMethod()]
		public void BuildConfig_GetHashCodeTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);
			BuildConfig target2 = new BuildConfig(project, name);

			int expected = target.GetHashCode();
			int actual = target2.GetHashCode();
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void BuildConfig_GetHashCodeTest2()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			string name2 = "Debug Config 2";
			BuildConfig target = new BuildConfig(project, name);
			BuildConfig target2 = new BuildConfig(project, name2);

			int expected = target.GetHashCode();
			int actual = target2.GetHashCode();
			Assert.AreNotEqual(expected, actual);
		}

		/// <summary>
		///A test for SortSteps
		///</summary>
		[TestMethod()]
		public void SortStepsTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);
			target.Steps.Add(new InternalBuildStep(project, 1, StepType.All, null, null));
			target.Steps.Add(new InternalBuildStep(project, 0, StepType.Listing, null, null));
			target.Steps.Add(new ExternalBuildStep(project, 2, "lolprog -a -v"));

			target.SortSteps();
			int prevNumber = -1;
			for (int i = 0; i < target.Steps.Count; i++)
			{
				Assert.IsTrue(prevNumber < target.Steps[i].StepNumber);
				prevNumber = target.Steps[i].StepNumber;
			}
		}

		/// <summary>
		///A test for ToString
		///</summary>
		[TestMethod()]
		public void BuildConfig_ToStringTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);

			string expected = name;
			string actual = target.ToString();
			Assert.AreEqual(expected, actual);
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
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);

			string expected = name;
			string actual = target.Name;
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Project
		///</summary>
		[TestMethod()]
		public void ProjectTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			string name = "Debug Config";
			BuildConfig target = new BuildConfig(project, name);

			Assert.AreEqual(projectFile, target.Project.ProjectFile);
		}
	}
}
