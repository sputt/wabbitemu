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

        public StackFrameInit(Block block, int stackSize)
        {
			StackSize = stackSize;
        }

		public override string ToString()
		{
			int TotalParamSize = 0;
			foreach (Declaration decl in Block.Function.Params)
			{
				TotalParamSize += decl.Type.Size;
			}
			TotalParamSize += new BuiltInType("void *").Size;

			var sb = new StringBuilder();

			if (StackSize - TotalParamSize > 0)
				sb.AppendLine("__sp += " + (StackSize - TotalParamSize).ToString() + ";");
			sb.AppendLine(new Move(Block.FindDeclaration("__iy"), Block.FindDeclaration("__sp")).ToString());
			sb.AppendLine(new Push(Block.FindDeclaration("__hl")).ToString());
			sb.AppendLine(new Push(Block.FindDeclaration("__de")).ToString());
			sb.AppendLine(new Push(Block.FindDeclaration("__bc")).ToString());

			return sb.ToString();
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
