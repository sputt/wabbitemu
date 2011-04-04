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
			InitBlock.Statements.Add(new Add(block.FindDeclaration("__iy"), block.FindDeclaration("__sp")));
			int StackOffset = -(StackSize - block.stack.GetNonAutosSize());
			InitBlock.Statements.Add(new Assignment(block.FindDeclaration("__hl"), new Immediate(StackOffset)));
			InitBlock.Statements.Add(new Add(block.FindDeclaration("__hl"), block.FindDeclaration("__sp")));
			InitBlock.Statements.Add(new Move(block.FindDeclaration("__sp"), block.FindDeclaration("__hl")));
        }

		public override string ToString()
		{
			return InitBlock.ToString().Replace("{", "").Replace("}", "");
		}

		public override string ToAssemblyString()
		{
			int TotalParamSize = 0;
			foreach (Declaration decl in Block.Function.Params)
			{
				TotalParamSize += decl.Type.Size;
			}
			TotalParamSize += new BuiltInType("void *").Size;
			TotalParamSize *= -1;

			var sb = new StringBuilder();

			sb.AppendLine(new Push(Block.FindDeclaration("__iy")).ToAssemblyString());
			sb.AppendLine(new Push(Block.FindDeclaration("__hl")).ToAssemblyString());
			sb.AppendLine(new Push(Block.FindDeclaration("__de")).ToAssemblyString());
			sb.AppendLine(new Push(Block.FindDeclaration("__bc")).ToAssemblyString());
			var exp = new Expression(Tokenizer.Tokenize(TotalParamSize.ToString()));
			sb.AppendLine(new Store(Block.FindDeclaration("__de"), 
				Datum.Parse(Block, exp.Eval()[0].Token)).ToAssemblyString());
			sb.AppendLine(new Sub(Block.FindDeclaration("__iy"), Block.FindDeclaration("__de")).ToAssemblyString());
			if (StackSize - TotalParamSize > 0)
				sb.AppendLine("__sp += " + (StackSize - TotalParamSize).ToString() + ";");

			return sb.ToString();
		}

    }
}
