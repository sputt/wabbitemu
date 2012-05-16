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
		[TestMethod()]
		public void CharRemoverTest_Test1()
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
        public void CharRemoverTest_Test2()
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
        public void CharRemoverTest_Test3()
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
        public void CharRemoverTest_Test4()
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
