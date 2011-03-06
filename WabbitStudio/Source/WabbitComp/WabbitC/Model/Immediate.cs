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

        public Immediate(Token valueToken)
        {
            Type = new BuiltInType();
            Value = valueToken;
        }

        public override string ToString()
        {
            return Value.ToString();
        }
    }
}
