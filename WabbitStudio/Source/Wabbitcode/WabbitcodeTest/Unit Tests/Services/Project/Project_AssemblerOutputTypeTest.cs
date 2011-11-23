using Revsoft.Wabbitcode.Services.Project;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for Project_AssemblerOutputTypeTest and is intended
	///to contain all Project_AssemblerOutputTypeTest Unit Tests
	///</summary>
	[TestClass()]
	public class Project_AssemblerOutputTypeTest
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
		///A test for ToString
		///</summary>
		[TestMethod()]
		public void ToStringTest()
		{
			string ext = ".8xp";
			Project.AssemblerOutputType target = new Project.AssemblerOutputType(ext);
			string expected = ".8xp";
			string actual;
			actual = target.ToString();
			Assert.AreEqual(expected, actual);
		}
	}
}
