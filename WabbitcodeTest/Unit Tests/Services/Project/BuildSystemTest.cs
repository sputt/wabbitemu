using System;
using System.Collections.Generic;
using System.Xml;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Project.Interface;
using Revsoft.Wabbitcode.Utilities;

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
            Mock<IProject> projectMock = new Mock<IProject>(MockBehavior.Strict);
			bool CreateDefaults = false;
			BuildSystem target = new BuildSystem(projectMock.Object, CreateDefaults);
            Assert.AreEqual(projectMock.Object, target.Project);
            Assert.AreEqual(0, target.BuildConfigs.Count);
		}

        [TestMethod()]
        public void BuildSystemConstructorTest_CreateDefaults()
        {
            Mock<IProject> projectMock = new Mock<IProject>(MockBehavior.Strict);
            bool CreateDefaults = true;
            BuildSystem target = new BuildSystem(projectMock.Object, CreateDefaults);
            Assert.AreEqual(projectMock.Object, target.Project);
            Assert.AreEqual(2, target.BuildConfigs.Count);
        }

		/// <summary>
		///A test for Build
		///</summary>
		[TestMethod()]
		public void BuildTest()
		{
            var projectMock = new Mock<IProject>(MockBehavior.Strict);
            var configMock = new Mock<IBuildConfig>(MockBehavior.Strict);
            configMock.Setup(c => c.Build()).Verifiable();
			bool CreateDefaults = false;
			BuildSystem target = new BuildSystem(projectMock.Object, CreateDefaults);
            target.BuildConfigs.Add(configMock.Object);
			target.Build();

            configMock.Verify();
		}

        /// <summary>
        ///A test for Build
        ///</summary>
        [TestMethod()]
        public void BuildTest_NoConfig()
        {
            var projectMock = new Mock<IProject>(MockBehavior.Strict);
            bool CreateDefaults = false;
            BuildSystem target = new BuildSystem(projectMock.Object, CreateDefaults);
            try
            {
                target.Build();
            }
            catch (Exception ex)
            {
                if (!ex.Message.Contains("config"))
                {
                    Assert.Fail("Incorrect exception");
                }
            }
            Assert.Fail("Exception was not thrown");
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
            FilePath inputPath = new FilePath(@"C:\Users\Test\Test.asm");
            var projectMock = new Mock<IProject>(MockBehavior.Strict);
            var configMock = new Mock<IBuildConfig>(MockBehavior.Strict);
            var stepMock = new Mock<IInternalBuildStep>(MockBehavior.Strict);
            var stepsList = new List<IBuildStep>();
            configMock.Setup(c => c.Steps).Returns(stepsList);
            stepMock.Setup(s => s.IsMainOutput).Returns(true).Verifiable();
            stepMock.Setup(s => s.InputFile).Returns(inputPath);
            stepsList.Add(stepMock.Object);

            bool CreateDefaults = false;
            BuildSystem target = new BuildSystem(projectMock.Object, CreateDefaults);
            target.AddConfig(configMock.Object, true);
            Assert.AreEqual(inputPath, target.MainFile);

            configMock.Verify();
            stepMock.Verify();
		}

		/// <summary>
		///A test for MainOutput
		///</summary>
		[TestMethod()]
		public void MainOutputTest()
		{
            FilePath outputPath = new FilePath(@"C:\Users\Test\Test.8xp");
            var projectMock = new Mock<IProject>(MockBehavior.Strict);
            var configMock = new Mock<IBuildConfig>(MockBehavior.Strict);
            var stepMock = new Mock<IInternalBuildStep>(MockBehavior.Strict);
            var stepsList = new List<IBuildStep>();
            configMock.Setup(c => c.Steps).Returns(stepsList);
            stepMock.Setup(s => s.IsMainOutput).Returns(true).Verifiable();
            stepMock.Setup(s => s.OutputFile).Returns(outputPath);
            stepsList.Add(stepMock.Object);

            bool CreateDefaults = false;
            BuildSystem target = new BuildSystem(projectMock.Object, CreateDefaults);
            target.AddConfig(configMock.Object, true);
            Assert.AreEqual(outputPath, target.MainOutput);

            configMock.Verify();
            stepMock.Verify();
		}
	}
}
