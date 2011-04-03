using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    static class AssignmentHelper
    {
        //public static bool IsLValue()
        //{
   
        //}

        public static ValueStatement ParseSingle(Block block, Declaration LValue, Token RValue)
        {
            if (Immediate.IsImmediate(RValue) == false)
            {
                Declaration valDecl = block.FindDeclaration(RValue.Text);
				if (valDecl != null && LValue.ConstValue != null)
					LValue.ConstValue = new Immediate(LValue.ConstValue.Value);
                return new Move(LValue, valDecl);
            }
            else
            {
                Immediate valImm = new Immediate(RValue);
				LValue.ConstValue = valImm;
                return new Assignment(LValue, valImm);
            }
        }
    }
}
