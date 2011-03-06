using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Assignment : ValueStatement
    {
        public Declaration LValue;
        public Value RValue;

        public override string ToString()
        {
            return LValue.Name + " = " + RValue;
        }
    }
}
