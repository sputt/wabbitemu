using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class ReturnMove : ValueStatement
    {
        public Declaration RValue;
        public ReturnMove(Declaration src)
        {
            RValue = src;
        }

        public override List<Declaration> GetModifiedDeclarations()
        {
            return new List<Declaration>();
        }

        public override List<Declaration> GetReferencedDeclarations()
        {
            return new List<Declaration>() { RValue };
        }

        public override string ToString()
        {
            return "__hl = " + (RValue.ConstValue == null ? RValue.Name  : RValue.ConstValue.ToString()) + ";";
        }
    }
}
