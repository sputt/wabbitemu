using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Add : ValueStatement
    {
        public Declaration LValue;
        public Declaration AddValue;

        public Add(Declaration lValue, Declaration addValue)
        {
            LValue = lValue;
            AddValue = addValue;
        }

        public override string ToString()
        {
            return LValue.Name + " += " + AddValue.Name;
        }
    }
}
