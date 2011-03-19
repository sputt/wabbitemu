using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;

namespace WabbitC.Optimizer
{
    static class DeadCodeOptimizer
    {
        static List<OptimizerSymbol> symbolTable = new List<OptimizerSymbol>();
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
            bool[] deadInstructions = new bool[block.Statements.Count];
            bool changedInstructions;
            do
            {
                changedInstructions = false;
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
                        var decl = block.FindDeclaration(assignment.LValue.Name);
                        var isReferenced = IsReferenced(block, assignment.LValue, i + 1);
                        if (decl != null && !isReferenced)
                        {
                            deadInstructions[i] = true;
                            changedInstructions = true;
                        }
                    }
                }
                if (changedInstructions)
                {
                    List<Statement> newStatements = new List<Statement>();
                    for (int i = 0; i < block.Statements.Count; i++)
                    {
                        if (!deadInstructions[i])
                            newStatements.Add(block.Statements[i]);
                    }
                    block.Statements = newStatements;
                }
            } while (changedInstructions);
        }

        private static bool IsReferenced(Block block, Declaration declaration, int i)
        {
            bool isRefed = false;
            for (; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
                isRefed |= StatementHelper.Contains(statement, declaration);
            }
            return isRefed;
        }
    }
}
