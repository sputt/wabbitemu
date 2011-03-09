using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Store : ValueStatement
    {
        Datum store;
        Datum value;

        public Store(Datum store, Datum value)
        {
            this.store = store;
            this.value = value;
        }

        public static void Parse(Block block, ref List<Token>.Enumerator tokens)
        {
            int deref = 0;
            if (tokens.Current == "*")
            {
                deref++;
            }
            tokens.MoveNext();

        }

        public override string ToString()
        {
            return base.ToString();
        }
    }
}
