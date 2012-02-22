using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for AssemblerTest and is intended
	///to contain all AssemblerTest Unit Tests
	///</summary>
	[TestClass()]
	public class AssemblerTest
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
		///A test for AddDefine
		///</summary>
		[TestMethod()]
		public void AddDefineTest()
		{
			Assembler target = new Assembler(); // TODO: Initialize to an appropriate value
			string name = string.Empty; // TODO: Initialize to an appropriate value
			string value = string.Empty; // TODO: Initialize to an appropriate value
			target.AddDefine(name, value);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for AddIncludeDir
		///</summary>
		[TestMethod()]
		public void AddIncludeDirTest()
		{
			Assembler target = new Assembler(); // TODO: Initialize to an appropriate value
			string path = string.Empty; // TODO: Initialize to an appropriate value
			target.AddIncludeDir(path);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for Assemble
		///</summary>
		[TestMethod()]
		public void AssembleTest()
		{
			Assembler target = new Assembler(); // TODO: Initialize to an appropriate value
			target.Assemble();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ClearDefines
		///</summary>
		[TestMethod()]
		public void ClearDefinesTest()
		{
			Assembler target = new Assembler(); // TODO: Initialize to an appropriate value
			target.ClearDefines();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ClearIncludeDirs
		///</summary>
		[TestMethod()]
		public void ClearIncludeDirsTest()
		{
			Assembler target = new Assembler(); // TODO: Initialize to an appropriate value
			target.ClearIncludeDirs();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for SetInputFile
		///</summary>
		[TestMethod()]
		public void SetInputFileTest()
		{
			Assembler target = new Assembler(); // TODO: Initialize to an appropriate value
			string file = string.Empty; // TODO: Initialize to an appropriate value
			target.SetInputFile(file);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for SetOutputFile
		///</summary>
		[TestMethod()]
		public void SetOutputFileTest()
		{
			Assembler target = new Assembler(); // TODO: Initialize to an appropriate value
			string file = string.Empty; // TODO: Initialize to an appropriate value
			target.SetOutputFile(file);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}
	}
}
