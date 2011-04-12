using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;

namespace WabbitC.Optimizer.Loop
{
	static class StrengthReduction
	{
		public static void Optimize(ref Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				OptimizeBlock(ref block);
			}
		}

		public static void OptimizeBlock(ref Block block)
		{
			var statements = from Statement st in block select st;
			foreach (var statement in statements)
			{
				int index = block.Statements.IndexOf(statement);
				block.Statements.Remove(statement);
				var newStatements = new List<Statement>();

				//actual optimization code

				newStatements.Add(statement);
				block.Statements.InsertRange(index, newStatements);
			}
		}
	}
}
