using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Statements;
using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
	class StackFrameCleanup : Statement
	{
		int StackSize;

		public StackFrameCleanup(Block block, int stackSize)
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

			sb.AppendLine(new Pop(Block.FindDeclaration("__bc")).ToString());
			sb.AppendLine(new Pop(Block.FindDeclaration("__de")).ToString());
			sb.AppendLine(new Pop(Block.FindDeclaration("__hl")).ToString());
			sb.AppendLine(new Move(Block.FindDeclaration("__sp"), Block.FindDeclaration("__iy")).ToString());
			return sb.ToString();
		}
	}
}
