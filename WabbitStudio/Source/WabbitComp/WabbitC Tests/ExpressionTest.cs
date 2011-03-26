using WabbitC;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Linq;

namespace WabbitC_Tests
{
    
    
    /// <summary>
    ///This is a test class for ExpressionTest and is intended
    ///to contain all ExpressionTest Unit Tests
    ///</summary>
    [TestClass()]
    public class ExpressionTest
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
        ///A test for Eval
        ///</summary>
        [TestMethod()]
        public void EvalTest1()
        {

			List<Token> tokens = Tokenizer.Tokenize("test = test2 = 20");// TODO: Initialize to an appropriate value
            Expression target = new Expression(tokens); // TODO: Initialize to an appropriate value

			List<Expression> expected = new List<Expression>(); // TODO: Initialize to an appropriate value
			expected.Add(new Expression(Tokenizer.ToToken("=")));
			expected.Add(new Expression(Tokenizer.ToToken("test")));
            expected.Add(new Expression(Tokenizer.ToToken("=")));
            expected.Add(new Expression(Tokenizer.ToToken("test2")));
            expected.Add(new Expression(Tokenizer.ToToken("20")));
            
            List<Expression> actual;
            actual = target.Eval();
			Compare(expected, actual);
        }

        [TestMethod()]
        public void EvalTest2()
        {
			List<Token> tokens = Tokenizer.Tokenize("test + (test2 + 20)"); // TODO: Initialize to an appropriate value
            Expression target = new Expression(tokens); // TODO: Initialize to an appropriate value

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("+")));
            expected.Add(new Expression(Tokenizer.ToToken("+")));
            expected.Add(new Expression(Tokenizer.ToToken("test")));
            expected.Add(new Expression(Tokenizer.ToToken("test2")));
			expected.Add(new Expression(Tokenizer.ToToken("20")));

            List<Expression> actual;
            actual = target.Eval();
			Compare(expected, actual);
        }

		[TestMethod()]
		public void EvalTest3()
		{
			List<Token> tokens = Tokenizer.Tokenize("20 / (10 - 8) + 9 * 7"); ; // TODO: Initialize to an appropriate value
			Expression target = new Expression(tokens); // TODO: Initialize to an appropriate value

			List<Expression> expected = new List<Expression>(); // TODO: Initialize to an appropriate value
			expected.Add(new Expression(Tokenizer.ToToken("73")));
			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

        [TestMethod()]
        public void EvalTest4()
        {

			List<Token> tokens = Tokenizer.Tokenize("!test");
            Expression target = new Expression(tokens);

            List<Expression> expected = new List<Expression>();
            expected.Add(new Expression(Tokenizer.ToToken("!")));
            expected.Add(new Expression(Tokenizer.ToToken("test")));

            List<Expression> actual;
            actual = target.Eval();
			Compare(expected, actual);
        }

		[TestMethod()]
		public void EvalTest5()
		{
			List<Token> tokens = Tokenizer.Tokenize("test = function(10, arg2) + arg1 + 40");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("=")));
			expected.Add(new Expression(Tokenizer.ToToken("test")));
			expected.Add(new Expression(Tokenizer.ToToken("+")));
			expected.Add(new Expression(Tokenizer.ToToken("+")));
			expected.Add(new Expression(Tokenizer.Tokenize("function(10, arg2)")).Eval()[0]);
			expected.Add(new Expression(Tokenizer.ToToken("arg1")));
			expected.Add(new Expression(Tokenizer.ToToken("40")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

		[TestMethod()]
		public void EvalTest6()
		{
			List<Token> tokens = Tokenizer.Tokenize("test = cool ? not : definitely");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Token.AssignmentOperatorToken));
			expected.Add(new Expression(Tokenizer.ToToken("test")));
			expected.Add(new Expression(Tokenizer.ToToken("?")));
			expected.Add(new Expression(Tokenizer.ToToken("cool")));
			expected.Add(new Expression(Tokenizer.ToToken("not")));
			expected.Add(new Expression(Tokenizer.ToToken("definitely")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

		[TestMethod()]
        public void EvalTest7()
        {
			List<Token> tokens = Tokenizer.Tokenize("*test * 2"); ;
            Expression target = new Expression(tokens);

            List<Expression> expected = new List<Expression>();
            expected.Add(new Expression(Tokenizer.ToToken("*")));
            expected.Add(new Expression(Tokenizer.ToToken("*")));
            expected.Add(new Expression(Tokenizer.ToToken("test")));
            expected.Add(new Expression(Tokenizer.ToToken("2")));

            List<Expression> actual;
            actual = target.Eval();
			Compare(expected, actual);
        }

		[TestMethod()]
		public void EvalTest8()
		{
			List<Token> tokens = Tokenizer.Tokenize("*test++");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("*")));
			expected.Add(new Expression(Tokenizer.ToToken("++")));
			expected.Add(new Expression(Tokenizer.ToToken("test")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

		[TestMethod()]
		public void EvalTest9()
		{
			List<Token> tokens = Tokenizer.Tokenize("test = !(x < y  && u != v) ? y : x >= z ? z : 1");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("=")));
			expected.Add(new Expression(Tokenizer.ToToken("test")));
			expected.Add(new Expression(Tokenizer.ToToken("?")));
			expected.Add(new Expression(Tokenizer.ToToken("!")));
			expected.Add(new Expression(Tokenizer.Tokenize("x < y  && u != v")));
			expected.Add(new Expression(Tokenizer.ToToken("y")));
			expected.Add(new Expression(Tokenizer.ToToken("?")));
			expected.Add(new Expression(Tokenizer.ToToken(">=")));
			expected.Add(new Expression(Tokenizer.ToToken("x")));
			expected.Add(new Expression(Tokenizer.ToToken("z")));
			expected.Add(new Expression(Tokenizer.ToToken("z")));
			expected.Add(new Expression(Tokenizer.ToToken("1")));


			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

		[TestMethod()]
		public void EvalTest10()
		{
			List<Token> tokens = Tokenizer.Tokenize("sizeof (int) * x");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("*")));
			expected.Add(new Expression(Tokenizer.Tokenize("sizeof(int)")).Eval()[0]);
			expected.Add(new Expression(Tokenizer.ToToken("x")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

		[TestMethod()]
		public void CastTest1()
		{
			List<Token> tokens = Tokenizer.Tokenize("(unsigned char *) test");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.Tokenize("unsigned char *")));
			expected.Add(new Expression(Tokenizer.ToToken("test")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
			Assert.IsTrue(actual[0].IsCast, "Not marked as a cast properly");
		}

		[TestMethod()]
		public void CastTest2()
		{
			List<Token> tokens = Tokenizer.Tokenize("(int *) ((unsigned char *) foo + (unsigned char *) bar)");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.Tokenize("int *")));
			expected.Add(new Expression(Tokenizer.Tokenize("(unsigned char *) foo + (unsigned char *) bar")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
			Assert.IsTrue(actual[0].IsCast, "Not marked as a cast properly");
		}

		[TestMethod()]
		public void CastTest3()
		{
			List<Token> tokens = Tokenizer.Tokenize("(int) (char) 342");
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("int")));
			expected.Add(new Expression(Tokenizer.ToToken("char")));
			expected.Add(new Expression(Tokenizer.ToToken("342")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
			Assert.IsTrue(actual[0].IsCast, "Not marked as a cast properly");
			Assert.IsTrue(actual[1].IsCast, "Not marked as a cast properly");
		}

        [TestMethod()]
        public void CastTest4()
        {
            List<Token> tokens = Tokenizer.Tokenize("(test)-342");
            Expression target = new Expression(tokens);

            List<Expression> expected = new List<Expression>();
            expected.Add(new Expression(Tokenizer.ToToken("test")));
            expected.Add(new Expression(Tokenizer.ToToken("-342")));

            List<Expression> actual;
            actual = target.Eval();
            Compare(expected, actual);
            Assert.IsTrue(actual[0].IsCast, "Not marked as a cast properly");
        }

		[TestMethod()]
		public void CommaOperatorTest1()
		{
			List<Token> tokens = Tokenizer.Tokenize("d = 10, d = testfunc(with, args)");
			Expression target = new Expression(tokens);

			var actual = target.Eval();

			var expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken(",")));
			expected.Add(new Expression(Token.AssignmentOperatorToken));
			expected.Add(new Expression(Tokenizer.ToToken("d")));
			expected.Add(new Expression(Tokenizer.ToToken("10")));
			expected.Add(new Expression(Token.AssignmentOperatorToken));
			expected.Add(new Expression(Tokenizer.ToToken("d")));
			expected.Add(new Expression(Tokenizer.Tokenize("testfunc(with, args)")).Eval()[0]);

			Compare(expected, actual);
		}

        [TestMethod()]
        public void TripleDerefTest()
        {
			var tokens = Tokenizer.Tokenize("***test");
            var target = new Expression(tokens);

            var actual = target.Eval();

            Assert.AreEqual(1, actual[0].Operands, "First dereference operand count wrong");
            Assert.AreEqual(1, actual[1].Operands, "Second dereference operand count wrong");
			Assert.AreEqual(1, actual[2].Operands, "Third dereference operand count wrong");
            Assert.AreEqual("test", actual[3].Tokens[0], "Was not parsed correctly");
        }

        [TestMethod()]
        public void AddressOfTest()
        {
            var tokens = Tokenizer.Tokenize("ptest = &test");
            var target = new Expression(tokens);
            List<Expression> expected = new List<Expression>();
            expected.Add(new Expression(Tokenizer.ToToken("=")));
            expected.Add(new Expression(Tokenizer.ToToken("ptest")));
            expected.Add(new Expression(Tokenizer.ToToken("&")));
            expected.Add(new Expression(Tokenizer.ToToken("test")));

            List<Expression> actual;
            actual = target.Eval();
            Compare(expected, actual);
        }

		[TestMethod()]
		public void OptimizeTest1()
		{
			List<Token> tokens = Tokenizer.Tokenize("test = (10 *arg * (arg3 * 20)) + (arg2 + 40)"); ;
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("=")));
			expected.Add(new Expression(Tokenizer.ToToken("test")));
			expected.Add(new Expression(Tokenizer.ToToken("+")));
			expected.Add(new Expression(Tokenizer.ToToken("+")));
			expected.Add(new Expression(Tokenizer.ToToken("arg2")));
			expected.Add(new Expression(Tokenizer.Tokenize("arg*arg3*200")));
			expected.Add(new Expression(Tokenizer.ToToken("40")));

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

        [TestMethod()]
        public void DoubleNegative()
        {
            List<Token> tokens = Tokenizer.Tokenize("-(-300)"); ;
            Expression target = new Expression(tokens);

            List<Expression> expected = new List<Expression>();
            expected.Add(new Expression(Tokenizer.ToToken("300")));

            List<Expression> actual;
            actual = target.Eval();
            Compare(expected, actual);
        }

		void Compare(List<Expression> expected, List<Expression> actual)
		{
			for (int i = 0; i < expected.Count; i++)
			{
				Assert.IsTrue(expected[i].Tokens.SequenceEqual<Token>(actual[i].Tokens),
					"Expected: \"" + string.Join<Token>("", expected[i].Tokens.ToArray()) + "\" " +
					"Actual: \"" + string.Join<Token>("", actual[i].Tokens.ToArray()) + "\"");
			}
		}
    }
}
