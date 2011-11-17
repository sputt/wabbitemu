using WabbitC.Model;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using WabbitC;
using System.Collections.Generic;

namespace WabbitC_Tests
{
	
	
	/// <summary>
	///This is a test class for CastHelperTest and is intended
	///to contain all CastHelperTest Unit Tests
	///</summary>
	[TestClass()]
	public class CastHelperTest
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
		///A test for IsCast
		///</summary>
		[TestMethod()]
		public void IsCastTest1()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(unsigned char *)");
			bool expected = true;
			bool actual;
			actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest2()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(char **)");
			bool expected = true;
			bool actual;
			actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest3()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(struct bob *)");
			bool expected = true;
			bool actual;
			actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest4()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(test)");
			bool expected = true;
			bool actual;
			actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest5()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(*)");
			bool expected = false;
			bool actual;
			actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest6()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(unsigned char)");
			bool expected = true;
			bool actual;
			actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest_MissingStartParen()
		{
			List<Token> tokenList = Tokenizer.Tokenize("unsigned char)");
			bool expected = false;
			bool actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest_MissingEndParen()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(unsigned char");
			bool expected = false;
			bool actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}

		[TestMethod()]
		public void IsCastTest_InvalidKeyword()
		{
			List<Token> tokenList = Tokenizer.Tokenize("(char+)");
			bool expected = false;
			bool actual = CastHelper.IsCast(tokenList);
			Assert.AreEqual(expected, actual);
		}
	}
}
