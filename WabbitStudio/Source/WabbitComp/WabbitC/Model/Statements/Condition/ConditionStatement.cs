using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class ConditionalOperator : ValueStatement
    {
        public Declaration Result;
        public Declaration CondDecl;
        public Token Operator;
        public Datum CondValue;

        public ConditionalOperator(Declaration result, Declaration condDecl, Token opToken, Datum condValue)
        {
            Result = result;
            CondDecl = condDecl;
            Operator = opToken;
            CondValue = condValue;
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(Result);
            sb.Append(" = ");
            sb.Append("(");
            sb.Append(CondDecl);
            sb.Append(" ");
            sb.Append(Operator);
            sb.Append(" ");
            sb.Append(CondValue);
            sb.Append(")");
            sb.Append(";");
            return sb.ToString();
        }
    }
}
