using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
	class RemoveMathImmediates
	{
		public static void Run(Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;

				var maths = from Statement s in block
							where s is Add || s is Sub 
							select s;
				var conds = from Statement s in block
							where s is ConditionStatement
							select s;

				foreach (MathStatement op in maths)
				{
					Immediate imm = op.RValue as Immediate;
					if (imm != null)
					{
						Declaration temp = block.CreateTempDeclaration(imm.Type);

						int offset = block.Statements.IndexOf(op);

						if (op is Sub)
						{
							imm.Negate();
							block.Statements.Remove(op);
							block.Statements.Insert(offset, new Add(temp, imm));
						}
						block.Statements.Insert(offset, new Assignment(temp, imm));
						op.RValue = temp;
					}
				}

				foreach (ConditionStatement cond in conds)
				{
					if (cond.CondValue is Immediate)
					{
						Immediate imm = (Immediate)cond.CondValue;
						Declaration temp = block.CreateTempDeclaration(imm.Type);

						int offset = block.Statements.IndexOf(cond);
						block.Statements.Insert(offset, new Assignment(temp, imm));
						cond.CondValue = temp;
					}
				}
			}
		}
	}
}
