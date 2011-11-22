using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using System.Diagnostics;
using System.Collections;

namespace WabbitC.Optimizer
{
	static class VariableReducer
	{
		public static void Optimize(ref Module module)
		{
			for (int i = 0; i < module.Declarations.Count; i++)
			{
				Declaration decl = module.Declarations[i];
				if (decl.Code != null)
					OptimizeBlock(ref decl.Code);
			}
		}

		public static void OptimizeBlock(ref Block block)
		{
			var gotoTable = new Hashtable();
			for (int i = 0; i < block.Statements.Count; i++)
			{
				var statement = block.Statements[i];
				if (statement is Annotation || statement is Assignment || statement is Label)
				{
					continue;
				}
				else if (statement is ReturnMove)
				{
					var retMove = statement as ReturnMove;
					if (retMove.RValue == block.FindDeclaration("__hl"))
						block.Statements.Remove(statement);
				}
				else if (statement is Move)
				{
					var move = statement as Move;
					if (((Declaration)move.RValue).ConstStatement != null)
					{
						var newStatement = (Statement)((Declaration)move.RValue).ConstStatement.Clone();
						newStatement.ReplaceDeclaration((Declaration)move.RValue, move.LValue);
						block.Statements[i] = newStatement;
						i--;	//rerun current statement to correctly populate LValue const statement
					}
					else
					{
						move.LValue.ConstStatement = move;
					}
				}
				else if (statement is ConditionStatement)
				{
					var cond = statement as ConditionStatement;
					cond.LValue.ConstStatement = cond;
				}
				else if (statement is MathStatement)
				{
					var math = statement as MathStatement;
					var refed = math.RValue as Declaration;
					if (refed != null)
					{
						var newStatement = refed.ConstStatement;
						if (newStatement != null && newStatement is Move)
						{
							math.ReplaceDeclaration(refed, newStatement.GetReferencedDeclarations().First());
							block.Statements[i] = math;
							//block.Statements.Remove(newStatement);
						}
						math.LValue.ConstStatement = null;
					}
					else
					{
						if (math.LValue.ConstStatement != null)
						{
							var newStatement = math.LValue.ConstStatement;
							if (statement is Not)
							{
								if (newStatement is ConditionStatement)
								{
									var newBlock = new Block(block);
									var cond = newStatement as ConditionStatement;
									var decl = ConditionStatement.BuildStatements(newBlock, new Expression(cond.GetInverseOperator()),
										Tokenizer.ToToken(cond.CondDecl.ToString()), Tokenizer.ToToken(cond.CondValue.ToString()));
									newBlock.Statements[0].ReplaceDeclaration(decl, cond.LValue);
									block.Statements.Remove(newStatement);
									block.Statements.Remove(math);
									block.Statements.InsertRange(i - 1, newBlock.Statements);
								}
							}
							else
							{
								/*var decl = math.LValue.ConstStatement.GetReferencedDeclarations()[0];
								if (decl.ConstStatement == null)
								{
									math.LValue.ConstStatement = null;
								}
								/*else
								{
									math.ReplaceDeclaration(math.LValue, decl);
									//block.Statements.Remove(newStatement);
									//i--;
								}*/
								math.LValue.ConstStatement = null;
							}
						}
						else
						{
							math.LValue.ConstStatement = null;
						}
					}
				}
				else if (statement is FunctionCall)
				{
					var funcCall = statement as FunctionCall;
					for (int j = 0; j < funcCall.Params.Count; j++)
					{
						var param = funcCall.Params[j];
						if (param.ConstStatement != null)
						{
							if (param.ConstStatement is Move)
							{
								param = (Declaration) (param.ConstStatement as Move).RValue;
							}
						}
						funcCall.Params[j] = param;
					}
					funcCall.LValue.ConstStatement = funcCall;
				}
				else if (statement is Goto)
				{
					var gotoType = statement as Goto;
					int j;
					if (gotoType.CondDecl != null)
					{
						if (!gotoTable.Contains(i))
						{
							gotoTable.Add(i, true);
							j = i - 1;
							//we really only care if were going backwards
							while (j > 0 && statement != gotoType.TargetLabel)
								statement = block.Statements[--j];
							if (j != 0)
								i = j;
						}
					}
					for (int k = 0; k < block.Declarations.Count; k++)
					{
						block.Declarations[k].ConstStatement = null;
					}
					/*else
					{
						j = i + 1;
						while (j + 1 < block.Statements.Count && statement != gotoType.TargetLabel)
							statement = block.Statements[++j];
						if (j + 1 != block.Statements.Count)
							i = j;
					}*/
				}
				else if (statement is Return)
				{
					var returnStatement = statement as Return;
					var returnVal = returnStatement.ReturnReg as Declaration;
					if (returnVal.ConstStatement != null && returnVal.ConstStatement is Move)
					{
						var newStatement = returnVal.ConstStatement;
						var lValue = newStatement.GetModifiedDeclarations().First();
						returnStatement.ReplaceDeclaration(lValue, newStatement.GetReferencedDeclarations().First());
						//block.Statements.Remove(newStatement);
					}
				}
			}
		}

	}
}
