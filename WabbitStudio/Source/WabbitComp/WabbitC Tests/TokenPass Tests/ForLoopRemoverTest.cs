using WabbitC.TokenPasses;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using WabbitC;
using System.Collections.Generic;
using System.Linq;

namespace WabbitC_Tests
{
	
	
	/// <summary>
	///This is a test class for ForLoopRemoverTest and is intended
	///to contain all ForLoopRemoverTest Unit Tests
	///</summary>
	[TestClass()]
	public class ForLoopRemoverTest
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
		public void RunTest1()
		{
			ForLoopRemover target = new ForLoopRemover();

			List<Token> tokenList = Tokenizer.Tokenize("int j = 5; for (i = 0; i < 20 ; i++) {int var; if (1) { var = 10; }}");
			List<Token> expected = Tokenizer.Tokenize("int j = 5; i = 0; while (i < 20) {int var; if (1) { var = 10; } i++;}");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(String.Empty, expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(String.Empty, actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest2()
		{
			ForLoopRemover target = new ForLoopRemover();

			List<Token> tokenList = Tokenizer.Tokenize("for (i = 0; i < 20 + (4 * 4); i++);");
			List<Token> expected = Tokenizer.Tokenize("i = 0; while (i < 20 + (4 * 4)) {i++;}");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(String.Empty, expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(String.Empty, actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest3()
		{
			ForLoopRemover target = new ForLoopRemover();

			List<Token> tokenList = Tokenizer.Tokenize("for (;;);");
			List<Token> expected = Tokenizer.Tokenize("; while (1) {;}");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(String.Empty, expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(String.Empty, actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest4()
		{
			ForLoopRemover target = new ForLoopRemover();

			List<Token> tokenList = Tokenizer.Tokenize("for (j = 2, i = 0; i < 20 && j < 20; j += 10, i++) {int var; var = 10;}");
			List<Token> expected = Tokenizer.Tokenize("j = 2, i = 0; while (i < 20 && j < 20) {int var; var = 10; j += 10, i++;}");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(String.Empty, expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(String.Empty, actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest5()
		{
			ForLoopRemover target = new ForLoopRemover();

			List<Token> tokenList = Tokenizer.Tokenize("for (;i < 20;);");
			List<Token> expected = Tokenizer.Tokenize("; while (i < 20) {;}");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(String.Empty, expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(String.Empty, actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest6()
		{
			ForLoopRemover target = new ForLoopRemover();

			List<Token> tokenList = Tokenizer.Tokenize("for (i = 0; i < 20; i++) i = 5;");
			List<Token> expected = Tokenizer.Tokenize("i = 0; while (i < 20) { i = 5; i++;}");

			List<Token> actual;
			try
			{
				actual = target.Run(tokenList);
			}
			catch (Exception ex)
			{
				Assert.AreEqual("Invalid token of some kind", ex.Message);
				return;
			}
			Assert.Fail("No exception thrown");
		}
	}
}
