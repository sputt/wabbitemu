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

		public override List<Declaration> GetModifiedDeclarations()
		{
			return new List<Declaration>() { LValue };
		}

		public override string ToString()
		{
			return LValue.Name + " = " + CallTarget.Name + "();";
		}

		public override string ToAssemblyString()
		{
			return "call " + CallTarget.Name;
		}
	}
}
