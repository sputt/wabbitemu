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
		[TestMethod()]
		public void ArrayDeref_Test1()
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
        public void ArrayDeref_Test2()
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
        public void ArrayDeref_Test3()
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
        public void ArrayDeref_Test4()
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
        public void ArrayDeref_Test5()
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
