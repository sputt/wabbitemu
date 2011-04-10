using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class Call : ControlStatement
	{
		public Declaration LValue;
		public Declaration CallTarget;

		public Call(Declaration callTarget, Declaration LValue)
		{
			CallTarget = callTarget;
			this.LValue = LValue;
		}

		public override ISet<Declaration> GetReferencedDeclarations()
		{
			return new HashSet<Declaration>() { LValue };
		}

		public override ISet<Declaration> GetModifiedDeclarations()
		{
			return new HashSet<Declaration>() { };
		}

		public override string ToString()
		{
			return CallTarget.Name + "();";
		}

		public override string ToAssemblyString()
		{
			return "call " + CallTarget.Name;
		}
	}
}
