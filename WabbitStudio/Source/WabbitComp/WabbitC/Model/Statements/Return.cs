using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Return : ControlStatement
    {
        public Datum ReturnReg;

        public Return(Datum decl)
        {
            ReturnReg = decl;
        }

        public override List<Declaration> GetReferencedDeclarations()
        {
            if (ReturnReg.GetType() == typeof(Declaration))
            {
                return new List<Declaration>() { ReturnReg as Declaration };
            }
            else
            {
                return base.GetReferencedDeclarations();
            }
        }

        public override string ToString()
        {
            return "return " + ReturnReg + ";";
        }
    }
}
