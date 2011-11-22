using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class Assignment : ValueStatement, ILValue, IRValue
	{
		public Declaration LValue { get; set; }
		public Datum RValue { get; set; }

		public Assignment(Declaration decl, Immediate imm)
		{
			LValue = decl;
			RValue = imm;
		}

		public override ISet<Declaration> GetModifiedDeclarations()
		{
			return new HashSet<Declaration>() { LValue };
		}

		public override ISet<Declaration> GetReferencedDeclarations()
		{
			return new HashSet<Declaration>() { };
		}

		public override string ToString()
		{
			return LValue.Name + " = " + RValue + ";";
		}

		public override string ToAssemblyString()
		{
			return "ld " + LValue.Name + "," + RValue;
		}

		public override Immediate Apply()
		{
			return (Immediate) RValue;
		}
	}
}
