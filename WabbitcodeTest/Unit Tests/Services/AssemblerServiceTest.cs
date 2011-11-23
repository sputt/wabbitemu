using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Utilities;
using Revsoft.Wabbitcode.Services.Parser;
using System.Collections.Generic;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for AssemblerServiceTest and is intended
	///to contain all AssemblerServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class AssemblerServiceTest
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
		///A test for AssemblerService Constructor
		///</summary>
		[TestMethod()]
		public void AssemblerServiceConstructorTest()
		{
			AssemblerService target = new AssemblerService();
			Assert.Inconclusive("TODO: Implement code to verify target");
		}

		/// <summary>
		///A test for AssembleFile
		///</summary>
		[TestMethod()]
		public void AssembleFileTest()
		{
			AssemblerService target = new AssemblerService(); // TODO: Initialize to an appropriate value
			FilePath inputPath = null; // TODO: Initialize to an appropriate value
			FilePath outputPath = null; // TODO: Initialize to an appropriate value
			IList<IDefine> defines = null; // TODO: Initialize to an appropriate value
			AssemblyFlags flags = new AssemblyFlags(); // TODO: Initialize to an appropriate value
			Action<string> callback = null; // TODO: Initialize to an appropriate value
			bool expected = false; // TODO: Initialize to an appropriate value
			bool actual;
			actual = target.AssembleFile(inputPath, outputPath, defines, flags, callback);
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for Assembler
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void AssemblerTest()
		{
			AssemblerService_Accessor target = new AssemblerService_Accessor(); // TODO: Initialize to an appropriate value
			Assembler expected = null; // TODO: Initialize to an appropriate value
			Assembler actual;
			target.Assembler = expected;
			actual = target.Assembler;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
