using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace WabbitcodeTest
{
    
    
    /// <summary>
    ///This is a test class for IPathsServiceTest and is intended
    ///to contain all IPathsServiceTest Unit Tests
    ///</summary>
	[TestClass()]
	public class IPathsServiceTest
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


		internal virtual IPathsService_Accessor CreateIPathsService_Accessor()
		{
			// TODO: Instantiate an appropriate concrete class.
			IPathsService_Accessor target = null;
			return target;
		}

		/// <summary>
		///A test for DockConfig
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void DockConfigTest()
		{
			IPathsService_Accessor target = CreateIPathsService_Accessor(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.DockConfig;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for IncludeDirectory
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void IncludeDirectoryTest()
		{
			IPathsService_Accessor target = CreateIPathsService_Accessor(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.IncludeDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ProjectDirectory
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectDirectoryTest()
		{
			IPathsService_Accessor target = CreateIPathsService_Accessor(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.ProjectDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for TemplatesConfig
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void TemplatesConfigTest()
		{
			IPathsService_Accessor target = CreateIPathsService_Accessor(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.TemplatesConfig;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for TemplatesDirectory
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void TemplatesDirectoryTest()
		{
			IPathsService_Accessor target = CreateIPathsService_Accessor(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.TemplatesDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for WabbitcodeDirectory
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void WabbitcodeDirectoryTest()
		{
			IPathsService_Accessor target = CreateIPathsService_Accessor(); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.WabbitcodeDirectory;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
