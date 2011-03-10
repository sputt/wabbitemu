using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class Sub: ValueStatement
    {
        public Declaration LValue;
        public Datum SubValue;

        public Sub(Declaration lValue, Datum subValue)
        {
            LValue = lValue;
            SubValue = subValue;
        }

        public override string ToString()
        {
			StringBuilder sb = new StringBuilder(LValue.Name);
			sb.Append(" -= ");
			sb.Append(SubValue);
            sb.Append(";");
			return sb.ToString();
        }
    }
}
