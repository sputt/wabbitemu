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
	public class CharRemoverTest
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
			CharacterRemover target = new CharacterRemover();

			List<Token> tokenList = Tokenizer.Tokenize("int test = '\\a'+'\\b'+'\\f'+'\\n'+'\\r'+'\\t'+'\\v'+'\\''+'\\\\'+'\"'+'\\0'+'\\x40';");
			List<Token> expected = Tokenizer.Tokenize("int test = 7 + 8 + 12 + 10 + 13 + 9 + 11 + 39 + 92 + 34 + 0 + 40;");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}

		[TestMethod()]
		public void RunTest2()
		{
			CharacterRemover target = new CharacterRemover();

			List<Token> tokenList = Tokenizer.Tokenize("int test = '\\?';");

			List<Token> actual;
			try
			{
				actual = target.Run(tokenList);
			}
			catch (Exception ex)
			{
				Assert.AreEqual(ex.Message, "Invalid escape sequence");
				return;
			}
			Assert.Fail("No exception thrown");
		}

		[TestMethod()]
		public void RunTest3()
		{
			CharacterRemover target = new CharacterRemover();

			List<Token> tokenList = Tokenizer.Tokenize("int test = 'blah';");

			List<Token> actual;
			try
			{
				actual = target.Run(tokenList);
			}
			catch (Exception ex)
			{
				Assert.AreEqual(ex.Message, "Too many characters in single quote");
				return;
			}
			Assert.Fail("No exception thrown");
		}

		[TestMethod()]
		public void RunTest4()
		{
			CharacterRemover target = new CharacterRemover();

			List<Token> tokenList = Tokenizer.Tokenize("int test = '\b blah';");

			List<Token> actual;
			try
			{
				actual = target.Run(tokenList);
			}
			catch (Exception ex)
			{
				Assert.AreEqual(ex.Message, "Too many characters in single quote");
				return;
			}
			Assert.Fail("No exception thrown");
		}
	}
}
