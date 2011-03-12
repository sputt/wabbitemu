using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Load : ValueStatement
    {
        Declaration LValue;
        Datum LoadAddress;

        public Load(Declaration LValue, Datum loadAddr)
        {
            this.LValue = LValue;
            this.LoadAddress = loadAddr;
        }

        public override string ToString()
        {
            return this.LValue.Name + " = *" + this.LoadAddress.ToString() + ";";
        }
    }
}
