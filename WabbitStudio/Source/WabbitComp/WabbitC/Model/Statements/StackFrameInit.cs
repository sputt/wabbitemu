using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Statements;
using WabbitC.Model.Types;

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

			sb.AppendLine(new Push(Block.FindDeclaration("__iy")).ToString());
			sb.AppendLine("__iy = __sp - " + (TotalParamSize).ToString() + ";");
			if (StackSize - TotalParamSize > 0)
				sb.AppendLine("__sp += " + (StackSize - TotalParamSize).ToString() + ";");
			sb.AppendLine(new Push(Block.FindDeclaration("__hl")).ToString());
			sb.AppendLine(new Push(Block.FindDeclaration("__de")).ToString());
			sb.AppendLine(new Push(Block.FindDeclaration("__bc")).ToString());

			return sb.ToString();
		}

    }
}
