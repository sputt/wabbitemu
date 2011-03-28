using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Cast : ValueStatement
    {
        public Declaration LValue;
        public Type Type;
        public Datum RValue;

        public Cast(Declaration lValue, Type type, Datum datum)
        {
            LValue = lValue;
            Type = type;
            RValue = datum;
        }

        public override List<Declaration> GetModifiedDeclarations()
        {
            return new List<Declaration>() { LValue };
        }

        public override List<Declaration> GetReferencedDeclarations()
        {
            if (RValue.GetType() == typeof(Declaration))
            {
                return new List<Declaration>() { RValue as Declaration };
            }
            else
            {
                return base.GetReferencedDeclarations();
            }
        }

        public override string ToString()
        {
            return LValue + " = (" + Type + ") " + RValue + ";";
        }
    }
}
