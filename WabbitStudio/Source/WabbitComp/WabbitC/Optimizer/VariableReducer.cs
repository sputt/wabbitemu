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
                var type = statement.GetType();
				if (type == typeof(Annotation) || type == typeof(Assignment) || type == typeof(Label))
				{
					continue;
				}
				else if (type == typeof(ReturnMove))
				{
					var retMove = statement as ReturnMove;
					if (retMove.RValue == block.FindDeclaration("__hl"))
						block.Statements.Remove(statement);
				}
				else if (type == typeof(Move))
				{
					var move = statement as Move;
					if (move.RValue.ConstStatement != null)
					{
						var newStatement = move.RValue.ConstStatement;
						var lValue = newStatement.GetModifiedDeclarations()[0];
						//block.Statements.Remove(newStatement);
						block.Statements.Remove(move);
						newStatement.ReplaceDeclaration(lValue, move.LValue);
						move.LValue.ConstStatement = newStatement;
						i--;
					}
					else
					{
						move.LValue.ConstStatement = move;
					}
				}
				else if (type.BaseType == typeof(ConditionStatement))
				{
					var cond = statement as ConditionStatement;
					cond.LValue.ConstStatement = cond;
				}
				else if (type.BaseType == typeof(MathStatement))
				{
					var math = statement as MathStatement;
					var refed = math.RValue as Declaration;
					if (refed != null)
					{
						var newStatement = refed.ConstStatement;
						if (newStatement != null && newStatement.GetType() == typeof(Move))
						{
							math.ReplaceDeclaration(refed, newStatement.GetReferencedDeclarations()[0]);
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
							if (type == typeof(Not))
							{
								if (newStatement.GetType().BaseType == typeof(ConditionStatement))
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
								var decl = math.LValue.ConstStatement.GetReferencedDeclarations()[0];
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
							}
						}
						else
						{
							math.LValue.ConstStatement = null;
						}
					}
				}
				else if (type == typeof(FunctionCall))
				{
					var funcCall = statement as FunctionCall;
					for (int j = 0; j < funcCall.Params.Count; j++)
					{
						var param = funcCall.Params[j];
						if (param.ConstStatement != null)
						{
							if (param.ConstStatement.GetType() == typeof(Move))
							{
								param = (param.ConstStatement as Move).RValue;
							}
						}
						funcCall.Params[j] = param;
					}
					funcCall.LValue.ConstStatement = funcCall;
				}
				else if (type == typeof(Goto))
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
						block.Declarations[k].ConstStatement = null;
					/*else
					{
						j = i + 1;
						while (j + 1 < block.Statements.Count && statement != gotoType.TargetLabel)
							statement = block.Statements[++j];
						if (j + 1 != block.Statements.Count)
							i = j;
					}*/
				}
				else if (type == typeof(Return))
				{
					var returnStatement = statement as Return;
					var returnVal = returnStatement.ReturnReg as Declaration;
					if (returnVal.ConstStatement != null && returnVal.ConstStatement.GetType() == typeof(Move))
					{
						var newStatement = returnVal.ConstStatement;
						var lValue = newStatement.GetModifiedDeclarations()[0];
						returnStatement.ReplaceDeclaration(lValue, newStatement.GetReferencedDeclarations()[0]);
						//block.Statements.Remove(newStatement);
					}
				}
            }
        }

    }
}
