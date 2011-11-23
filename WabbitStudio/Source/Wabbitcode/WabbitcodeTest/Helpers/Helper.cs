using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.IO;

namespace WabbitcodeTest.Helpers
{
	class Helper
	{
		public static Stream GetResource(string name)
		{
			var asm = Assembly.GetExecutingAssembly();
			var stream = asm.GetManifestResourceStream("WabbitcodeTest.Resources." + name);
			return stream;
		}
	}
}
