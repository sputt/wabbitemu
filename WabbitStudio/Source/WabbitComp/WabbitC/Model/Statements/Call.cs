using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class Call : ControlStatement
	{
		public Declaration CallTarget;

		public Call(Declaration callTarget)
		{
			CallTarget = callTarget;
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
