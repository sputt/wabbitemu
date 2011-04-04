using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Statements;
using WabbitC.Model.Types;
using WabbitC.Model.Statements.Math;

namespace WabbitC.Model.Statements
{
    class StackFrameInit : Statement
    {
		int StackSize;
		public Block InitBlock;

        public StackFrameInit(Block block, int stackSize)
        {
			StackSize = stackSize;

			InitBlock = new Block(block);
			InitBlock.Statements.Add(new Push(block.FindDeclaration("__iy")));

			//BuiltInType blt = block.Function.ReturnType as BuiltInType;
			//if (blt != null && blt.BuildInTypeType == BuiltInType.BuiltInTypeType.Void)
			//{
			//    InitBlock.Statements.Add(new Push(block.FindDeclaration("__hl")));
			//}
			InitBlock.Statements.Add(new Push(block.FindDeclaration("__de")));
			InitBlock.Statements.Add(new Push(block.FindDeclaration("__bc")));

			InitBlock.Statements.Add(new Assignment(block.FindDeclaration("__iy"), new Immediate(0)));
            var tempDecl = block.CreateTempDeclaration(block.FindDeclaration("__iy").Type);
            InitBlock.Statements.Add(new Cast(tempDecl, block.FindDeclaration("__iy").Type, block.FindDeclaration("__sp")));
			InitBlock.Statements.Add(new Add(block.FindDeclaration("__iy"), tempDecl));
			int StackOffset = -(StackSize - block.stack.GetNonAutosSize());
			if (StackOffset != 0)
			{
				InitBlock.Statements.Add(new Assignment(block.FindDeclaration("__hl"), new Immediate(StackOffset)));
				InitBlock.Statements.Add(new Add(block.FindDeclaration("__hl"), block.FindDeclaration("__sp")));
				InitBlock.Statements.Add(new Move(block.FindDeclaration("__sp"), block.FindDeclaration("__hl")));
			}
        }

		public override string ToString()
		{
			return InitBlock.ToString().Replace("{", "").Replace("}", "");
		}

		public override string ToAssemblyString()
		{
			return InitBlock.ToAssemblyString();
		}

    }
}
