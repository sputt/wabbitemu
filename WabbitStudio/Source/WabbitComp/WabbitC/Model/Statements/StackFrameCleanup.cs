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

			int localsSize = StackSize - block.stack.GetNonAutosSize();
			int paramsSize = block.stack.GetOffset(block.Function.Params.Last<Declaration>()) +
				block.Function.Params.Last<Declaration>().Type.Size;

			Declaration hl = block.FindDeclaration("__hl");
			Declaration de = block.FindDeclaration("__de");
			Declaration bc = block.FindDeclaration("__bc");
			Declaration iy = block.FindDeclaration("__iy");
			Declaration sp = block.FindDeclaration("__sp");

			var s = CleanupBlock.Statements;
			s.Add(new Push(hl));
			s.Add(new Assignment(hl, new Immediate(StackSize + hl.Type.Size - 1)));
			s.Add(new Add(hl, sp));
			s.Add(new Exchange());
			s.Add(new Assignment(hl, new Immediate(-paramsSize)));
			s.Add(new Add(hl, de));
			s.Add(new Assignment(bc, new Immediate(StackSize - localsSize - paramsSize)));
			s.Add(new BlockCopyReverse());
			s.Add(new Pop(hl));
			s.Add(new Exchange());
			s.Add(new Add(hl, new Immediate(1)));
			s.Add(new Move(sp, hl));
			s.Add(new Exchange());
			s.Add(new Pop(bc));
			s.Add(new Pop(de));
			s.Add(new Pop(iy));
			s.Add(new Add(sp, new Immediate(4)));
			s.Add(new Return(null));
		}

		public override string ToString()
		{
			return CleanupBlock.ToString().Replace("{", String.Empty).Replace("}", String.Empty);
		}

		public override string ToAssemblyString()
		{
			return CleanupBlock.ToAssemblyString();
		}
	}
}
