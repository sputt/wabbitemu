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
					string tabs = "\t";
					Block block = functions.Current.Code;
					var statements = from Statement st in functions.Current.Code
									 select st;
					foreach (var statement in statements)
					{
						var asmString = statement.ToAssemblyString();
						if (statement.GetType() == typeof(Push))
							tabs += "\t";
						else if (statement.GetType() == typeof(Pop))
							tabs = tabs.Remove(tabs.Length - 1);
						sb.Append(tabs);
						asmString = asmString.Replace(Environment.NewLine, Environment.NewLine + tabs);
						sb.AppendLine(asmString);
					}
				}
			}
			return sb.ToString();
		}
	}
}
