using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace WabbitcodeTest
{
	
	
	/// <summary>
	///This is a test class for ResourceServiceTest and is intended
	///to contain all ResourceServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class ResourceServiceTest
	{
		/// <summary>
		///A test for GetResource
		///</summary>
		[TestMethod()]
		public void GetResourceTest()
		{
			string name = "Templates.xml"; // TODO: Initialize to an appropriate value
			string expected = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
			string actual;
			actual = ResourceService.GetResource(name);
			Assert.IsTrue(actual.StartsWith(expected));
		}
	}
}
