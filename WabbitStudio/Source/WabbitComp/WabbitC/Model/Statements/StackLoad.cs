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
		int Offset;
		/*public int StackOffset
		{
			get { return LValue.StackOffset; }
			set { LValue.StackOffset = value; }
		}*/

        public StackLoad(Declaration lValue, Declaration decl, int offset)
        {
            LValue = lValue;
            Decl = decl;
			Offset = offset;
        }

        public override string ToString()
        {
			//TODO: Make this true for structures too
            if (Decl.Type.GetType() == typeof(WabbitC.Model.Types.Array))
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

				return LValue.Name + " = (" + type + ") &(((unsigned char *) __iy)[" + 
					(-(Block.stack.Size - Offset) - Decl.Type.Size) + "]);";
            }
            else
            {
				return LValue.Name + " = *(" + Decl.Type + "*) &(((unsigned char *) __iy)[" +
					(-(Block.stack.Size - Offset) - Decl.Type.Size) + "]);";
            }
        }
    }
}
