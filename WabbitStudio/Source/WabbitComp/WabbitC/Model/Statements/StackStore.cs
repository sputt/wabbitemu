using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class StackStore : ValueStatement
    {
        Declaration LValue;
        Declaration Decl;

        public StackStore(Declaration lValue, Declaration decl)
        {
            LValue = lValue;
            Decl = decl;
        }

        public override string ToString()
        {
            return "*(" + LValue.Type + "*) &__stack[" + LValue.StackOffset + "] = " + Decl.Name + ";";
        }
    }
}
