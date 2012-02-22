using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project.Interface;
using Moq;
using System.Linq;

namespace WabbitcodeTest
{
	/// <summary>
	///This is a test class for ParserServiceTest and is intended
	///to contain all ParserServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class ParserServiceTest
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
		///A test for DestroyService
		///</summary>
		[TestMethod()]
		public void DestroyServiceTest()
		{
			Mock<IProject> projectMock = new Mock<IProject>(MockBehavior.Strict);
			ParserService target = new ParserService();
			target.InitService(projectMock.Object);
			target.DestroyService();
			Assert.IsNull(target.Project);
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest()
		{
			Mock<IProject> projectMock = new Mock<IProject>(MockBehavior.Strict);
			ParserService target = new ParserService();
			target.InitService(projectMock.Object);
			Assert.AreEqual(projectMock.Object, target.Project);
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest_ArgumentNull()
		{
			IProject project = null;
			ParserService target = new ParserService();
			try
			{
				target.InitService(project);
			}
			catch (ArgumentException ex)
			{
				Assert.IsTrue(ex.Message.Contains("IProject"));
				return;
			}
			Assert.Fail("Exception not thrown");
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest_ArgumentMissing()
		{
			ParserService target = new ParserService();
			try
			{
				target.InitService(new object[0]);
			}
			catch (ArgumentException ex)
			{
				Assert.IsTrue(ex.Message.Contains("arguments"));
				return;
			}
			Assert.Fail("Exception not thrown");
		}

		//Because we can parse at any point we need to have extensive tests for
		//files that are incomplete or missing values
		#region Label and Equate Tests

		#region Label Tests

		[TestMethod()]
		public void ParseFileTest_LabelTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1: \\Label2:";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.LabelsList.Count);
			Assert.AreEqual("Label1", actual.LabelsList.ElementAt(0).LabelName);
			Assert.AreEqual("Label2", actual.LabelsList.ElementAt(1).LabelName);
		}

		[TestMethod()]
		public void ParseFileTest_LabelTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1: \r\nLabel2:";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.LabelsList.Count);
			Assert.AreEqual("Label1", actual.LabelsList.ElementAt(0).LabelName);
			Assert.AreEqual("Label2", actual.LabelsList.ElementAt(1).LabelName);
		}

		[TestMethod()]
		public void ParseFileTest_MacroNotLabelTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "NotLabel1() \\NotLabel2()";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.LabelsList.Count);
		}

		[TestMethod()]
		public void ParseFileTest_MacroNotLabelTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "NotLabel1() \r\nNotLabel2()";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.LabelsList.Count);
		}

		[TestMethod()]
		public void ParseFileTest_LabelNoColonTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 \\Label2";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.LabelsList.Count);
			Assert.AreEqual("Label1", actual.LabelsList.ElementAt(0).LabelName);
			Assert.AreEqual("Label2", actual.LabelsList.ElementAt(1).LabelName);
		}

		[TestMethod()]
		public void ParseFileTest_LabelNoColonTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 \r\nLabel2";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.LabelsList.Count);
			Assert.AreEqual("Label1", actual.LabelsList.ElementAt(0).LabelName);
			Assert.AreEqual("Label2", actual.LabelsList.ElementAt(1).LabelName);
		}

		#endregion

		#region Equate Tests

		[TestMethod()]
		public void ParseFileTest_EqualsTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 = 10\\Label2 = 6";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("10", actual.DefinesList.ElementAt(0).Contents);
			Assert.AreEqual("6", actual.DefinesList.ElementAt(1).Contents);
		}

		[TestMethod()]
		public void ParseFileTest_EqualsTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 = 10\r\nLabel2 = 6";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("10", actual.DefinesList.ElementAt(0).Contents);
			Assert.AreEqual("6", actual.DefinesList.ElementAt(1).Contents);
		}

		[TestMethod()]
		public void ParseFileTest_EquateTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 .equ 10\\Label2 .equ 6";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("10", actual.DefinesList.ElementAt(0).Contents);
			Assert.AreEqual("6", actual.DefinesList.ElementAt(1).Contents);
		}

		[TestMethod()]
		public void ParseFileTest_EquateTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 .equ 10\r\nLabel2 .equ 6";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("10", actual.DefinesList.ElementAt(0).Contents);
			Assert.AreEqual("6", actual.DefinesList.ElementAt(1).Contents);
		}

		[TestMethod()]
		public void ParseFileTest_EqualsTest_MissingValue_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 = \\Label2 = ";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(0).Contents));
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(1).Contents));
		}

		[TestMethod()]
		public void ParseFileTest_EqualsTest_MissingValue_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 = \r\nLabel2 = ";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(0).Contents));
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(1).Contents));
		}

		[TestMethod()]
		public void ParseFileTest_EquateTest_MissingValue_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 .equ \\Label2 .equ ";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(0).Contents));
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(1).Contents));
		}

		[TestMethod()]
		public void ParseFileTest_EquateTest_MissingValue_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1 .equ \r\nLabel2 .equ ";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(0).Contents));
			Assert.IsTrue(string.IsNullOrEmpty(actual.DefinesList.ElementAt(1).Contents));
		}

		#endregion

		#region Define Tests
		[TestMethod()]
		public void ParseFileTest_DefineTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#define Label1 \\ #define Label2";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(0).Name);
		}

		[TestMethod()]
		public void ParseFileTest_DefineTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#define Label1 \r\n #define Label2";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(0).Name);
		}

		[TestMethod()]
		public void ParseFileTest_DefineWithValueTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#define Label1 10\\ #define Label2 20";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("10", actual.DefinesList.ElementAt(0).Contents);
			Assert.AreEqual("20", actual.DefinesList.ElementAt(1).Contents);
		}

		[TestMethod()]
		public void ParseFileTest_DefineWithValueTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#define Label1 10\r\n #define Label2 20";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.DefinesList.Count);
			Assert.AreEqual("Label1", actual.DefinesList.ElementAt(0).Name);
			Assert.AreEqual("Label2", actual.DefinesList.ElementAt(1).Name);
			Assert.AreEqual("10", actual.DefinesList.ElementAt(0).Contents);
			Assert.AreEqual("20", actual.DefinesList.ElementAt(1).Contents);
		}

		[TestMethod()]
		public void ParseFileTest_DefineNoNameTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#define \\ #define";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.DefinesList.Count);
		}

		[TestMethod()]
		public void ParseFileTest_DefineNoNameTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#define \\ #define ";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.DefinesList.Count);
		}

		#endregion

		#endregion

		#region Macro Tests

		[TestMethod()]
		public void ParseFileTest_MacroTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#macro TestMacro()\r\n\tld hl,1\r\n#endmacro";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.MacrosList.Count);
			Assert.AreEqual("TestMacro", actual.MacrosList.ElementAt(0).Name);
		}

		[TestMethod()]
		public void ParseFileTest_MacroTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#macro TestMacro()\\\tld hl,1\\#endmacro";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.MacrosList.Count);
		}

		[TestMethod()]
		public void ParseFileTest_MacroWithArgsTest()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#macro TestMacro(arg1, arg2)\r\n\tld hl,1\r\n#endmacro";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.MacrosList.Count);
			Assert.AreEqual("TestMacro", actual.MacrosList.ElementAt(0).Name);
			Assert.AreEqual(2, actual.MacrosList.ElementAt(0).Arguments.Count);
			Assert.AreEqual("arg1", actual.MacrosList.ElementAt(0).Arguments[0]);
			Assert.AreEqual("arg2", actual.MacrosList.ElementAt(0).Arguments[1]);
		}

		[TestMethod()]
		public void ParseFileTest_MacroWithArgsTest_ExtraComma()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#macro TestMacro(arg1, arg2, )\r\n\tld hl,1\r\n#endmacro";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.MacrosList.Count);
			Assert.AreEqual("TestMacro", actual.MacrosList.ElementAt(0).Name);
			Assert.AreEqual(2, actual.MacrosList.ElementAt(0).Arguments.Count);
			Assert.AreEqual("arg1", actual.MacrosList.ElementAt(0).Arguments[0]);
			Assert.AreEqual("arg2", actual.MacrosList.ElementAt(0).Arguments[1]);
		}

		[TestMethod()]
		public void ParseFileTest_MacroTest_NoEndMacro()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#macro TestMacro(arg1, arg2)\r\n\tld hl,1\r\n";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.MacrosList.Count);
			Assert.AreEqual("TestMacro", actual.MacrosList.ElementAt(0).Name);
		}

		#endregion

		#region Comment Tests

		[TestMethod()]
		public void ParseFileTest_BlockCommentTest()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#comment\r\nLabel1:\r\n#endcomment\r\nLabel2:";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.LabelsList.Count);
		}

		[TestMethod()]
		public void ParseFileTest_BlockCommentTest_NoEndComment()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#comment\r\nLabel1:\r\nLabel2:";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.LabelsList.Count);
		}

		[TestMethod()]
		public void ParseFileTest_SingleLineCommentTest()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "Label1:\r\n;Label2:";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.LabelsList.Count);
		}

		#endregion

		#region Include Tests

		[TestMethod()]
		public void ParseFileTest_IncludeTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#include \"Test2.asm\"\\#include \"Test3.asm\"";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.IncludeFilesList.Count);
			Assert.AreEqual("Test2.asm", actual.IncludeFilesList.ElementAt(0).IncludedFile);
			Assert.AreEqual("Test3.asm", actual.IncludeFilesList.ElementAt(1).IncludedFile);
		}

		[TestMethod()]
		public void ParseFileTest_IncludeTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#include \"Test2.asm\"\r\n#include \"Test3.asm\"";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(2, actual.IncludeFilesList.Count);
			Assert.AreEqual("Test2.asm", actual.IncludeFilesList.ElementAt(0).IncludedFile);
			Assert.AreEqual("Test3.asm", actual.IncludeFilesList.ElementAt(1).IncludedFile);
		}

		[TestMethod()]
		public void ParseFileTest_IncludeDuplicateFileTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#include \"Test2.asm\"\\#include \"Test2.asm\"";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.IncludeFilesList.Count);
			Assert.AreEqual("Test2.asm", actual.IncludeFilesList.ElementAt(0).IncludedFile);
		}

		[TestMethod()]
		public void ParseFileTest_IncludeDuplicateFileTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#include \"Test2.asm\"\r\n#include \"Test2.asm\"";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(1, actual.IncludeFilesList.Count);
			Assert.AreEqual("Test2.asm", actual.IncludeFilesList.ElementAt(0).IncludedFile);
		}

		[TestMethod()]
		public void ParseFileTest_IncludeNoFileTest_Slash()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#include \\#include ";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.IncludeFilesList.Count);
		}

		[TestMethod()]
		public void ParseFileTest_IncludeNoFileTest_Newline()
		{
			ParserService target = new ParserService();
			string file = "test.asm";
			string lines = "#include \r\n#include ";
			ParserInformation actual;
			actual = target.ParseFile(file, lines);
			Assert.AreEqual(0, actual.IncludeFilesList.Count);
		}

		#endregion
	}
}

