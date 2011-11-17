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
	public class ArrayDerefTest
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


		[TestMethod()]
		public void RunTest1()
		{
			ArrayDereference target = new ArrayDereference();

			List<Token> tokenList = Tokenizer.Tokenize("int a[10]; a[4] = 3;");
			List<Token> expected = Tokenizer.Tokenize("int a[10]; *(a + 4) = 3;");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest2()
		{
			ArrayDereference target = new ArrayDereference();

			List<Token> tokenList = Tokenizer.Tokenize("int a[10]; a[4 + 8] = 3;");
			List<Token> expected = Tokenizer.Tokenize("int a[10]; *(a + (4 + 8)) = 3;");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest3()
		{
			ArrayDereference target = new ArrayDereference();

			List<Token> tokenList = Tokenizer.Tokenize("int a[10]; a[a[5]] = 3;");
			List<Token> expected = Tokenizer.Tokenize("int a[10]; *(a + (*(a + 5))) = 3;");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest4()
		{
			ArrayDereference target = new ArrayDereference();

			List<Token> tokenList = Tokenizer.Tokenize("return a[40];");
			List<Token> expected = Tokenizer.Tokenize("return *(a + 40);");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");

		}

		[TestMethod()]
		public void RunTest5()
		{
			ArrayDereference target = new ArrayDereference();

			List<Token> tokenList = Tokenizer.Tokenize("return a");
			List<Token> expected = Tokenizer.Tokenize("return a");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");

		}
	}
}
