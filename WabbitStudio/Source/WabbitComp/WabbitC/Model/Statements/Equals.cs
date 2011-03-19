using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Equals : ValueStatement
    {
        public Declaration Result;
        public Declaration EqualsDecl;
        public Datum EqualsValue;

        public Equals(Declaration result, Declaration equalsDecl, Datum equalsValue)
        {
            Result = result;
            EqualsDecl = equalsDecl;
            EqualsValue = equalsValue;
        }

        public override string ToString()
        {
			StringBuilder sb = new StringBuilder();
            sb.Append(Result);
            sb.Append(" = (");
            sb.Append(EqualsDecl);
			sb.Append(" == ");
            sb.Append(EqualsValue);
            sb.Append(");");
			return sb.ToString();
        }
    }
}
