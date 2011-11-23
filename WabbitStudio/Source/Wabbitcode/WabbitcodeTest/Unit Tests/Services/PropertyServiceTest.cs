using Revsoft.Wabbitcode.Services;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;

namespace WabbitcodeTest
{
	/// <summary>
	///This is a test class for PropertyServiceTest and is intended
	///to contain all PropertyServiceTest Unit Tests
	///</summary>
	[TestClass()]
	public class PropertyServiceTest
	{
		/// <summary>
		///A test for GetEditorProperty
		///</summary>
		[TestMethod]
		public void EditorPropertyTest()
		{
			string pName = "WordWrap";
			object expected = true;
			PropertyService.SaveEditorProperty(pName, expected);
			object actual = PropertyService.GetEditorProperty(pName);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for GetWabbitcodeProperty
		///</summary>
		[TestMethod]
		public void WabbitcodePropertyTest()
		{
			string pName = "Top";
			object expected = 25.0;
			PropertyService.SaveWabbitcodeProperty(pName, expected);
			object actual = PropertyService.GetWabbitcodeProperty(pName);
			Assert.AreEqual(expected, actual);
		}

		/// <summary>
		///A test for Save
		///</summary>
		[TestMethod]
		public void SaveTest()
		{
			try
			{
				PropertyService.Save();
			}
			catch (Exception e)
			{
				Assert.Fail(e.ToString());
			}
		}
	}
}
