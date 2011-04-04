using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class StackLoad : ValueStatement
    {
        Declaration Slot;
        Declaration Decl;
		/*public int StackOffset
		{
			get { return LValue.StackOffset; }
			set { LValue.StackOffset = value; }
		}*/

        public StackLoad(Declaration decl, Declaration slot)
        {
			Slot = slot;
            Decl = decl;
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

				return Decl.Name + " = (" + Decl.Type + ") (" + type + ") &(((unsigned char *) __iy)[" +
					(-Block.stack.GetOffset(Slot)) + " - " + Slot.Type.Size + "]);";
            }
            else
            {
				return Decl.Name + " = (" + Decl.Type +") *(" + Slot.Type + "*) &(((unsigned char *) __iy)[" +
					(-Block.stack.GetOffset(Slot)) + " - " + Slot.Type.Size + "]);";
            }
        }

		public override string ToAssemblyString()
		{
			return "";
		}
    }
}
