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
	class ConvertAddSubToIncDec
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
						int immValue = int.Parse(imm.ToString());
						if (immValue <= 4)
						{
							int offset = block.Statements.IndexOf(op);
							block.Statements.Remove(op);
							for (int i = 0; i < immValue; i++)
							{
								Statement replacement = null;
								if (op.GetType() == typeof(Sub))
								{
									replacement = new Dec(op.LValue);
								}
								else
								{
									replacement = new Inc(op.LValue);
								}
								block.Statements.Insert(offset, replacement);
							}
						}
					}
				}
			}
		}
	}
}
