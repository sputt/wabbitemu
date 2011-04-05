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
							where s.GetType() == typeof(Add) || s.GetType() == typeof(Sub)
							select s;

				foreach (MathStatement op in maths)
				{
					Immediate imm = op.RValue as Immediate;
					if (imm != null)
					{
						Declaration temp = block.CreateTempDeclaration(imm.Type);

						int offset = block.Statements.IndexOf(op);

						if (op.GetType() == typeof(Sub))
						{
							imm.Negate();
							block.Statements.Remove(op);
							block.Statements.Insert(offset, new Add(temp, imm));
						}
						else
						{
							block.Statements.Insert(offset, new Assignment(temp, imm));
							op.RValue = temp;
						}
					}
				}
			}
		}
	}
}
