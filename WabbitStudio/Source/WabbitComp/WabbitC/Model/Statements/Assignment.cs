using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Assignment : ValueStatement
    {
        public Declaration LValue;
        public Immediate RValue;

        public Assignment(Declaration decl, Immediate imm)
        {
            LValue = decl;
            RValue = imm;
        }

        public override List<Declaration> GetModifiedDeclarations()
        {
            return new List<Declaration>() { LValue };
        }

        public override List<Declaration> GetReferencedDeclarations()
        {
			return new List<Declaration>() { LValue };
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
            return RValue;
        }
    }
}
