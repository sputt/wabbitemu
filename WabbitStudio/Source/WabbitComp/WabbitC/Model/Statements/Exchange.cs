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
			return "__hl ^= __de, __de ^= __hl, __hl ^= __de;";
		}

		public override string ToAssemblyString()
		{
			return "ex de,hl";
		}

		public override ISet<Declaration> GetModifiedDeclarations()
		{
			return new HashSet<Declaration>() { Block.FindDeclaration("__hl"), Block.FindDeclaration("__de") };
		}

		public override ISet<Declaration> GetReferencedDeclarations()
		{
			return new HashSet<Declaration>() { Block.FindDeclaration("__hl"), Block.FindDeclaration("__de") };
		}
	}
}
