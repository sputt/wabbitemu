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
                int intTest;
                var statement = block.Statements[i];
                System.Type type = statement.GetType();
                if (type == typeof(If))
                {
                    var trueBlock = (statement as If).TrueCase;
                    OptimizeBlock(ref trueBlock);
                    var falseBlock = (statement as If).FalseCase;
                    OptimizeBlock(ref falseBlock);
                }
                else if (type == typeof(Assignment))
                {
                    var assignment = statement as Assignment;
                    var symbol = FindSymbol(assignment.LValue);
                    symbol.IsConstant = int.TryParse(assignment.RValue.Value, out intTest);
                    symbol.Value = Datum.Parse(block, assignment.RValue.Value);
                    symbol.IsVolatile = false;
                }
                else if (type == typeof(Move))
                {
                    var move = statement as Move;
                    var symbol = FindSymbol(move.RValue);
                    if (symbol != null && symbol.IsConstant)
                    {
                        var immediate = new Immediate(Tokenizer.ToToken(symbol.Value.ToString()));
                        var assigment = new Assignment(move.LValue, immediate);
                        block.Statements[i] = assigment;
                        symbol = FindSymbol(assigment.LValue);
                        symbol.IsConstant = true;
                        symbol.Value = immediate;
                    }
                    else
                    {
                        symbol = FindSymbol(move.LValue);
                        symbol.IsConstant = false;
                    }
                }
                else if (type.BaseType == typeof(MathStatement))
                {
                    var math = statement as MathStatement;
					var imath = statement as IMathOperator;
                    var symbol = FindSymbol(math.LValue);
                    if (symbol.IsConstant)
                    {
						var value = imath.Apply();
                        if (value != null)
                        {
							var lValue = math.LValue;
                            var immediate = value as Immediate;
                            symbol.IsConstant = true;
                            if (immediate.Value.Type == TokenType.IntType)
                            {
                                int result;
								result = int.Parse(immediate.Value);
                                var newImmediate = new Immediate(Tokenizer.ToToken(result.ToString()));
                                var assigment = new Assignment(lValue, newImmediate);
                                block.Statements[i] = assigment;
                                symbol.Value = newImmediate;
                            }
                        }
                        else
                        {
							symbol.IsConstant = false;
                        }
                    }
                }
                else if (type == typeof(Return))
                {
                    var returnType = statement as Return;
                    var symbol = FindSymbol((Declaration) returnType.ReturnReg);
                    if (symbol != null && symbol.IsConstant)
                    {
                        //returnType = new Return(symbol.Value);
                        //block.Statements[i] = returnType;
                    }
                    else
                    {

                    }
                }
            }
        }

        static OptimizerSymbol FindSymbol(Declaration decl)
        {
            var symbolToFind = OptimizerSymbol.Parse(decl);
            var index = symbolTable.IndexOf(symbolToFind);
            if (index == -1)
                return null;
            return symbolTable[index];
        }
    }


    class OptimizerSymbol
    {
        public string Name { get; set; }
        public bool IsConstant { get; set; }
        public bool IsVolatile { get; set; }
        public bool IsAlive { get; set; }
        public Datum Value { get; set; }

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
