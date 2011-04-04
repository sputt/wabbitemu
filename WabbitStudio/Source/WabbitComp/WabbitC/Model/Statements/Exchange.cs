using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class Exchange : ValueStatement
	{
		public Exchange()
		{
		}
		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();
			sb.Append("__hl ^= __de, __de ^= __hl, __hl ^= __de;");
			return sb.ToString();
		}

		public override List<Declaration> GetReferencedDeclarations()
		{
			return new List<Declaration>() { Block.FindDeclaration("__hl"), Block.FindDeclaration("__de") };
		}
	}
}
