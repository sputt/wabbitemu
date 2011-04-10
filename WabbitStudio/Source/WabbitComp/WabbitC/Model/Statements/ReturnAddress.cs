using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
	/// <summary>
	/// Used to push the return address onto the stack before calling a function
	/// </summary>
	class ReturnAddress : Push
	{
		public Declaration Decl;

		public ReturnAddress(Declaration decl)
		{
			Decl = decl;
		}

		public override ISet<Declaration> GetModifiedDeclarations()
		{
			return new HashSet<Declaration>() { };
		}

		public override ISet<Declaration> GetReferencedDeclarations()
		{
			return new HashSet<Declaration>() { Decl };
		}

		public override string ToString()
		{
			return "__sp -= " + new BuiltInType("int").Size + ", (*(int *) __sp = " + Decl + ");";
		}

		public override string ToAssemblyString()
		{
			return "";
		}
	}
}
