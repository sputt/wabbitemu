using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Move : ValueStatement
    {
        public Declaration LValue;
        public Declaration RValue;

        public Move(Declaration dest, Declaration src)
        {
            LValue = dest;
            RValue = src;
        }

        public override List<Declaration> GetModifiedDeclarations()
        {
            return new List<Declaration>() { LValue };
        }

        public override List<Declaration> GetReferencedDeclarations()
        {
			return new List<Declaration>() { LValue, RValue };
        }

        public override string ToString()
        {
			if (LValue == RValue)
				return "";
            return LValue.Name + " = " + RValue.Name + ";";
        }

		public override string ToAssemblyString()
		{
			if (LValue == RValue)
				return "";
			return "ld " + LValue.Name + "," + RValue.Name;
		}
    }
}
