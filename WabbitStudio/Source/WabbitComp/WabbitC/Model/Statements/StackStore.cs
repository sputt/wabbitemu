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
		public int StackOffset
		{
			get { return LValue.StackOffset; }
			set { LValue.StackOffset = value; }
		}

        public StackStore(Declaration lValue, Declaration decl)
        {
            LValue = lValue;
            Decl = decl;
        }

        public override string ToString()
        {
            if (Decl.Type.Size > 4)
            {
                return "";
            }
            else
            {
                return "*(" + LValue.Type + "*) &__stack[" + LValue.StackOffset + "] = " + Decl.Name + ";";
            }
        }
    }
}
