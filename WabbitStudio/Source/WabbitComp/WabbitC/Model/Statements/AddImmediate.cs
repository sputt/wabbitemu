using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class AddImmediate : ValueStatement
    {
        public Declaration LValue;
        public Immediate AddValue;

        public AddImmediate(Declaration lValue, Immediate addValue)
        {
            LValue = lValue;
            AddValue = addValue;
        }

        public override string ToString()
        {
            return LValue.Name + " += " + AddValue;
        }
    }
}
