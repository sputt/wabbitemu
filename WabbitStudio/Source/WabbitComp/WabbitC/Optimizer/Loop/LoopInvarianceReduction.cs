using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;

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
				//actual optimization code
				bool hasChanged;
				do
				{
					hasChanged = false;
					hasChanged |= CSE.OptimizeBlock(ref statement.Block);
					VariableReuse.OptimizeBlock(ref statement.Block);
					DeadCodeOptimizer.OptimizeBlock(ref statement.Block);
				}
				while (hasChanged);
				DeadVariableOptimizer.OptimizeBlock(ref statement.Block);
			}
		}
	}
}
