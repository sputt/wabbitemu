using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Store : ValueStatement
    {
        public Datum StoreAddress;
        public Datum Value;

        public Store(Datum store, Datum value)
        {
            StoreAddress = store;
            Value = value;
        }

        public override List<Declaration> GetModifiedDeclarations()
        {
            var Result = new List<Declaration>();
            if (StoreAddress.GetType() == typeof(Declaration))
            {
                Result.Add(StoreAddress as Declaration);
            }
            return Result;
        }

        public override List<Declaration> GetReferencedDeclarations()
        {
            var Result = new List<Declaration>();
            if (Value.GetType() == typeof(Declaration))
            {
                Result.Add(Value as Declaration);
            }
            return Result;
        }

        public override string ToString()
        {
            return "*" + StoreAddress + " = " + Value + ";";
        }
    }
}
