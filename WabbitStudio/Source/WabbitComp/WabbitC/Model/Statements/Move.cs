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

        public override string ToString()
        {
            return LValue.Name + " = " + RValue.Name + ";";
        }
    }
}
