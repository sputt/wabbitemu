using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class Move : ValueStatement, ILValue, IRValue
	{
		public Declaration LValue { get; set; }
		public Datum RValue { get; set; }

		public Move(Declaration dest, Declaration src)
		{
			LValue = dest;
			RValue = src;
		}

		public override ISet<Declaration> GetModifiedDeclarations()
		{
			return new HashSet<Declaration>() { LValue };
		}

		public override ISet<Declaration> GetReferencedDeclarations()
		{
			return new HashSet<Declaration>() { (Declaration) RValue };
		}

		public override string ToString()
		{
			return LValue.Name + " = " + ((Declaration) RValue).Name + ";";
		}

		public override string ToAssemblyString()
		{
			if (LValue == RValue)
			{
				return String.Empty;
			}
			return "ld " + LValue.Name + "," + ((Declaration)RValue).Name;
		}
	}
}
