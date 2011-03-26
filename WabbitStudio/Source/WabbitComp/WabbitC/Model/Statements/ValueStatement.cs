using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class ValueStatement : Statement
    {
        public virtual Immediate Apply()
        {
            throw new NotImplementedException();
        }
    }

    interface IMathOperator
    {
        Token GetHandledOperator();
        Immediate Apply();
    }
}
