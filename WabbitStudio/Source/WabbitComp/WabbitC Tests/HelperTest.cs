using WabbitC.StatementPasses.RegisterAllocator;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;
using WabbitC.Model;
using WabbitC.Model.Statements;
using System.Collections.Generic;

namespace WabbitC_Tests
{
    
    
    /// <summary>
    ///This is a test class for HelperTest and is intended
    ///to contain all HelperTest Unit Tests
    ///</summary>
	[TestClass()]
	public class HelperTest
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
		///A test for GetMostReferencedVariables
		///</summary>
		[TestMethod()]
		public void GetMostReferencedVariablesTest()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));

			block.Statements.Add(new Assignment(block.FindDeclaration("a"), new Immediate(10)));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("a")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("b"), block.FindDeclaration("a")));

			Helper target = new Helper(block); // TODO: Initialize to an appropriate value
			List<Declaration> expected = new List<Declaration>() { block.FindDeclaration("a"), block.FindDeclaration("b") }; // TODO: Initialize to an appropriate value
			List<Declaration> actual;
			actual = target.GetMostReferencedVariables();
			Assert.IsTrue(expected.SequenceEqual<Declaration>(actual));
		}

		[TestMethod()]
		public void GetMostModifiedVariablesTest()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));

			block.Statements.Add(new Assignment(block.FindDeclaration("a"), new Immediate(10)));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("a")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("b"), block.FindDeclaration("a")));

			Helper target = new Helper(block); // TODO: Initialize to an appropriate value
			List<Declaration> expected = new List<Declaration>() { block.FindDeclaration("b"), block.FindDeclaration("a") }; // TODO: Initialize to an appropriate value
			List<Declaration> actual;
			actual = target.GetMostModifiedVariables();
			Assert.IsTrue(expected.SequenceEqual<Declaration>(actual));
		}
	}
}
