using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class StackLoad : ValueStatement
    {
        Declaration LValue;
        Declaration Decl;

        public StackLoad(Declaration lValue, Declaration decl)
        {
            LValue = lValue;
            Decl = decl;
        }

        public override string ToString()
        {
            return LValue.Name + " = *(" + Decl.Type + "*) &__stack[" + Decl.StackOffset + "];";
        }
    }
}
