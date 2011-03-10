using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Add : ValueStatement
    {
        public Declaration LValue;
        public Datum AddValue;

        public Add(Declaration lValue, Datum addValue)
        {
            LValue = lValue;
            AddValue = addValue;
        }

        public override string ToString()
        {
			StringBuilder sb = new StringBuilder(LValue.Name);
			sb.Append(" += ");
			sb.Append(AddValue);
            sb.Append(";");
			return sb.ToString();
        }
    }
}
