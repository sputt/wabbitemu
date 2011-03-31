using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements.Assembly
{
	class Load : ValueStatement
	{
		Declaration LValue, RValue;
		public Load(Declaration lValue, Declaration rValue)
		{
			LValue = lValue;
			RValue = rValue;
		}

		public override string ToString()
		{
			return "ld " + LValue + "," + RValue;
		}
	}
}
