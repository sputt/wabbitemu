using Revsoft.Wabbitcode.Services.Project;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Interface;
using System.Xml;
using System.Collections.Generic;

namespace WabbitcodeTest
{
    
    
    /// <summary>
    ///This is a test class for BuildSystemTest and is intended
    ///to contain all BuildSystemTest Unit Tests
    ///</summary>
	[TestClass()]
	public class BuildSystemTest
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
		///A test for BuildSystem Constructor
		///</summary>
		[TestMethod()]
		public void BuildSystemConstructorTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults);
			Assert.Inconclusive("TODO: Implement code to verify target");
		}

		/// <summary>
		///A test for Build
		///</summary>
		[TestMethod()]
		public void BuildTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults); // TODO: Initialize to an appropriate value
			target.Build();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for CreateXML
		///</summary>
		[TestMethod()]
		public void CreateXMLTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults); // TODO: Initialize to an appropriate value
			XmlTextWriter writer = null; // TODO: Initialize to an appropriate value
			target.CreateXML(writer);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ReadXML
		///</summary>
		[TestMethod()]
		public void ReadXMLTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults); // TODO: Initialize to an appropriate value
			XmlTextReader reader = null; // TODO: Initialize to an appropriate value
			target.ReadXML(reader);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for BuildConfigs
		///</summary>
		[TestMethod()]
		public void BuildConfigsTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults); // TODO: Initialize to an appropriate value
			IList<IBuildConfig> actual;
			actual = target.BuildConfigs;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for CurrentConfig
		///</summary>
		[TestMethod()]
		public void CurrentConfigTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults); // TODO: Initialize to an appropriate value
			IBuildConfig expected = null; // TODO: Initialize to an appropriate value
			IBuildConfig actual;
			target.CurrentConfig = expected;
			actual = target.CurrentConfig;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for MainFile
		///</summary>
		[TestMethod()]
		public void MainFileTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.MainFile;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for MainOutput
		///</summary>
		[TestMethod()]
		public void MainOutputTest()
		{
			IProject project = null; // TODO: Initialize to an appropriate value
			bool CreateDefaults = false; // TODO: Initialize to an appropriate value
			BuildSystem target = new BuildSystem(project, CreateDefaults); // TODO: Initialize to an appropriate value
			string actual;
			actual = target.MainOutput;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for Project
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectTest()
		{
			PrivateObject param0 = null; // TODO: Initialize to an appropriate value
			BuildSystem_Accessor target = new BuildSystem_Accessor(param0); // TODO: Initialize to an appropriate value
			IProject expected = null; // TODO: Initialize to an appropriate value
			IProject actual;
			target.Project = expected;
			actual = target.Project;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
