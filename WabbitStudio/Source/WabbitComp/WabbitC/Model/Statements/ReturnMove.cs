using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class ReturnMove : ValueStatement
    {
        public Datum RValue;
        public ReturnMove(Datum src)
        {
            RValue = src;
        }

        public override ISet<Declaration> GetModifiedDeclarations()
        {
			Declaration decl = Block.Function.Is16() ? Block.FindDeclaration("__hl") : Block.FindDeclaration("__a");
			return new HashSet<Declaration>() { decl };
        }

        public override ISet<Declaration> GetReferencedDeclarations()
        {
            var temp = new HashSet<Declaration>();
			if (RValue is Declaration)
				temp.Add((Declaration)RValue);
			return temp;
        }

        public override string ToString()
        {
            return GetModifiedDeclarations().First() + " = " + RValue.ToString() + ";";
        }

		public override string ToAssemblyString()
		{
			return "ld " + GetModifiedDeclarations().First() + "," + RValue.ToString();
		}
    }
}
