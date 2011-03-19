using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class ConditionalOperator : ValueStatement
    {
        public Declaration Result;
        public Declaration EqualsDecl;
        public Token Operator;
        public Datum EqualsValue;

        public ConditionalOperator(Declaration result, Declaration equalsDecl, Token opToken, Datum equalsValue)
        {
            Result = result;
            EqualsDecl = equalsDecl;
            Operator = opToken;
            EqualsValue = equalsValue;
        }
    }
}
