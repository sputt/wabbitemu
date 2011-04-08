using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using WabbitC.Model.Types;

namespace WabbitC.Optimizer
{
	static class RemoveUselessMath
	{
		public static void Optimize(Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;

				var maths = from Statement s in block
							where s.GetType() == typeof(Mult) || s.GetType() == typeof(Div)
							select s;

				foreach (MathStatement op in maths)
				{
					var imm = op.RValue as Immediate;
					var type = op.GetType();
					if (imm != null && imm.Value == Tokenizer.ToToken("1"))
					{
						block.Statements.Remove(op);
					}
				}
			}
		}
	}
}
