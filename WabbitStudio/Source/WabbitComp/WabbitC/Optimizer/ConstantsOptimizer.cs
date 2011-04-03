using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using System.Diagnostics;

namespace WabbitC
{
    static class ConstantsOptimizer
    {
        static List<OptimizerSymbol> symbolTable = new List<OptimizerSymbol>();
        public static void Optimize(ref Block module)
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
            for (int i = 0; i < block.Declarations.Count; i++)
            {
                symbolTable.Add(OptimizerSymbol.Parse(block.Declarations[i]));
            }
            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
                System.Type type = statement.GetType();
				if (type == typeof(Annotation))
				{
					continue;
				}
                else if (type == typeof(Assignment))
                {
                    var assignment = statement as Assignment;
					assignment.LValue.ConstValue = new Immediate(assignment.RValue.Value);
                }
                else if (type == typeof(Move))
                {
                    var move = statement as Move;
                    if (move.RValue.ConstValue != null)
                    {
                        var immediate = new Immediate(move.RValue.ConstValue.Value);
                        var assigment = new Assignment(move.LValue, immediate);
                        block.Statements[i] = assigment;
						move.LValue.ConstValue = immediate;
                    }
                    else
                    {
						move.LValue.ConstValue = null;
                    }
                }
				else if (type.BaseType == typeof(ConditionStatement))
				{
					var cond = statement as ConditionStatement;
					if (cond.CondDecl.ConstValue != null)
					{
						var immediate = new Immediate(cond.LValue.ConstValue.Value);
						
					}
					else
					{
						cond.LValue.ConstValue = null;
					}
				}
				else if (type.BaseType == typeof(MathStatement))
				{
					var math = statement as MathStatement;
					var imath = statement as IMathOperator;
					var refedSymbols = math.GetReferencedDeclarations();
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
				/*else if (type == typeof(Return))
				{
					var returnType = statement as Return;
					var symbol = FindSymbol((Declaration)returnType.ReturnReg);
					if (symbol != null && symbol.IsConstant)
					{
						//returnType = new Return(symbol.Value);
						//block.Statements[i] = returnType;
					}
					else
					{

					}
				}*/
				else if (type == typeof(Goto))
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
        }
    }


    class OptimizerSymbol
    {
        public string Name { get; set; }
        public bool IsAlive { get; set; }

        public static OptimizerSymbol Parse(Declaration statement)
        {
            var symbol = new OptimizerSymbol();
            symbol.Name = statement.Name;
            return symbol;
        }

        public override bool Equals(object obj)
        {
            if (obj == null)
                return base.Equals(obj);
            var symbol = obj as OptimizerSymbol;
            return symbol.Name == this.Name;
        }

        public override int GetHashCode()
        {
            return Name.GetHashCode();
        }

        public override string ToString()
        {
            return Name;
        }
    }
}
