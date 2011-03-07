using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model
{
    class Immediate
    {
        public BuiltInType Type;
        public Token Value;

        public Immediate(List<Token> valueList)
        {
            Type = new BuiltInType();
            Value = valueList[0];
        }

        public override string ToString()
        {
            return Value.ToString();
        }
    }
}
