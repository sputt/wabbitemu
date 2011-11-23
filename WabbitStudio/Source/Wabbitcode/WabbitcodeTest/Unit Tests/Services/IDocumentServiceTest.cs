using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Panels;

namespace WabbitcodeTest
{
    
    
    /// <summary>
    ///This is a test class for IDocumentServiceTest and is intended
    ///to contain all IDocumentServiceTest Unit Tests
    ///</summary>
	[TestClass()]
	public class IDocumentServiceTest
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


		internal virtual IDocumentService_Accessor CreateIDocumentService_Accessor()
		{
			// TODO: Instantiate an appropriate concrete class.
			IDocumentService_Accessor target = null;
			return target;
		}

		/// <summary>
		///A test for CreateDocument
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void CreateDocumentTest()
		{
			IDocumentService_Accessor target = CreateIDocumentService_Accessor(); // TODO: Initialize to an appropriate value
			string title = string.Empty; // TODO: Initialize to an appropriate value
			Editor expected = null; // TODO: Initialize to an appropriate value
			Editor actual;
			actual = target.CreateDocument(title);
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for OpenDocument
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void OpenDocumentTest()
		{
			IDocumentService_Accessor target = CreateIDocumentService_Accessor(); // TODO: Initialize to an appropriate value
			string filename = string.Empty; // TODO: Initialize to an appropriate value
			Editor expected = null; // TODO: Initialize to an appropriate value
			Editor actual;
			actual = target.OpenDocument(filename);
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for OpenDocument
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void OpenDocumentTest1()
		{
			IDocumentService_Accessor target = CreateIDocumentService_Accessor(); // TODO: Initialize to an appropriate value
			Editor doc = null; // TODO: Initialize to an appropriate value
			target.OpenDocument(doc);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for OpenDocument
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void OpenDocumentTest3()
		{
			IDocumentService_Accessor target = CreateIDocumentService_Accessor(); // TODO: Initialize to an appropriate value
			Editor doc = null; // TODO: Initialize to an appropriate value
			string filename = string.Empty; // TODO: Initialize to an appropriate value
			target.OpenDocument(doc, filename);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}
	}
}
