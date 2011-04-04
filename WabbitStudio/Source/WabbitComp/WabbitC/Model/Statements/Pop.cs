using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
	class Pop : Statement
	{
		public Declaration Decl;

		public Pop(Declaration decl)
		{
			Decl = decl;
		}

		public override List<Declaration> GetModifiedDeclarations()
		{
			return new List<Declaration>() { Decl };
		}

		public override List<Declaration> GetReferencedDeclarations()
		{
			return new List<Declaration>() { Decl };
		}

		public override string ToString()
		{
			return "(" + Decl + "= *(int *) __sp), __sp += " + new BuiltInType("int").Size + ";";
		}

		public override string ToAssemblyString()
		{
			return "pop " + Decl;
		}
	}
}
