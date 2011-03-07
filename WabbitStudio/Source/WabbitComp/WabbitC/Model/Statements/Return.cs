using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Return : ControlStatement
    {
        Declaration ReturnReg;

        public Return(Declaration decl)
        {
            ReturnReg = decl;
        }

        public override string ToString()
        {
            return "return " + ReturnReg.Name + ";";
        }
    }
}
