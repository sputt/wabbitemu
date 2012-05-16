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
		/// <summary>
		///A test for Run
		///</summary>
		[TestMethod()]
		public void Bracer_Test1()
		{
			string fileContents = Compiler.TryOpenFile(@"..\..\..\WabbitC Tests\TokenPass Tests\bracer_test.c");
			Assert.IsTrue(!string.IsNullOrEmpty(fileContents));
			List<Token> tokenList = Tokenizer.Tokenize(fileContents);

			fileContents = Compiler.TryOpenFile(@"..\..\..\WabbitC Tests\TokenPass Tests\bracer_result.c");
			Assert.IsTrue(!string.IsNullOrEmpty(fileContents));
			List<Token> expected = Tokenizer.Tokenize(fileContents);
			
			Bracer target = new Bracer();
			List<Token> actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual(actual));
		}

		[TestMethod()]
        public void Bracer_Test2()
		{
			Bracer target = new Bracer();

			List<Token> tokenList = Tokenizer.Tokenize("do if (test) test = 0; while (1); var = 2;");
			List<Token> expected = Tokenizer.Tokenize("do {if (test) {test = 0;}} while (1); var = 2;");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}

		[TestMethod()]
        public void Bracer_Test3()
		{
			Bracer target = new Bracer();

			List<Token> tokenList = Tokenizer.Tokenize("do { while (1) i += 1; } while (1);");
			List<Token> expected = Tokenizer.Tokenize("do { while (1) {i += 1;} } while (1);");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}

		[TestMethod()]
        public void Bracer_Test4()
		{
			Bracer target = new Bracer();

			List<Token> tokenList = Tokenizer.Tokenize("do { do if (test) while (0) test = 0; while (1); } while (1);");
			List<Token> expected = Tokenizer.Tokenize("do { do {if (test) {while (0) {test = 0;}}} while (1); } while (1);");

			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}

		[TestMethod()]
        public void Bracer_Test5()
		{
			Bracer target = new Bracer();

			List<Token> tokenList = Tokenizer.Tokenize("if (test) test = 0; else if ((test) > 5) while(0) test = 1;");
			List<Token> expected = Tokenizer.Tokenize("if (test) { test = 0; } else { if ((test) > 5) { while(0) { test = 1; }}}");
			 
			List<Token> actual;
			actual = target.Run(tokenList);

			Assert.IsTrue(expected.SequenceEqual<Token>(actual),
				"Expected: \"" + string.Join<Token>(" ", expected.ToArray()) + "\" " +
				"Actual: \"" + string.Join<Token>(" ", actual.ToArray()) + "\"");
		}
	}
}
