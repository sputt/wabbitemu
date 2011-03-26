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

        public override string ToString()
        {
            return LValue + " = (" + Type + ") " + RValue + ";";
        }
    }
}
