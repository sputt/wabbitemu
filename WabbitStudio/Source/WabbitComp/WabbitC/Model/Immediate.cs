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

        public static bool IsImmediate(Token immToken)
        {
            int number1;
            double number2;

            return int.TryParse(immToken.Text, out number1) || double.TryParse(immToken.Text, out number2);
        }

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
