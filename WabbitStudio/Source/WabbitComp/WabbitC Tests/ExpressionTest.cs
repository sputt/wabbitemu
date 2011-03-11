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
            Tokenizer tokenizer = new Tokenizer();
            tokenizer.Tokenize("test2 = 20");
            List<Token> tokens = tokenizer.Tokens; // TODO: Initialize to an appropriate value
            Expression target = new Expression(tokens); // TODO: Initialize to an appropriate value

            //var expr1 = new Expression(Tokenizer.ToToken("="));
            //var expr2 = new Expression(Tokenizer.ToToken("test"));
            var expr3 = new Expression(Tokenizer.ToToken("="));
            var expr4 = new Expression(Tokenizer.ToToken("test2"));
            var expr5 = new Expression(Tokenizer.ToToken("20"));
            
            List<Expression> expected = new List<Expression>(); // TODO: Initialize to an appropriate value
           // expected.Add(expr1);
           // expected.Add(expr2);
            expected.Add(expr3);
            expected.Add(expr4);
            expected.Add(expr5);
            List<Expression> actual;
            actual = target.Eval();
			Compare(expected, actual);
        }

        [TestMethod()]
        public void EvalTest2()
        {
            Tokenizer tokenizer = new Tokenizer();
            tokenizer.Tokenize("test + (test2 + 20)");
            List<Token> tokens = tokenizer.Tokens; // TODO: Initialize to an appropriate value
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
			Tokenizer tokenizer = new Tokenizer();
			tokenizer.Tokenize("20 / (10 - 8) + 9 * 7");
			List<Token> tokens = tokenizer.Tokens; // TODO: Initialize to an appropriate value
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
            Tokenizer tokenizer = new Tokenizer();
            tokenizer.Tokenize("!test");
            List<Token> tokens = tokenizer.Tokens;
            Expression target = new Expression(tokens);

            List<Expression> expected = new List<Expression>();

            var expr1 = new Expression(Tokenizer.ToToken("!"));
            var expr2 = new Expression(Tokenizer.ToToken("test"));

            expected.Add(expr1);
            expected.Add(expr2);

            List<Expression> actual;
            actual = target.Eval();
			Compare(expected, actual);
        }

		[TestMethod()]
		public void EvalTest5()
		{
			Tokenizer tokenizer = new Tokenizer();
			tokenizer.Tokenize("!test");
			List<Token> tokens = tokenizer.Tokens;
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();

			var expr1 = new Expression(Tokenizer.ToToken("!"));
			var expr2 = new Expression(Tokenizer.ToToken("test"));

			expected.Add(expr1);
			expected.Add(expr2);

			List<Expression> actual;
			actual = target.Eval();
			Compare(expected, actual);
		}

		[TestMethod()]
		public void EvalTest6()
		{
			Tokenizer tokenizer = new Tokenizer();
			tokenizer.Tokenize("test = cool ? not : definitely");
			List<Token> tokens = tokenizer.Tokens;
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
            Tokenizer tokenizer = new Tokenizer();
            tokenizer.Tokenize("*test * 2");
            List<Token> tokens = tokenizer.Tokens;
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
			Tokenizer tokenizer = new Tokenizer();
			tokenizer.Tokenize("*test++");
			List<Token> tokens = tokenizer.Tokens;
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
			Tokenizer tokenizer = new Tokenizer();
			tokenizer.Tokenize("test = (10 *arg * (arg3 * 20)) + (arg2 + 40)");
			List<Token> tokens = tokenizer.Tokens;
			Expression target = new Expression(tokens);

			List<Expression> expected = new List<Expression>();
			expected.Add(new Expression(Tokenizer.ToToken("=")));
			expected.Add(new Expression(Tokenizer.ToToken("test")));
			expected.Add(new Expression(Tokenizer.ToToken("+")));
			expected.Add(new Expression(Tokenizer.ToToken("+")));
			expected.Add(new Expression(Tokenizer.ToToken("arg2")));
			expected.Add(new Expression(new List<Token> { Tokenizer.ToToken("arg"), Tokenizer.ToToken("*"), Tokenizer.ToToken("arg3"), Tokenizer.ToToken("*"), Tokenizer.ToToken("200") }));
			expected.Add(new Expression(Tokenizer.ToToken("40")));

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
