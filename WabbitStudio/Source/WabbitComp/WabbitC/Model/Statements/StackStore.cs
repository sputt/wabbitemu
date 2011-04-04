using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class StackStore : ValueStatement
    {
        Declaration Decl;
		Declaration Slot;
		/*public int StackOffset
		{
			get { return LValue.StackOffset; }
			set { LValue.StackOffset = value; }
		}*/

        public StackStore(Declaration slot, Declaration decl)
        {
			Slot = slot;
			Decl = decl;
        }

        public override string ToString()
        {
			if (Decl.Type.GetType() == typeof(WabbitC.Model.Types.Array))
            {
                return "";
            }
            else
            {
				Type SlotPtr = Slot.Type.Clone() as Type;
				SlotPtr.Reference();
				return "*(" + SlotPtr + ") &__sp[" + (-Block.stack.GetOffset(Slot)) + " - " + Decl.Type.Size + "] = (" + 
					Slot.Type + ") " + Decl.Name + ";";
            }
        }
    }
}
