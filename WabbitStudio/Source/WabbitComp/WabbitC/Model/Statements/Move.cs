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

        public override ISet<Declaration> GetModifiedDeclarations()
        {
            return new HashSet<Declaration>() { LValue };
        }

		public override ISet<Declaration> GetReferencedDeclarations()
        {
			return new HashSet<Declaration>() { RValue };
        }

        public override string ToString()
        {
            return LValue.Name + " = " + RValue.Name + ";";
        }

		public override string ToAssemblyString()
		{
            if (LValue == RValue)
                return String.Empty;
			return "ld " + LValue.Name + "," + RValue.Name;
		}
    }
}
