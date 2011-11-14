using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Return : ControlStatement
    {
        public Declaration ReturnReg;

        public Return(Declaration decl)
        {
            ReturnReg = decl;
        }

        public override ISet<Declaration> GetReferencedDeclarations()
        {
            if (ReturnReg != null && ReturnReg is Declaration)
            {
                return new HashSet<Declaration>() { ReturnReg as Declaration };
            }
            else
            {
                return base.GetReferencedDeclarations();
            }
        }

        public override string ToString()
        {
			if (ReturnReg == null)
				return "return;";
            return "return " + ReturnReg + ";";
        }
    }
}
