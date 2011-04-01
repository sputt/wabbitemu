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
            var symbolTable = new List<OptimizerSymbol>();
			var gotoTable = new Hashtable();
            foreach (var decl in block.Declarations)
            {
                var symbol = OptimizerSymbol.Parse(decl);
                symbol.IsConstant = false;
                symbolTable.Add(symbol);
            }
            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
                var type = statement.GetType();
				if (type == typeof(Annotation) || type == typeof(Assignment) || type == typeof(Label))
				{
					continue;
				}
                else if (type == typeof(Move))
                {
                    var move = statement as Move;
					var symbol = FindSymbol(move.RValue, ref symbolTable);
					if (symbol != null && symbol.ConstStatment != null)
					{
						var newStatement = symbol.ConstStatment;
						var lValue = newStatement.GetModifiedDeclarations()[0];
						block.Statements.Remove(newStatement);
						block.Statements.Remove(move);
						newStatement.ReplaceDeclaration(lValue, move.LValue);
						block.Statements.Insert(--i, newStatement);
						symbol = FindSymbol(move.LValue, ref symbolTable);
						symbol.ConstStatment = newStatement;
					}
					else
					{
						symbol = FindSymbol(move.LValue, ref symbolTable);
						symbol.ConstStatment = move;
					}
                }
                else if (type.BaseType == typeof(ConditionStatement))
                {
                    var cond = statement as ConditionStatement;
					var symbol = FindSymbol(cond.LValue, ref symbolTable);
					symbol.ConstStatment = cond;
                }
                else if (type.BaseType == typeof(MathStatement))
                {
                    var math = statement as MathStatement;
					var symbol = FindSymbol(math.RValue as Declaration, ref symbolTable);
					if (symbol != null && symbol.ConstStatment != null)
					{
						var newStatement = symbol.ConstStatment;
						var lValue = newStatement.GetModifiedDeclarations()[0];
						if (newStatement.GetType() == typeof(Move))
						{
							math.ReplaceDeclaration(lValue, newStatement.GetReferencedDeclarations()[0]);
							block.Statements[i] = math;
							//block.Statements.Remove(newStatement);
						}
						symbol = FindSymbol(math.LValue, ref symbolTable);
						symbol.ConstStatment = null;
					}
					else
					{
						symbol = FindSymbol(math.LValue, ref symbolTable);
						if (symbol != null && symbol.ConstStatment != null)
						{
							var newStatement = symbol.ConstStatment;
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
								var decl = symbol.ConstStatment.GetReferencedDeclarations()[0];
								var replaceSymbol = FindSymbol(decl, ref symbolTable);
								if (replaceSymbol != null)
								{
									math.ReplaceDeclaration(math.LValue, decl);
									//block.Statements.Remove(newStatement);
									//i--;
								}
								else
								{
									symbol.ConstStatment = null;
								}
							}
						}
						else
						{
							if (symbol != null)
								symbol.ConstStatment = null;
						}
					}
                }
                else if (type == typeof(FunctionCall))
                {
                    var funcCall = statement as FunctionCall;
					OptimizerSymbol symbol;
					for (int j = 0; j < funcCall.Params.Count; j++)
					{
						var param = funcCall.Params[j];
						symbol = FindSymbol(param as Declaration, ref symbolTable);
						if (symbol != null && symbol.ConstStatment != null)
						{
							if (symbol.ConstStatment.GetType() == typeof(Move))
							{
								param = (symbol.ConstStatment as Move).RValue;
							}
						}
						funcCall.Params[j] = param;
					}
					symbol = FindSymbol(funcCall.LValue, ref symbolTable);
					symbol.ConstStatment = funcCall;
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
						symbolTable[k].ConstStatment = null;
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
					var symbol = FindSymbol(returnVal, ref symbolTable);
					if (symbol != null && symbol.ConstStatment != null && symbol.ConstStatment.GetType() == typeof(Move))
					{
						var newStatement = symbol.ConstStatment;
						var lValue = newStatement.GetModifiedDeclarations()[0];
						returnStatement.ReplaceDeclaration(lValue, newStatement.GetReferencedDeclarations()[0]);
						block.Statements[i--] = returnStatement;
						block.Statements.Remove(newStatement);
					}
				}
            }
        }

        static OptimizerSymbol FindSymbol(Declaration decl, ref List<OptimizerSymbol> symbolTable)
        {
			if (decl == null)
				return null;
            var symbolToFind = OptimizerSymbol.Parse(decl);
            var index = symbolTable.IndexOf(symbolToFind);
            if (index == -1)
                return null;
            return symbolTable[index];
        }
    }
}
