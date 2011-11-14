using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Linq;
using System.Collections.Generic;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.StatementPasses.RegisterAllocator;

using WabbitC.Model.Types;

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


		private void AddRegisters(Block block)
		{
			Module mod = new Module();
			mod.UpdateModule(block);
		}

		/// <summary>
		///A test for GetMostReferencedVariables
		///</summary>
		[TestMethod()]
		public void GetMostReferencedVariablesTest()
		{
			Block block = new Block();

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));

			block.Statements.Add(new Assignment(block.FindDeclaration("a"), new Immediate(10)));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("a")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("b"), block.FindDeclaration("a")));

			RegisterHelper target = new RegisterHelper(block); // TODO: Initialize to an appropriate value
			List<Declaration> expected = new List<Declaration>() { block.FindDeclaration("a"), block.FindDeclaration("b") }; // TODO: Initialize to an appropriate value
			List<Declaration> actual;
			actual = target.GetMostReferencedVariables();
			Assert.IsTrue(expected.SequenceEqual<Declaration>(actual));
		}

		[TestMethod()]
		public void GetMostModifiedVariablesTest()
		{
			Block block = new Block();

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));

			block.Statements.Add(new Assignment(block.FindDeclaration("a"), new Immediate(10)));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("a")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("b"), block.FindDeclaration("a")));

			RegisterHelper target = new RegisterHelper(block);
			List<Declaration> expected = new List<Declaration>() { block.FindDeclaration("b"), block.FindDeclaration("a") }; // TODO: Initialize to an appropriate value
			List<Declaration> actual;
			actual = target.GetMostModifiedVariables();
			Assert.IsTrue(expected.SequenceEqual<Declaration>(actual));
		}

		/// <summary>
		///A test for Alloc16
		///</summary>
		[TestMethod()]
		public void AllocTest1()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "c"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "d"));

			var statements = new List<Statement>();
			RegisterHelper target = new RegisterHelper(block);

			Declaration decl;
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("b"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 2");
			decl = target.AllocateRegister(block.FindDeclaration("c"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 3");
			decl = target.AllocateRegister(block.FindDeclaration("d"), ref statements);
			Assert.IsTrue(decl == null, "Did not fail alloc 4");

		}

		[TestMethod()]
		public void AllocTest2()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "b"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "c"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "d"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "e"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "f"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "g"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "h"));

			var statements = new List<Statement>();
			RegisterHelper target = new RegisterHelper(block);

			Declaration decl;
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("b"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 2");
			decl = target.AllocateRegister(block.FindDeclaration("c"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 3");
			decl = target.AllocateRegister(block.FindDeclaration("d"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 4");
			decl = target.AllocateRegister(block.FindDeclaration("e"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 5");
			decl = target.AllocateRegister(block.FindDeclaration("f"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 6");
			decl = target.AllocateRegister(block.FindDeclaration("g"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 7");
			decl = target.AllocateRegister(block.FindDeclaration("h"), ref statements);
			Assert.IsTrue(decl == null, "Did not fail alloc 8");

		}

		[TestMethod()]
		public void AllocTest3()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "b"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "c"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "d"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "e"));

			var statements = new List<Statement>();
			RegisterHelper target = new RegisterHelper(block);

			Declaration decl;
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("b"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 2");
			decl = target.AllocateRegister(block.FindDeclaration("c"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 3");
			decl = target.AllocateRegister(block.FindDeclaration("d"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 4");
			decl = target.AllocateRegister(block.FindDeclaration("e"), ref statements);
			Assert.IsTrue(decl == null, "Did not fail alloc 5");
		}

		[TestMethod()]
		public void AllocTest4()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "b"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "c"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "d"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "e"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("char"), "f"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "g"));

			var statements = new List<Statement>();
			RegisterHelper target = new RegisterHelper(block);

			Declaration decl;
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("b"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 2");
			decl = target.AllocateRegister(block.FindDeclaration("c"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 3");
			decl = target.AllocateRegister(block.FindDeclaration("d"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 4");
			decl = target.AllocateRegister(block.FindDeclaration("e"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 5");
			decl = target.AllocateRegister(block.FindDeclaration("f"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 6");

			target.FreeRegister(block.FindDeclaration("b"));
			target.FreeRegister(block.FindDeclaration("f"));

			var regs = target.GetFreeRegisters();

			decl = target.AllocateRegister(block.FindDeclaration("g"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 7");
		}

		[TestMethod()]
		public void AllocTest5()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));

			var statements = new List<Statement>();
			RegisterHelper target = new RegisterHelper(block);

			Declaration decl;
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
			decl = target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			Assert.IsTrue(decl != null, "Failed alloc 1");
		}

		[TestMethod()]
		public void LeastImportant1()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "c"));

			block.Statements.Add(new Assignment(block.FindDeclaration("b"), new Immediate(10)));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("c")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("b"), block.FindDeclaration("a")));

			RegisterHelper target = new RegisterHelper(block); // TODO: Initialize to an appropriate value
			var statements = new List<Statement>();
			target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			target.AllocateRegister(block.FindDeclaration("b"), ref statements);
			Declaration decl = target.GetLeastImportantAllocatedVariable(block.Statements[0]);

			Assert.AreEqual(decl, block.FindDeclaration("a"));
		}

		[TestMethod()]
		public void LeastImportant2()
		{
			Block block = new Block();
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "c"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "cond"));


			Label lbl = new Label("test");

			block.Statements.Add(new Assignment(block.FindDeclaration("b"), new Immediate(10)));
			block.Statements.Add(new Goto(lbl, block.FindDeclaration("cond"), Goto.GotoCondition.NC));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("b")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("a"), block.FindDeclaration("b")));
			block.Statements.Add(lbl);
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("c"), block.FindDeclaration("b")));

			RegisterHelper target = new RegisterHelper(block); // TODO: Initialize to an appropriate value
			var statements = new List<Statement>();
			target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			target.AllocateRegister(block.FindDeclaration("c"), ref statements);
			Declaration decl = target.GetLeastImportantAllocatedVariable(block.Statements[0]);

			Assert.AreEqual(decl, block.FindDeclaration("a"));
		}

		[TestMethod()]
		public void LeastImportant3()
		{
			Block block = new Block(); // TODO: Initialize to an appropriate value
			AddRegisters(block);

			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "a"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "b"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "c"));
			block.Declarations.Add(new Declaration(new WabbitC.Model.Types.BuiltInType("int"), "cond"));


			Label lbl = new Label("test");

			block.Statements.Add(new Assignment(block.FindDeclaration("c"), new Immediate(10)));
			block.Statements.Add(new Goto(lbl, block.FindDeclaration("cond"), Goto.GotoCondition.NC));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("b")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("a"), block.FindDeclaration("b")));
			block.Statements.Add(lbl);
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("b")));
			block.Statements.Add(new Move(block.FindDeclaration("b"), block.FindDeclaration("b")));
			block.Statements.Add(new WabbitC.Model.Statements.Math.Add(block.FindDeclaration("c"), block.FindDeclaration("b")));

			RegisterHelper target = new RegisterHelper(block); // TODO: Initialize to an appropriate value
			var statements = new List<Statement>();
			target.AllocateRegister(block.FindDeclaration("a"), ref statements);
			target.AllocateRegister(block.FindDeclaration("c"), ref statements);
			Declaration decl = target.GetLeastImportantAllocatedVariable(block.Statements[0]);

			Assert.AreEqual(decl, block.FindDeclaration("c"));
		}
	}
}
