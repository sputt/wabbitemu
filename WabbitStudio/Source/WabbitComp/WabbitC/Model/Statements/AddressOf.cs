using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class AddressOf : ValueStatement
    {
        Declaration LValue;
        Declaration RValue;

        public AddressOf(Declaration lValue, Declaration rValue)
        {
            LValue = lValue;
            RValue = rValue;
        }

        public override string ToString()
        {
            return LValue + " = &" + RValue + ";";
        }
    }
}
