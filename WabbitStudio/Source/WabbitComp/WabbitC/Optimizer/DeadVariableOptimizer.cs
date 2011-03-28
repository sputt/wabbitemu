using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
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
                if (decl.Code != null)
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
				var refed = statement.GetReferencedDeclarations();
				var modified = statement.GetModifiedDeclarations();
				MarkAlive(refed, ref symbolTable);
				MarkAlive(modified, ref symbolTable);
				/*var type = statement.GetType();
                if (type == typeof(Assignment))
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
                else if (type == typeof(ConditionStatement))
                {
                    var condOp = statement as ConditionStatement;
                    MarkAlive(condOp.CondDecl, ref symbolTable);
                    if (condOp.CondValue.GetType() == typeof(Declaration))
                        MarkAlive((Declaration)condOp.CondValue, ref symbolTable);
                }
                else if (type == typeof(Sub) || type == typeof(Add))
                {
                    var sub = statement as Sub;
					var add = statement as Add;
                    MarkAlive(add != null ? add.LValue : sub.LValue, ref symbolTable);
                }
                else if (type == typeof(FunctionCall))
                {
                    FunctionCall funcCall = statement as FunctionCall;
                    MarkAlive(funcCall.LValue, ref symbolTable);
				}*/
            }
            for (int i = block.Declarations.Count - 1; i > -1; i--)
            {
                if (!symbolTable[i].IsAlive)
                    block.Declarations.RemoveAt(i);
            }
        }

        static void MarkAlive(List<Declaration> decls, ref List<OptimizerSymbol> symbolTable)
        {
			foreach (var decl in decls)
			{
				var symbol = FindSymbol(decl, ref symbolTable);
				if (symbol != null)
					symbol.IsAlive = true;
			}
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
