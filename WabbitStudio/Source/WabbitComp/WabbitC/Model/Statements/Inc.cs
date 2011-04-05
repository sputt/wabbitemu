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
			return Decl + "++;";
		}

		public override string ToAssemblyString()
		{
			return "inc " + Decl;
		}
	}
}
