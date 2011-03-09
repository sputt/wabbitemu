using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    public class Datum
    {
        public static Datum Parse(Block block, Token token)
        {
            if (Immediate.IsImmediate(token))
            {
                return (Datum)new Immediate(token);
            }
            else
            {
                return (Datum)block.FindDeclaration(token);
            }
        }
    }
}
