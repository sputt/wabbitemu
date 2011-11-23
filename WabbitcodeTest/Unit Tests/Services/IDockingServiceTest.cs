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
    ///This is a test class for IDockingServiceTest and is intended
    ///to contain all IDockingServiceTest Unit Tests
    ///</summary>
	[TestClass()]
	public class IDockingServiceTest
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


		internal virtual IDockingService_Accessor CreateIDockingService_Accessor()
		{
			// TODO: Instantiate an appropriate concrete class.
			IDockingService_Accessor target = null;
			return target;
		}

		/// <summary>
		///A test for InitPanels
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void InitPanelsTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			target.InitPanels(statusBar);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ShowDockPanel
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ShowDockPanelTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			DocumentContent newFile = null; // TODO: Initialize to an appropriate value
			target.ShowDockPanel(newFile);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ShowDockPanel
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ShowDockPanelTest1()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			DockableContent newFile = null; // TODO: Initialize to an appropriate value
			target.ShowDockPanel(newFile);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ActiveContent
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ActiveContentTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			ManagedContent actual;
			actual = target.ActiveContent;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ActiveDocument
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ActiveDocumentTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			Editor actual;
			actual = target.ActiveDocument;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ActivePanel
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ActivePanelTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			IWabbitcodePanel actual;
			actual = target.ActivePanel;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for Documents
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void DocumentsTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
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
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			ErrorList actual;
			actual = target.ErrorList;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for LabelList
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void LabelListTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			LabelList actual;
			actual = target.LabelList;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for MainWindow
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void MainWindowTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			MainWindow actual;
			actual = target.MainWindow;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for OutputWindow
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void OutputWindowTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			OutputWindow actual;
			actual = target.OutputWindow;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ProjectViewer
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void ProjectViewerTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			ProjectViewer actual;
			actual = target.ProjectViewer;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for StatusBar
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void StatusBarTest()
		{
			IDockingService_Accessor target = CreateIDockingService_Accessor(); // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService actual;
			actual = target.StatusBar;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
