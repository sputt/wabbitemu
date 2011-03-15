using WabbitC.TokenPasses;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using WabbitC;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace WabbitC_Tests
{
    
    
    /// <summary>
    ///This is a test class for BracerTest and is intended
    ///to contain all BracerTest Unit Tests
    ///</summary>
	[TestClass()]
	public class BracerTest
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
		///A test for Run
		///</summary>
		[TestMethod()]
		public void RunTest()
		{
			string fileContents = Compiler.TryOpenFile(@"..\..\..\WabbitC Tests\bracer_test.c");
			Assert.IsTrue(!string.IsNullOrEmpty(fileContents));
			List<Token> tokenList = Tokenizer.Tokenize(fileContents);

			fileContents = Compiler.TryOpenFile(@"..\..\..\WabbitC Tests\bracer_result.c");
			Assert.IsTrue(!string.IsNullOrEmpty(fileContents));
			List<Token> expected = Tokenizer.Tokenize(fileContents);
			
			Bracer target = new Bracer();
			List<Token> actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual(actual));
		}
	}
}
