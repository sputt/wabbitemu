using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;
using System.Diagnostics;
using WabbitC.StatementPasses.RegisterAllocator;

namespace WabbitC.StatementPasses
{
    static class SuperRegisterAllocator
    {
		public static void Run(Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				List<Block> basicBlocks = block.GetBasicBlocks();

				foreach (var decl in block.Declarations)
				{
					block.stack.ReserveSpace(decl);
				}

				block.Statements.Clear();
				for (int i = 0; i < basicBlocks.Count; i++)
				{
					Block basicBlock = basicBlocks[i];
					AllocateBlock(ref basicBlock);
					block.Statements.AddRange(basicBlock.Statements);
				}

				block.Statements.Insert(0, new StackFrameInit(block, block.stack.Size));
				block.Statements.Add(new StackFrameCleanup(block, block.stack.Size));

				block.Declarations.Clear();
			}
		}

		static void AllocateBlock(ref Block block)
		{
			RegisterHelper helper = new RegisterHelper(block);
			var statements = from Statement st in block select st;
			foreach (Statement statement in statements)
			{
				int index = block.Statements.IndexOf(statement);
				block.Statements.Remove(statement);

				var newStatements = new List<Statement>();

				foreach (var decl in statement.GetReferencedDeclarations().Union(statement.GetModifiedDeclarations()))
				{
					if (block.Module.Registers.Contains(decl))
					{
						helper.ReserveRegister(decl);
					}
				}

				foreach (var decl in statement.GetReferencedDeclarations().Union(statement.GetModifiedDeclarations()))
				{
					var allocStatements = new List<Statement>();
					Declaration reg = helper.AllocateRegister(decl, ref allocStatements);
					Debug.Assert(decl != null);
					Debug.Assert(reg != null);

					if (reg != decl && statement.GetReferencedDeclarations().Contains(decl))
					{
						newStatements.Add(new StackLoad(reg, decl));
					}
					newStatements.AddRange(allocStatements);
					statement.ReplaceDeclaration(decl, reg);
				}

				newStatements.Add(statement);

				foreach (var decl in statement.GetModifiedDeclarations())
				{
					Declaration slot = helper.GetAssignedVariable(decl);
					Debug.Assert(slot != null);
					Debug.Assert(decl != null);
					if (slot != decl)
					{
						newStatements.Add(new StackStore(slot, decl));
					}
				}

				block.Statements.InsertRange(index, newStatements);

				foreach (var decl in statement.GetReferencedDeclarations().Union(statement.GetModifiedDeclarations()))
				{
					helper.FreeRegister(decl);
				}
			}
		}
    }
}
