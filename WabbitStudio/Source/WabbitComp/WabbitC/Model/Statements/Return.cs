using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Return : ControlStatement
    {
        Datum returnReg;
        public Datum ReturnReg
        {
            get { return returnReg; }
        }

        public Return(Datum decl)
        {
            returnReg = decl;
        }

        public override string ToString()
        {
            return "return " + ReturnReg + ";";
        }
    }
}
