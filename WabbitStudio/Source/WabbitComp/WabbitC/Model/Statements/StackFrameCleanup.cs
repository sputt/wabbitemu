using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Statements;
using WabbitC.Model.Types;
using WabbitC.Model.Statements.Math;

namespace WabbitC.Model.Statements
{
	class StackFrameCleanup : Statement
	{
		int StackSize;
		Block CleanupBlock;

		public StackFrameCleanup(Block block, int stackSize)
		{
			StackSize = stackSize;
			CleanupBlock = new Block(block);

			int StackOffset = StackSize - block.stack.GetNonAutosSize();
			CleanupBlock.Statements.Add(new Exchange());
			CleanupBlock.Statements.Add(new Assignment(block.FindDeclaration("__hl"), new Immediate(StackOffset)));
			CleanupBlock.Statements.Add(new Add(block.FindDeclaration("__hl"), block.FindDeclaration("__sp")));
			CleanupBlock.Statements.Add(new Move(block.FindDeclaration("__sp"), block.FindDeclaration("__hl")));
			CleanupBlock.Statements.Add(new Exchange());

			CleanupBlock.Statements.Add(new Pop(block.FindDeclaration("__bc")));
			CleanupBlock.Statements.Add(new Pop(block.FindDeclaration("__de")));
			//BuiltInType blt = block.Function.ReturnType as BuiltInType;
			//if (blt != null && blt.BuildInTypeType == BuiltInType.BuiltInTypeType.Void)
			//{
			//    CleanupBlock.Statements.Add(new Pop(block.FindDeclaration("__hl")));
			//}
			CleanupBlock.Statements.Add(new Pop(block.FindDeclaration("__iy")));
		}

		public override string ToString()
		{
			return CleanupBlock.ToString().Replace("{", "").Replace("}", "");
		}
	}
}
