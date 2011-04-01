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
		int Offset;
		/*public int StackOffset
		{
			get { return LValue.StackOffset; }
			set { LValue.StackOffset = value; }
		}*/

        public StackStore(Declaration lValue, Declaration decl, int offset)
        {
            LValue = lValue;
            Decl = decl;
			Offset = offset;
        }

        public override string ToString()
        {
            if (Decl.Type.Size > 4)
            {
                return "";
            }
            else
            {
                return "*(" + LValue.Type + "*) &__stack[" + Offset + "] = " + Decl.Name + ";";
            }
        }
    }
}
