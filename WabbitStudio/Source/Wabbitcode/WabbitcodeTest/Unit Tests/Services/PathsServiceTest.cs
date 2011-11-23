using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for PathsServiceTest and is intended
	///to contain all PathsServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class PathsServiceTest
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
		///A test for PathsService Constructor
		///</summary>
		[TestMethod()]
		public void PathsServiceConstructorTest()
		{
			PathsService target = new PathsService();
			Assert.Inconclusive("TODO: Implement code to verify target");
		}

		/// <summary>
		///A test for DestroyService
		///</summary>
		[TestMethod()]
		public void DestroyServiceTest()
		{
			PathsService target = new PathsService(); // TODO: Initialize to an appropriate value
			target.DestroyService();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest()
		{
			PathsService target = new PathsService(); // TODO: Initialize to an appropriate value
			object[] objects = null; // TODO: Initialize to an appropriate value
			target.InitService(objects);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ApplicationAppData
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ApplicationAppDataTest()
		{
			PathsService_Accessor target = new PathsService_Accessor(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.ApplicationAppData;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for DockConfig
		///</summary>
		[TestMethod()]
		public void DockConfigTest()
		{
			PathsService target = new PathsService(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.DockConfig;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for IncludeDirectory
		///</summary>
		[TestMethod()]
		public void IncludeDirectoryTest()
		{
			PathsService target = new PathsService(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.IncludeDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ProjectDirectory
		///</summary>
		[TestMethod()]
		public void ProjectDirectoryTest()
		{
			PathsService target = new PathsService(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.ProjectDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for TemplatesConfig
		///</summary>
		[TestMethod()]
		public void TemplatesConfigTest()
		{
			PathsService target = new PathsService();
			string actual;
			actual = target.TemplatesConfig;
			
		}

		/// <summary>
		///A test for TemplatesDirectory
		///</summary>
		[TestMethod()]
		public void TemplatesDirectoryTest()
		{
			PathsService target = new PathsService(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.TemplatesDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for WabbitcodeDirectory
		///</summary>
		[TestMethod()]
		public void WabbitcodeDirectoryTest()
		{
			PathsService target = new PathsService(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.WabbitcodeDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
