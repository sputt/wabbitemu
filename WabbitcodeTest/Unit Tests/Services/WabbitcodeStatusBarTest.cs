using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Windows.Controls.Primitives;

namespace WabbitcodeTest
{
    
    
    /// <summary>
    ///This is a test class for WabbitcodeStatusBarTest and is intended
    ///to contain all WabbitcodeStatusBarTest Unit Tests
    ///</summary>
	[TestClass()]
	public class WabbitcodeStatusBarTest
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
		///A test for WabbitcodeStatusBar Constructor
		///</summary>
		[TestMethod()]
		public void WabbitcodeStatusBarConstructorTest()
		{
			WabbitcodeStatusBar target = new WabbitcodeStatusBar();
			Assert.Inconclusive("TODO: Implement code to verify target");
		}

		/// <summary>
		///A test for DisplayProgress
		///</summary>
		[TestMethod()]
		public void DisplayProgressTest()
		{
			WabbitcodeStatusBar target = new WabbitcodeStatusBar(); // TODO: Initialize to an appropriate value
			string taskName = string.Empty; // TODO: Initialize to an appropriate value
			double workDone = 0F; // TODO: Initialize to an appropriate value
			OperationStatus status = new OperationStatus(); // TODO: Initialize to an appropriate value
			target.DisplayProgress(taskName, workDone, status);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for DoHideProgress
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void DoHideProgressTest()
		{
			WabbitcodeStatusBar_Accessor target = new WabbitcodeStatusBar_Accessor(); // TODO: Initialize to an appropriate value
			target.DoHideProgress();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for HideProgress
		///</summary>
		[TestMethod()]
		public void HideProgressTest()
		{
			WabbitcodeStatusBar target = new WabbitcodeStatusBar(); // TODO: Initialize to an appropriate value
			target.HideProgress();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for SetMessage
		///</summary>
		[TestMethod()]
		public void SetMessageTest()
		{
			WabbitcodeStatusBar target = new WabbitcodeStatusBar(); // TODO: Initialize to an appropriate value
			string message = string.Empty; // TODO: Initialize to an appropriate value
			bool highlighted = false; // TODO: Initialize to an appropriate value
			target.SetMessage(message, highlighted);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for StopHideProgress
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void StopHideProgressTest()
		{
			WabbitcodeStatusBar_Accessor target = new WabbitcodeStatusBar_Accessor(); // TODO: Initialize to an appropriate value
			target.StopHideProgress();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for CursorStatusBarPanel
		///</summary>
		[TestMethod()]
		public void CursorStatusBarPanelTest()
		{
			WabbitcodeStatusBar target = new WabbitcodeStatusBar(); // TODO: Initialize to an appropriate value
			StatusBarItem actual;
			actual = target.CursorStatusBarPanel;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		/// <summary>
		///A test for ModeStatusBarPanel
		///</summary>
		[TestMethod()]
		public void ModeStatusBarPanelTest()
		{
			WabbitcodeStatusBar target = new WabbitcodeStatusBar(); // TODO: Initialize to an appropriate value
			StatusBarItem actual;
			actual = target.ModeStatusBarPanel;
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
