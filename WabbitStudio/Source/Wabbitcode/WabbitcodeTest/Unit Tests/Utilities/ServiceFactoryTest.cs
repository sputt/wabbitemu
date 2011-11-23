using Revsoft.Wabbitcode;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Utilities;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for ServiceFactoryTest and is intended
	///to contain all ServiceFactoryTest Unit Tests
	///</summary>
	[TestClass()]
	public class ServiceFactoryTest
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
		[ClassInitialize()]
		public static void MyClassInitialize(TestContext testContext)
		{
		}
		
		//Use ClassCleanup to run code after all tests in a class have run
		[ClassCleanup()]
		public static void MyClassCleanup()
		{
		}
		
		#endregion


		/// <summary>
		///A test for GetServiceInstance
		///</summary>
		public void GetServiceInstanceTestHelper<T>()
		{
			object[] objects = null; // TODO: Initialize to an appropriate value
			T expected = default(T); // TODO: Initialize to an appropriate value
			T actual;
			actual = ServiceFactory.Instance.GetServiceInstance<T>(objects);
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void GetServiceInstanceTest()
		{
			GetServiceInstanceTestHelper<GenericParameterHelper>();
		}

		/// <summary>
		///A test for DestroyServiceInstance
		///</summary>
		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void DestroyServiceInstanceTest()
		{
			ServiceFactory target = new ServiceFactory();
			IService serviceToDestroy = null; // TODO: Initialize to an appropriate value
			target.DestroyServiceInstance(serviceToDestroy);
			Assert.Inconclusive("A method that does not return a value cannot be verified.");
		}

		/// <summary>
		///A test for GetServiceInstance
		///</summary>
		public void GetServiceInstanceTest1Helper<T>()
		{
			ServiceFactory target = new ServiceFactory();
			T expected = default(T); // TODO: Initialize to an appropriate value
			T actual;
			actual = target.GetServiceInstance<T>();
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void GetServiceInstanceTest1()
		{
			GetServiceInstanceTest1Helper<GenericParameterHelper>();
		}

		/// <summary>
		///A test for CreateServiceInstance
		///</summary>
		public void CreateServiceInstanceTestHelper<T>()
		{
			ServiceFactory target = new ServiceFactory();
			object[] objects = null; // TODO: Initialize to an appropriate value
			T expected = default(T); // TODO: Initialize to an appropriate value
			T actual;
			actual = target.CreateServiceInstance<T>(objects);
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void CreateServiceInstanceTest()
		{
			CreateServiceInstanceTestHelper<GenericParameterHelper>();
		}

		/// <summary>
		///A test for CreateServiceInstance
		///</summary>
		public void CreateServiceInstanceTest1Helper<T>()
		{
			ServiceFactory target = new ServiceFactory();
			T expected = default(T); // TODO: Initialize to an appropriate value
			T actual;
			actual = target.CreateServiceInstance<T>();
			Assert.AreEqual(expected, actual);
			Assert.Inconclusive("Verify the correctness of this test method.");
		}

		[TestMethod()]
		[DeploymentItem("Wabbitcode.exe")]
		public void CreateServiceInstanceTest1()
		{
			CreateServiceInstanceTest1Helper<GenericParameterHelper>();
		}
	}
}
