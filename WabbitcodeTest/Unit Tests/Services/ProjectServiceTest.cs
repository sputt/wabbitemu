using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Interface;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Interface.Services;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for ProjectServiceTest and is intended
	///to contain all ProjectServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class ProjectServiceTest
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
		///A test for ProjectService Constructor
		///</summary>
		[TestMethod()]
		public void ProjectServiceConstructorTest()
		{
			ProjectService target = new ProjectService();
			Assert.Inconclusive("TODO: Implement code to verify target");
		}

		/// <summary>
		///A test for DestroyService
		///</summary>
		[TestMethod()]
		public void DestroyServiceTest()
		{
			ProjectService target = new ProjectService(); // TODO: Initialize to an appropriate value
			target.DestroyService();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest()
		{
			ProjectService target = new ProjectService(); // TODO: Initialize to an appropriate value
			object[] objects = null; // TODO: Initialize to an appropriate value
			target.InitService(objects);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for OpenProject
		///</summary>
		[TestMethod()]
		public void OpenProjectTest()
		{
			ProjectService target = new ProjectService(); // TODO: Initialize to an appropriate value
			string fileName = string.Empty; // TODO: Initialize to an appropriate value
			target.OpenProject(fileName);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for CurrentProject
		///</summary>
		[TestMethod()]
		public void CurrentProjectTest()
		{
			ProjectService target = new ProjectService(); // TODO: Initialize to an appropriate value
			IProject expected = null; // TODO: Initialize to an appropriate value
			IProject actual;
			target.CurrentProject = expected;
			actual = target.CurrentProject;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for OpenProjects
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void OpenProjectsTest()
		{
			ProjectService_Accessor target = new ProjectService_Accessor(); // TODO: Initialize to an appropriate value
			List<IProject> expected = null; // TODO: Initialize to an appropriate value
			List<IProject> actual;
			target.OpenProjects = expected;
			actual = target.OpenProjects;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ParserService
		///</summary>
		[TestMethod()]
		public void ParserServiceTest()
		{
			ProjectService target = new ProjectService(); // TODO: Initialize to an appropriate value
			IParserService expected = null; // TODO: Initialize to an appropriate value
			IParserService actual;
			target.ParserService = expected;
			actual = target.ParserService;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
