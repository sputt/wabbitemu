using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Interface;

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
		///A test for ParserService Constructor
		///</summary>
		[TestMethod()]
		public void ParserServiceConstructorTest()
		{
			ParserService target = new ParserService();
			Assert.Inconclusive("TODO: Implement code to verify target");
		}

		/// <summary>
		///A test for DestroyService
		///</summary>
		[TestMethod()]
		public void DestroyServiceTest()
		{
			ParserService target = new ParserService(); // TODO: Initialize to an appropriate value
			target.DestroyService();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest()
		{
			ParserService target = new ParserService(); // TODO: Initialize to an appropriate value
			object[] objects = null; // TODO: Initialize to an appropriate value
			target.InitService(objects);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ParseFile
		///</summary>
		[TestMethod()]
		public void ParseFileTest()
		{
			ParserService target = new ParserService(); // TODO: Initialize to an appropriate value
			string file = string.Empty; // TODO: Initialize to an appropriate value
			string lines = string.Empty; // TODO: Initialize to an appropriate value
			float increment = 0F; // TODO: Initialize to an appropriate value
			Action<double> callback = null; // TODO: Initialize to an appropriate value
			ParserInformation expected = null; // TODO: Initialize to an appropriate value
			ParserInformation actual;
			actual = target.ParseFile(file, lines, increment, callback);
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ParseFile
		///</summary>
		[TestMethod()]
		public void ParseFileTest1()
		{
			ParserService target = new ParserService(); // TODO: Initialize to an appropriate value
			string file = string.Empty; // TODO: Initialize to an appropriate value
			ParserInformation expected = null; // TODO: Initialize to an appropriate value
			ParserInformation actual;
			actual = target.ParseFile(file);
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for Project
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectTest()
		{
			ParserService_Accessor target = new ParserService_Accessor(); // TODO: Initialize to an appropriate value
			IProject expected = null; // TODO: Initialize to an appropriate value
			IProject actual;
			target.Project = expected;
			actual = target.Project;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
