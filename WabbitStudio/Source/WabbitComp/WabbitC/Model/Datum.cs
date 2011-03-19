using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    class Datum
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

        public override string ToString()
        {
            if (this.GetType() == typeof(Immediate))
            {
                return ((Immediate) this).Value;
            }
            else if (this.GetType() == typeof(Declaration))
            {
                return ((Declaration)this).Name;
            }
            else
            {
                return base.ToString();
            }
        }
    }
}
