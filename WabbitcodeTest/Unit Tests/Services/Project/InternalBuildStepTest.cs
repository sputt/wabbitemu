using Revsoft.Wabbitcode.Services.Project;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Utilities;
using Moq;
using Revsoft.Wabbitcode.Services;
using System.IO;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for InternalBuildStepTest and is intended
	///to contain all InternalBuildStepTest Unit Tests
	///</summary>
	[TestClass()]
	public class InternalBuildStepTest
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
		///A test for InternalBuildStep Constructor
		///</summary>
		[TestMethod()]
		public void InternalBuildStepConstructorTest()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			Assert.AreEqual(inputFile, target.InputFile);
			Assert.AreEqual(outputFile, target.OutputFile);
			Assert.AreEqual(type, target.StepType);
			Assert.AreEqual(number, target.StepNumber);
		}

		[TestMethod()]
		public void InternalBuildStep_GetHashCode()
		{
			Mock<IProject> mockProject = new Mock<IProject>(MockBehavior.Strict);

			IProject project = mockProject.Object;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);
			InternalBuildStep target2 = new InternalBuildStep(project, number, type, inputFile, outputFile);

			bool expected = true;
			bool actual = target.GetHashCode() == target2.GetHashCode();
			Assert.AreEqual(expected, actual);

			mockProject.Verify();
		}

		[TestMethod()]
		public void InternalBuildStep_GetHashCode_NullProject()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);
			InternalBuildStep target2 = new InternalBuildStep(project, number, type, inputFile, outputFile);

			bool expected = true;
			bool actual = target.GetHashCode() == target2.GetHashCode();
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void InternalBuildStep_Equals()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);
			InternalBuildStep target2 = new InternalBuildStep(project, number, type, inputFile, outputFile);

			bool expected = true;
			bool actual = target.Equals(target2);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void InternalBuildStep_Equals_NotEqual()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);
			string target2 = "test";

			bool expected = false;
			bool actual = target.Equals(target2);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void InternalBuildStep_Equals_NotEqual2()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);
			InternalBuildStep target2 = new InternalBuildStep(project, number + 1, type, inputFile, outputFile);

			bool expected = false;
			bool actual = target.Equals(target2);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Build
		///</summary>
		[TestMethod()]
		public void BuildTest_All()
		{
			Mock<IProject> mockProject = new Mock<IProject>(MockBehavior.Strict);

			IProject project = mockProject.Object;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";

			mockProject.Setup(p => p.ProjectOutputs.Add(outputFile)).Verifiable();
			mockProject.Setup(p => p.LabelOutputs.Add(Path.ChangeExtension(outputFile, ".lab"))).Verifiable();
			mockProject.Setup(p => p.ListOutputs.Add(Path.ChangeExtension(outputFile, ".lst"))).Verifiable();

			Mock<IAssemblerService> mockAssemblerService = new Mock<IAssemblerService>(MockBehavior.Strict);
			mockAssemblerService.Setup(a => a.AssembleFile(new FilePath(inputFile), new FilePath(outputFile), null,
								AssemblyFlags.Assemble | AssemblyFlags.Label | AssemblyFlags.List, null))
								.Returns(true)
								.Verifiable();
			mockProject.Setup(p => p.AssemblerService).Returns(mockAssemblerService.Object).Verifiable();

			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			bool expected = true;
			bool actual;
			actual = target.Build();
			Assert.AreEqual(expected, actual);

			mockProject.Verify();
			mockAssemblerService.Verify();
		}

		[TestMethod()]
		public void BuildTest_Assemble()
		{
			Mock<IProject> mockProject = new Mock<IProject>(MockBehavior.Strict);

			IProject project = mockProject.Object;
			int number = 0;
			StepType type = StepType.Assemble;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";

			mockProject.Setup(p => p.ProjectOutputs.Add(outputFile)).Verifiable();

			Mock<IAssemblerService> mockAssemblerService = new Mock<IAssemblerService>(MockBehavior.Strict);
			mockAssemblerService.Setup(a => a.AssembleFile(new FilePath(inputFile), new FilePath(outputFile), null,
								AssemblyFlags.Assemble, null))
								.Returns(true)
								.Verifiable();
			mockProject.Setup(p => p.AssemblerService).Returns(mockAssemblerService.Object).Verifiable();

			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			bool expected = true;
			bool actual;
			actual = target.Build();
			Assert.AreEqual(expected, actual);

			mockProject.Verify();
			mockAssemblerService.Verify();
		}

		[TestMethod()]
		public void BuildTest_List()
		{
			Mock<IProject> mockProject = new Mock<IProject>(MockBehavior.Strict);

			IProject project = mockProject.Object;
			int number = 0;
			StepType type = StepType.Listing;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";

			mockProject.Setup(p => p.ProjectOutputs.Add(outputFile)).Verifiable();
			mockProject.Setup(p => p.ListOutputs.Add(Path.ChangeExtension(outputFile, ".lst"))).Verifiable();

			Mock<IAssemblerService> mockAssemblerService = new Mock<IAssemblerService>(MockBehavior.Strict);
			mockAssemblerService.Setup(a => a.AssembleFile(new FilePath(inputFile), new FilePath(outputFile), null,
								AssemblyFlags.Assemble | AssemblyFlags.List, null))
								.Returns(true)
								.Verifiable();
			mockProject.Setup(p => p.AssemblerService).Returns(mockAssemblerService.Object).Verifiable();

			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			bool expected = true;
			bool actual;
			actual = target.Build();
			Assert.AreEqual(expected, actual);

			mockProject.Verify();
			mockAssemblerService.Verify();
		}

		[TestMethod()]
		public void BuildTest_Symbol()
		{
			Mock<IProject> mockProject = new Mock<IProject>(MockBehavior.Strict);

			IProject project = mockProject.Object;
			int number = 0;
			StepType type = StepType.SymbolTable;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";

			mockProject.Setup(p => p.ProjectOutputs.Add(outputFile)).Verifiable();
			mockProject.Setup(p => p.LabelOutputs.Add(Path.ChangeExtension(outputFile, ".lab"))).Verifiable();

			Mock<IAssemblerService> mockAssemblerService = new Mock<IAssemblerService>(MockBehavior.Strict);
			mockAssemblerService.Setup(a => a.AssembleFile(new FilePath(inputFile), new FilePath(outputFile), null,
								AssemblyFlags.Assemble | AssemblyFlags.Label, null))
								.Returns(true)
								.Verifiable();
			mockProject.Setup(p => p.AssemblerService).Returns(mockAssemblerService.Object).Verifiable();

			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			bool expected = true;
			bool actual;
			actual = target.Build();
			Assert.AreEqual(expected, actual);

			mockProject.Verify();
			mockAssemblerService.Verify();
		}

		/// <summary>
		///A test for Clone
		///</summary>
		[TestMethod()]
		public void CloneTest()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);
			
			object expected = target;
			object actual;
			actual = target.Clone();

			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for ToString
		///</summary>
		[TestMethod()]
		public void ToStringTest()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			string expected = "Assemble and list " + System.IO.Path.GetFileName(inputFile);
			string actual;
			actual = target.ToString();

			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Description
		///</summary>
		[TestMethod()]
		public void DescriptionTest_All()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			string expected = "Assemble and list " + System.IO.Path.GetFileName(inputFile);
			string actual;
			actual = target.ToString();

			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void DescriptionTest_Assemble()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.Assemble;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			string expected = "Assemble " + System.IO.Path.GetFileName(inputFile);
			string actual;
			actual = target.ToString();

			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void DescriptionTest_List()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.Listing;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			string expected = "List " + System.IO.Path.GetFileName(inputFile);
			string actual;
			actual = target.ToString();

			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void DescriptionTest_SymbolTable()
		{
			IProject project = null;
			int number = 0;
			StepType type = StepType.SymbolTable;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			string expected = "Symbolize " + System.IO.Path.GetFileName(inputFile);
			string actual;
			actual = target.ToString();

			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for InputFileRelative
		///</summary>
		[TestMethod()]
		public void InputFileRelativeTest()
		{
			Mock<IProject> mockProject = new Mock<IProject>(MockBehavior.Strict);
			mockProject.Setup(p => p.ProjectDirectory).Returns(@"C:\Users\Test\");

			IProject project = mockProject.Object;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			string expected = @"..\input.asm";
			string actual = target.InputFileRelative;
			Assert.AreEqual(expected, actual);

			mockProject.Verify();
		}

		/// <summary>
		///A test for OutputFile
		///</summary>
		[TestMethod()]
		public void OutputFileRelativeTest()
		{
			Mock<IProject> mockProject = new Mock<IProject>(MockBehavior.Strict);
			mockProject.Setup(p => p.ProjectDirectory).Returns(@"C:\Users\Test\");

			IProject project = mockProject.Object;
			int number = 0;
			StepType type = StepType.All;
			string inputFile = @"C:\Users\Test\Project\input.asm";
			string outputFile = @"C:\Users\Test\Project\New Folder\output.bin";
			InternalBuildStep target = new InternalBuildStep(project, number, type, inputFile, outputFile);

			string expected = @"..\..\output.bin";
			string actual = target.OutputFileRelative;
			Assert.AreEqual(expected, actual);

			mockProject.Verify();
		}
	}
}
