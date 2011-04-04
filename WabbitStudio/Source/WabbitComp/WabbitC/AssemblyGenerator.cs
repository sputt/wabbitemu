using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Types;
using WabbitC.Model.Statements;

namespace WabbitC
{
	static class AssemblyGenerator
	{
		public static string GenerateCode(ref Module module)
		{
			var sb = new StringBuilder();
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				if (functions.Current.Code != null)
				{
					Block block = functions.Current.Code;
					sb.Append(block.ToAssemblyString());
				}
			}
			return sb.ToString();
		}
	}
}
