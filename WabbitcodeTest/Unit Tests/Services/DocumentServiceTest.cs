using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Panels;
using Moq;
using Revsoft.Wabbitcode.Interface;

namespace WabbitcodeTest
{
	/// <summary>
	///This is a test class for DocumentServiceTest and is intended
	///to contain all DocumentServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class DocumentServiceTest
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

		static DocumentService target;

		#region Additional test attributes
		// 
		//You can use the following additional attributes as you write your tests:
		//
		//Use ClassInitialize to run code before running the first test in the class
		[ClassInitialize()]
		public static void MyClassInitialize(TestContext testContext)
		{
			target = new DocumentService();
			target.InitService();
		}
		
		//Use ClassCleanup to run code after all tests in a class have run
		[ClassCleanup()]
		public static void MyClassCleanup()
		{
			target.DestroyService();
		}
		#endregion

		/// <summary>
		///A test for CreateDocument
		///</summary>
		[TestMethod()]
		public void CreateDocumentTest()
		{
			Mock<IService> pathsService = new Mock<IService>(MockBehavior.Strict);
			pathsService.Setup(p => p.InitService(null)).Verifiable();
			
			string title = "Test Title";
			Editor expected = new Editor() { Title = title };
			Editor actual;
			actual = target.CreateDocument(title);
			Assert.AreEqual(expected.Title, actual.Title);
			Assert.IsTrue(target.OpenDocuments.Count == 1);

			pathsService.Verify();
		}

		/// <summary>
		///A test for OpenDocument
		///</summary>
		[TestMethod()]
		public void OpenDocumentTest1()
		{
			DocumentService target = new DocumentService(); // TODO: Initialize to an appropriate value
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
		public void OpenDocumentTest2()
		{
			DocumentService target = new DocumentService(); // TODO: Initialize to an appropriate value
			Editor doc = null; // TODO: Initialize to an appropriate value
			string filename = string.Empty; // TODO: Initialize to an appropriate value
			target.OpenDocument(doc, filename);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for OpenDocument
		///</summary>
		[TestMethod()]
		public void OpenDocumentTest3()
		{
			DocumentService target = new DocumentService(); // TODO: Initialize to an appropriate value
			Editor doc = null; // TODO: Initialize to an appropriate value
			target.OpenDocument(doc);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}
	}
}
