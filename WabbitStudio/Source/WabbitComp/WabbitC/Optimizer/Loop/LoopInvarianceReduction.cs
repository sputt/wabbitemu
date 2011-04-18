using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;

namespace WabbitC.Optimizer.Loop
{
	static class LoopInvarianceReduction
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
			var statements = from Statement st in block where st.GetType() == typeof(While) select st;
			foreach (While statement in statements)
			{
				OptimizeLoopBlock(ref statement.Block);
			}
		}

		public static bool OptimizeLoopBlock(ref Block block)
		{
			List<bool> variantDecls = new List<bool>();
			var statements = from Statement st in block select st;
			foreach (var statement in statements)
			{


			}
		}
	}
}
