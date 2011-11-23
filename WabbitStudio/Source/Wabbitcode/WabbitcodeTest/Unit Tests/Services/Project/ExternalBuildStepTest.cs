using Revsoft.Wabbitcode.Services.Project;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Utilities;
using Revsoft.Wabbitcode.Interface;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for ExternalBuildStepTest and is intended
	///to contain all ExternalBuildStepTest Unit Tests
	///</summary>
	[TestClass()]
	public class ExternalBuildStepTest
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
		///A test for ExternalBuildStep Constructor
		///</summary>
		[TestMethod()]
		public void ExternalBuildStepConstructorTest()
		{
			int number = 0;
			string program = "test";
			string arguments = "arg";
			ExternalBuildStep target = new ExternalBuildStep(null, number, program, arguments);
			Assert.AreEqual(0, target.StepNumber);
			Assert.AreEqual(program, target.InputFile);
			Assert.AreEqual(arguments, target.Arguments);
		}

		/// <summary>
		///A test for ExternalBuildStep Constructor
		///</summary>
		[TestMethod()]
		public void ExternalBuildStepConstructorTest1()
		{
			int number = 0;
			string commandLine = "test arg";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);
			Assert.AreEqual(0, target.StepNumber);
			Assert.AreEqual("test", target.InputFile);
			Assert.AreEqual("arg", target.Arguments);
		}

		/// <summary>
		///A test for ExternalBuildStep Constructor
		///</summary>
		[TestMethod()]
		public void ExternalBuildStepConstructorTest2()
		{
			int number = 0;
			string commandLine = "test";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);
			Assert.AreEqual(0, target.StepNumber);
			Assert.AreEqual("test", target.InputFile);
			Assert.AreEqual(String.Empty, target.Arguments);
		}

		[TestMethod()]
		public void ExternalBuildStep_Equals()
		{
			int number = 0;
			string program = "test";
			string arguments = "arg";
			ExternalBuildStep target = new ExternalBuildStep(null, number, program, arguments);
			ExternalBuildStep target2 = new ExternalBuildStep(null, number, program, arguments);
			
			bool actual = target.Equals(target2);
			Assert.AreEqual(true, actual);
		}

		[TestMethod()]
		public void ExternalBuildStep_Equals_NotEqual()
		{
			int number = 0;
			string program = "test";
			string arguments = "arg";
			ExternalBuildStep target = new ExternalBuildStep(null, number, program, arguments);
			string target2 = "test";

			bool actual = target.Equals(target2);
			Assert.AreEqual(false, actual);
		}

		[TestMethod()]
		public void ExternalBuildStep_Equals_NotEqual2()
		{
			int number = 0;
			string program = "test";
			string arguments = "arg";
			ExternalBuildStep target = new ExternalBuildStep(null, number, program, arguments);
			ExternalBuildStep target2 = new ExternalBuildStep(null, number + 1, program + "1", arguments + "1");

			bool actual = target.Equals(target2);
			Assert.AreEqual(false, actual);
		}

		[TestMethod()]
		public void ExternalBuildStep_GetHashCode()
		{
			int number = 0;
			string program = "test";
			string arguments = "arg";
			ExternalBuildStep target = new ExternalBuildStep(null, number, program, arguments);
			ExternalBuildStep target2 = new ExternalBuildStep(null, number, program, arguments);

			bool actual = target.GetHashCode() == target2.GetHashCode();
			Assert.AreEqual(true, actual);
		}

		/// <summary>
		///A test for Clone
		///</summary>
		[TestMethod()]
		public void CloneTest()
		{
			int number = 0;
			string commandLine = "test args";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);

			IBuildStep expected = target;
			IBuildStep actual = (IBuildStep) target.Clone();
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for ToString
		///</summary>
		[TestMethod()]
		public void ToStringTest()
		{
			int number = 0;
			string commandLine = "test args";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);

			string expected = "Run test";
			string actual = target.ToString();
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Arguments
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ArgumentsTest()
		{
			int stepNumber = 0;
			string commandLine = "test arg1 arg2 arg3";
			ExternalBuildStep target = new ExternalBuildStep(null, stepNumber, commandLine);
			
			string expected = "arg1 arg2 arg3";
			string actual = target.Arguments;
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Callback
		///</summary>
		[TestMethod()]
		public void CallbackTest()
		{
			int number = 0;
			string commandLine = "test args";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);

			bool pass = false;
			Action<string> expected = new Action<string>((string test) => pass = true);
			target.Callback = expected;
			target.Callback("output");
			Assert.AreEqual(true, pass);
		}

		/// <summary>
		///A test for Description
		///</summary>
		[TestMethod()]
		public void DescriptionTest()
		{
			int number = 0;
			string commandLine = "test args";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);

			string expected = "Run test";
			string actual = target.Description;
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for InputFile
		///</summary>
		[TestMethod()]
		public void InputFileTest()
		{
			int number = 0;
			string commandLine = @"C:\Users\Test\test args";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);
			
			FilePath expected = new FilePath(@"C:\Users\Test\test");
			target.InputFile = expected;
			FilePath actual = target.InputFile;
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Project
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectTest()
		{
			string projectFile = @"C:\Users\Test\file.wcodeproj";
			string projectName = "Project";
			IProject project = new Project(projectFile, projectName);
			int stepNumber = 0;
			string commandLine = "test arg";
			ExternalBuildStep target = new ExternalBuildStep(project, stepNumber, commandLine);

			IProject expected = project;
			IProject actual = target.Project;
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for StepNumber
		///</summary>
		[TestMethod()]
		public void StepNumberTest()
		{
			int number = int.MaxValue;
			string commandLine = "test args";
			ExternalBuildStep target = new ExternalBuildStep(null, number, commandLine);
			
			int expected = number;
			target.StepNumber = expected;
			int actual = target.StepNumber;
			Assert.AreEqual(expected, actual);
		}
	}
}
