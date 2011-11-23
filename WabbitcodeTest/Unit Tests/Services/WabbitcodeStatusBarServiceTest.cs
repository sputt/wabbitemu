using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Moq;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for WabbitcodeStatusBarServiceTest and is intended
	///to contain all WabbitcodeStatusBarServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class WabbitcodeStatusBarServiceTest
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

		[TestMethod()]
		public void WabbitcodeStatusBarServiceConstructorTest_Invalid_DockingService()
		{
			IDockingService dockingService = null;
			WabbitcodeStatusBar statusBar = new WabbitcodeStatusBar();
			try
			{
				WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar);
			}
			catch (ArgumentNullException ex)
			{
				Assert.IsTrue(ex.ParamName == "dockingService");
				return;
			}
			Assert.Fail("No Exception thrown");
		}

		[TestMethod()]
		public void WabbitcodeStatusBarServiceConstructorTest_Invalid_StatusBar()
		{
			Mock<IDockingService> dockingService = new Mock<IDockingService>(MockBehavior.Strict);
			WabbitcodeStatusBar statusBar = null;
			try
			{
				WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService.Object, statusBar);
			}
			catch (ArgumentNullException ex)
			{
				Assert.IsTrue(ex.ParamName == "statusBar");
				return;
			}
			Assert.Fail("No Exception thrown");
		}

		/// <summary>
		///A test for HideProgress
		///</summary>
		[TestMethod()]
		public void HideProgressTest()
		{
			IDockingService dockingService = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar); // TODO: Initialize to an appropriate value
			target.HideProgress();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for SetCaretPosition
		///</summary>
		[TestMethod()]
		public void SetCaretPositionTest()
		{
			IDockingService dockingService = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar); // TODO: Initialize to an appropriate value
			int x = 0; // TODO: Initialize to an appropriate value
			int y = 0; // TODO: Initialize to an appropriate value
			int charOffset = 0; // TODO: Initialize to an appropriate value
			target.SetCaretPosition(x, y, charOffset);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for SetInsertMode
		///</summary>
		[TestMethod()]
		public void SetInsertModeTest()
		{
			IDockingService dockingService = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar); // TODO: Initialize to an appropriate value
			bool insertMode = false; // TODO: Initialize to an appropriate value
			target.SetInsertMode(insertMode);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for SetMessage
		///</summary>
		[TestMethod()]
		public void SetMessageTest()
		{
			IDockingService dockingService = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar); // TODO: Initialize to an appropriate value
			string message = string.Empty; // TODO: Initialize to an appropriate value
			bool highlighted = false; // TODO: Initialize to an appropriate value
			target.SetMessage(message, highlighted);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for SetProgress
		///</summary>
		[TestMethod()]
		public void SetProgressTest()
		{
			IDockingService dockingService = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar); // TODO: Initialize to an appropriate value
			double progress = 0F; // TODO: Initialize to an appropriate value
			string name = string.Empty; // TODO: Initialize to an appropriate value
			OperationStatus status = new OperationStatus(); // TODO: Initialize to an appropriate value
			target.SetProgress(progress, name, status);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for ShowProgress
		///</summary>
		[TestMethod()]
		public void ShowProgressTest()
		{
			IDockingService dockingService = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar); // TODO: Initialize to an appropriate value
			target.ShowProgress();
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for Visible
		///</summary>
		[TestMethod()]
		public void VisibleTest()
		{
			IDockingService dockingService = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBar statusBar = null; // TODO: Initialize to an appropriate value
			WabbitcodeStatusBarService target = new WabbitcodeStatusBarService(dockingService, statusBar); // TODO: Initialize to an appropriate value
			bool expected = false; // TODO: Initialize to an appropriate value
			bool actual;
			target.Visible = expected;
			actual = target.Visible;
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}
	}
}
