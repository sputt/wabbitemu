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
            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
				var refed = statement.GetReferencedDeclarations();
				var modified = statement.GetModifiedDeclarations();
				MarkAlive(refed);
				MarkAlive(modified);
            }
            for (int i = block.Declarations.Count - 1; i > -1; i--)
            {
                if (!block.Declarations[i].IsAlive)
                    block.Declarations.RemoveAt(i);
            }
        }

        static void MarkAlive(IEnumerable<Declaration> decls)
        {
			foreach (var decl in decls)
			{
				decl.IsAlive = true;
			}
        }
    }
}
