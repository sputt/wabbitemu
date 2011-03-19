using WabbitC;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;

namespace WabbitC_Tests
{
    
    
    /// <summary>
    ///This is a test class for TokenizerTest and is intended
    ///to contain all TokenizerTest Unit Tests
    ///</summary>
    [TestClass()]
    public class TokenizerTest
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
        ///A test for Tokenize
        ///</summary>
        [TestMethod()]
        public void TokenizeTest1()
        {
            List<Token> tokens = Tokenizer.Tokenize("10");

            Assert.AreEqual(TokenType.IntType, tokens[0].Type);
            Assert.AreEqual("10", tokens[0].Text);
        }

        [TestMethod()]
        public void TokenizeTest2()
        {
            List<Token> tokens = Tokenizer.Tokenize("10.0");

            Assert.AreEqual(TokenType.RealType, tokens[0].Type);
            Assert.AreEqual("10.0", tokens[0].Text);
        }

        [TestMethod()]
        public void TokenizeTest3()
        {
            List<Token> tokens = Tokenizer.Tokenize("10.0f");

            Assert.AreEqual(TokenType.RealType, tokens[0].Type);
            Assert.AreEqual("10.0f", tokens[0].Text);
        }

        [TestMethod()]
        public void TokenizeTest4()
        {
            List<Token> tokens = Tokenizer.Tokenize("10L");

            Assert.AreEqual(TokenType.IntType, tokens[0].Type);
            Assert.AreEqual("10L", tokens[0].Text);
        }

        [TestMethod()]
        public void TokenizeTest5()
        {
            List<Token> tokens = Tokenizer.Tokenize("10UL");

            Assert.AreEqual(TokenType.IntType, tokens[0].Type);
            Assert.AreEqual("10UL", tokens[0].Text);
        }

        /*[TestMethod()]
        public void TokenizeTest6()
        {
            List<Token> tokens = Tokenizer.Tokenize("-10");

            Assert.AreEqual(TokenType.IntType, tokens[0].Type);
            Assert.AreEqual("-10", tokens[0].Text);
        }

        [TestMethod()]
        public void TokenizeTest7()
        {
            List<Token> tokens = Tokenizer.Tokenize("-10.0");

            Assert.AreEqual(TokenType.RealType, tokens[0].Type);
            Assert.AreEqual("-10.0", tokens[0].Text);
        }*/

        [TestMethod()]
        public void TokenizeTest8()
        {
            List<Token> tokens = Tokenizer.Tokenize("0x10");

            Assert.AreEqual(TokenType.IntType, tokens[0].Type);
            Assert.AreEqual("0x10", tokens[0].Text);
        }

        [TestMethod()]
        public void TokenizeTest9()
        {
            List<Token> tokens = Tokenizer.Tokenize("010");

            Assert.AreEqual(TokenType.IntType, tokens[0].Type);
            Assert.AreEqual("010", tokens[0].Text);
        }
    }
}
