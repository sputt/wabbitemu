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
				return "*(" + SlotPtr + ") &(((unsigned char *) __iy)[" + (-Block.stack.GetOffset(Slot)) + " + " + Block.stack.GetNonAutosSize() + " - " + Decl.Type.Size + "]) = (" + 
					Slot.Type + ") " + Decl.Name + ";";
            }
        }

		public override string ToAssemblyString()
		{
			return "stack_store(\"" + Decl.Name + "\", " + -Block.stack.GetOffset(Slot) + ")";
		}
    }
}
