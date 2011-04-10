using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class Inc : ValueStatement
	{
		public Declaration Decl;
		public Inc(Declaration decl)
		{
			Decl = decl;
		}

		public override ISet<Declaration> GetModifiedDeclarations()
		{
			return new HashSet<Declaration>() { Decl };
		}

		public override ISet<Declaration> GetReferencedDeclarations()
		{
			return new HashSet<Declaration>() { Decl };
		}

		public override string ToString()
		{
			return Decl + "++;";
		}

		public override string ToAssemblyString()
		{
			return "inc " + Decl;
		}
	}
}
