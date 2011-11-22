using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using System.Diagnostics;

namespace WabbitC.Optimizer.Loop
{
	static class ConstantsOptimizer
	{
		public static void Optimize(ref Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				var loops = block.Statements.Where(l => l is While || l is DoWhile);
				foreach (var loop in loops)
					OptimizeBlock(ref loop.Block);
			}
		}

		const string constOptString = "Optimized";
		const string constOptInProgressString = "Optimizing";
		public static void OptimizeBlock(ref Block block)
		{
			for (int i = 0; i < block.Statements.Count; i++)
			{
				var statement = block.Statements[i];
				if (statement is Annotation || statement is Label || statement is Goto)
				{
					continue;
				}
				else if (statement is Assignment)
				{
					var assignment = statement as Assignment;
					assignment.LValue.ConstValue = new Immediate(((Immediate) assignment.RValue).Value);
				}
				else if (statement is Move)
				{
					var move = statement as Move;
					if (((Declaration)move.RValue).ConstValue != null)
					{
						var immediate = new Immediate(((Declaration)move.RValue).ConstValue.Value);
						var assigment = new Assignment(move.LValue, immediate);
						block.Statements[i] = assigment;
						move.LValue.ConstValue = immediate;
					}
					else
					{
						move.LValue.ConstValue = null;
					}
				}
				else if (statement is ConditionStatement)
				{
					var cond = statement as ConditionStatement;
					if (cond.CondDecl.ConstValue != null)
					{
						//var immediate = new Immediate(cond.LValue.ConstValue.Value);

					}
					else
					{
						cond.LValue.ConstValue = null;
					}
				}
				else if (statement is MathStatement)
				{
					var math = statement as MathStatement;
					var imath = statement as IMathOperator;
					var refedSymbols = math.GetReferencedDeclarations().ToList();
					var refed = refedSymbols.Count > 1 ? refedSymbols[1] : refedSymbols[0];
					if (math.LValue.ConstValue != null || refed.ConstValue != null)
					{
						var value = imath.Apply();
						if (value != null)
						{
							var lValue = math.LValue;
							var immediate = value as Immediate;
							lValue.ConstValue = immediate;
							if (immediate.Value.Type == TokenType.IntType)
							{
								int result;
								result = int.Parse(immediate.Value);
								var newImmediate = new Immediate(Tokenizer.ToToken(result.ToString()));
								var assigment = new Assignment(lValue, newImmediate);
								block.Statements[i] = assigment;
							}
						}
						else
						{
							math.LValue.ConstValue = null;
						}
					}
				}
				else if (statement is Goto)
				{
					var gotoType = statement as Goto;
					if (gotoType.CondDecl != null)
					{
						for (int k = 0; k < block.Declarations.Count; k++)
						{
							block.Declarations[k].ConstValue = null;
						}
					}
					int j = i;
					while (j + 1 < block.Statements.Count && statement != gotoType.TargetLabel)
						statement = block.Statements[++j];
					if (j != block.Statements.Count)
						i = j;
				}
			}
			//mark that were all done with this one
			block.Properties.Remove(constOptInProgressString);
			block.Properties.Add(constOptString);
			/*for (int i = 0; i < block.Declarations.Count; i++)
			{
				block.Declarations[i].ConstValue = null;
			}*/
		}
	}
}
