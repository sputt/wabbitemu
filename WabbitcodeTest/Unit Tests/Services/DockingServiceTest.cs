using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using AvalonDock;
using Revsoft.Wabbitcode.Panels;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode;

namespace WabbitcodeTest
{
    
    
    /// <summary>
    ///This is a test class for DockingServiceTest and is intended
    ///to contain all DockingServiceTest Unit Tests
    ///</summary>
	[TestClass()]
	public class DockingServiceTest
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
		///A test for DockingService Constructor
		///</summary>
		[TestMethod()]
		public void DockingServiceConstructorTest()
		{
			DockingService target = new DockingService();
			Assert.Inconclusive("TODO: Implement code to verify target");
		}

		/// <summary>
		///A test for DeserializationCallback
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void DeserializationCallbackTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			object s = null; // TODO: Initialize to an appropriate value
			DeserializationCallbackEventArgs e = null; // TODO: Initialize to an appropriate value
			target.DeserializationCallback(s, e);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for DestroyService
		///</summary>
		[TestMethod()]
		public void DestroyServiceTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			target.DestroyService();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for InitPanels
		///</summary>
		[TestMethod()]
		public void InitPanelsTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			target.InitPanels(statusBar);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for InitService
		///</summary>
		[TestMethod()]
		public void InitServiceTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			object[] objects = null; // TODO: Initialize to an appropriate value
			target.InitService(objects);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ShowDockPanel
		///</summary>
		[TestMethod()]
		public void ShowDockPanelTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			DocumentContent doc = null; // TODO: Initialize to an appropriate value
			target.ShowDockPanel(doc);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ShowDockPanel
		///</summary>
		[TestMethod()]
		public void ShowDockPanelTest1()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			DockableContent doc = null; // TODO: Initialize to an appropriate value
			target.ShowDockPanel(doc);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ShowError
		///</summary>
		[TestMethod()]
		public void ShowErrorTest()
		{
			string error = string.Empty; // TODO: Initialize to an appropriate value
			DockingService.ShowError(error);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ShowError
		///</summary>
		[TestMethod()]
		public void ShowErrorTest1()
		{
			string error = string.Empty; // TODO: Initialize to an appropriate value
			Exception ex = null; // TODO: Initialize to an appropriate value
			DockingService.ShowError(error, ex);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ActiveContent
		///</summary>
		[TestMethod()]
		public void ActiveContentTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			ManagedContent actual;
			actual = target.ActiveContent;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ActiveDocument
		///</summary>
		[TestMethod()]
		public void ActiveDocumentTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			Editor actual;
			actual = target.ActiveDocument;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ActivePanel
		///</summary>
		[TestMethod()]
		public void ActivePanelTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			IWabbitcodePanel actual;
			actual = target.ActivePanel;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for DockManager
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void DockManagerTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			DockingManager expected = null; // TODO: Initialize to an appropriate value
			DockingManager actual;
			target.DockManager = expected;
			actual = target.DockManager;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for Documents
		///</summary>
		[TestMethod()]
		public void DocumentsTest()
		{
			DockingService target = new DockingService(); // TODO: Initialize to an appropriate value
			ManagedContentCollection<DocumentContent> actual;
			actual = target.Documents;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ErrorList
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ErrorListTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			ErrorList expected = null; // TODO: Initialize to an appropriate value
			ErrorList actual;
			target.ErrorList = expected;
			actual = target.ErrorList;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for LabelList
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void LabelListTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			LabelList expected = null; // TODO: Initialize to an appropriate value
			LabelList actual;
			target.LabelList = expected;
			actual = target.LabelList;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for MainWindow
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void MainWindowTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			MainWindow expected = null; // TODO: Initialize to an appropriate value
			MainWindow actual;
			target.MainWindow = expected;
			actual = target.MainWindow;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for OutputWindow
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void OutputWindowTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			OutputWindow expected = null; // TODO: Initialize to an appropriate value
			OutputWindow actual;
			target.OutputWindow = expected;
			actual = target.OutputWindow;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ProjectViewer
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectViewerTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			ProjectViewer expected = null; // TODO: Initialize to an appropriate value
			ProjectViewer actual;
			target.ProjectViewer = expected;
			actual = target.ProjectViewer;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for StatusBar
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void StatusBarTest()
		{
			DockingService_Accessor target = new DockingService_Accessor(); // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService expected = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService actual;
			target.StatusBar = expected;
			actual = target.StatusBar;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
