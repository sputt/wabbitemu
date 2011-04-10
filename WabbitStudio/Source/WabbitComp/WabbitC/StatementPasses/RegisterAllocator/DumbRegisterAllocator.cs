using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses.RegisterAllocator
{
    static class DumbRegisterAllocator
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				Helper helper = new Helper(block);

				foreach (var decl in block.Declarations)
				{
					block.stack.ReserveSpace(decl);
				}

				var statements = from Statement st in block select st;
				foreach (Statement statement in statements)
				{
					int index = block.Statements.IndexOf(statement);
					block.Statements.Remove(statement);

					var newStatements = new List<Statement>();

					foreach (var decl in statement.GetReferencedDeclarations())
					{
						var allocStatements = new List<Statement>();
						Declaration reg = helper.AllocateRegister(decl, ref allocStatements);
						Debug.Assert(decl != null);
						newStatements.Add(new StackLoad(reg, decl));
						newStatements.AddRange(allocStatements);

						statement.ReplaceDeclaration(decl, reg);
					}

					newStatements.Add(statement);
					
					foreach (var decl in statement.GetModifiedDeclarations())
					{
						newStatements.Add(new StackStore(helper.GetAssignedVariable(decl), decl));
					}

					block.Statements.InsertRange(index, newStatements);

					foreach (var decl in statement.GetReferencedDeclarations())
					{
						helper.FreeRegister(decl);
					}
				}

				functions.Current.Code.Statements.Insert(0, new StackFrameInit(block, block.stack.Size));
				functions.Current.Code.Statements.Add(new StackFrameCleanup(block, block.stack.Size));
			}
        }
    }
}
