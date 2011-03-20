using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using System.Diagnostics;

namespace WabbitC.Optimizer
{
    static class DeadVariableOptimizer
    {
        public static void Optimize(ref Module module)
        {
            for (int i = 0; i < module.Declarations.Count; i++)
            {
                Declaration decl = module.Declarations[i];
                OptimizeBlock(ref decl.Code);
            }
        }

        public static void OptimizeBlock(ref Block block)
        {
            List<OptimizerSymbol> symbolTable = new List<OptimizerSymbol>();
            foreach (var decl in block.Declarations)
            {
                var symbol = OptimizerSymbol.Parse(decl);
                symbol.IsAlive = false;
                symbolTable.Add(symbol);
            }
            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
                var type = statement.GetType();
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
                    MarkAlive(assignment.LValue, ref symbolTable);
                }
                else if (type == typeof(Move))
                {
                    var move = statement as Move;
                    MarkAlive(move.LValue, ref symbolTable);
                    MarkAlive(move.RValue, ref symbolTable);
                }
                else if (type == typeof(Sub))
                {
                    var sub = statement as Sub;
                    MarkAlive(sub.LValue, ref symbolTable);
                }
                else if (type == typeof(FunctionCall))
                {
                    FunctionCall funcCall = statement as FunctionCall;
                    MarkAlive(funcCall.LValue, ref symbolTable);
                }
            }
            for (int i = block.Declarations.Count - 1; i > -1; i--)
            {
                if (!symbolTable[i].IsAlive)
                    block.Declarations.RemoveAt(i);
            }
        }

        static void MarkAlive(Declaration decl, ref List<OptimizerSymbol> symbolTable)
        {
            var symbol = FindSymbol(decl, ref symbolTable);
            if (symbol != null)
                symbol.IsAlive = true;
        }

        static OptimizerSymbol FindSymbol(Declaration decl, ref List<OptimizerSymbol> symbolTable)
        {
            var symbolToFind = OptimizerSymbol.Parse(decl);
            var index = symbolTable.IndexOf(symbolToFind);
            if (index == -1)
                return null;
            return symbolTable[index];
        }
    }
}
