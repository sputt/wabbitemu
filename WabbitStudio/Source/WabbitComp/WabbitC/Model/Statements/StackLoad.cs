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
		public int StackOffset
		{
			get { return LValue.StackOffset; }
			set { LValue.StackOffset = value; }
		}

        public StackLoad(Declaration lValue, Declaration decl)
        {
            LValue = lValue;
            Decl = decl;
        }

        public override string ToString()
        {
            if (Decl.Type.Size > 4)
            {
                // It refers to the stack, not stored in the stack
                Type type;
                if (Decl.Type.GetType() == typeof(Types.Array))
                {
                    type = (Decl.Type as Types.Array).GetArrayPointerType();
                }
                else
                {
                    type = (Type) Decl.Type.Clone();
                    type.Reference();
                }

                return LValue.Name + " = (" + type + ") &__stack[" + Decl.StackOffset + "];";
            }
            else
            {
                return LValue.Name + " = *(" + Decl.Type + "*) &__stack[" + Decl.StackOffset + "];";
            }
        }
    }
}
