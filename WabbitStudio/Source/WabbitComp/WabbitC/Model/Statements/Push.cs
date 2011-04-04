using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
	class Push : Statement
	{
		public Declaration Decl;

		public Push(Declaration decl)
		{
			Decl = decl;
		}

		public override List<Declaration> GetModifiedDeclarations()
		{
			return new List<Declaration>() { };
		}

		public override List<Declaration> GetReferencedDeclarations()
		{
			return new List<Declaration>() { Decl };
		}

		public override string ToString()
		{
			return "__sp -= " + new BuiltInType("int").Size + ", (*(int *) __sp = " + Decl + ");";
		}

		public override string ToAssemblyString()
		{
			return "push " + Decl;
		}
	}
}
