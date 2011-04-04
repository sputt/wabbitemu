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
            if (StoreAddress.GetType() == typeof(Declaration))
            {
                Result.Add(StoreAddress as Declaration);
            }
            if (Value.GetType() == typeof(Declaration))
            {
                Result.Add(Value as Declaration);
            }
            return Result;
        }

        public override string ToString()
        {
            Type type;
            if (Value.GetType() == typeof(Declaration))
            {
                type = (Value as Declaration).Type;
            }
            else
            {
                type = (Value as Immediate).Type;
            }
            return "*(" + type + "*)" + StoreAddress + " = " + Value + ";";
        }

		public override string ToAssemblyString()
		{
			Type type;
			if (Value.GetType() == typeof(Declaration))
			{
				type = (Value as Declaration).Type;
			}
			else
			{
				type = (Value as Immediate).Type;
			}
			return "ld " + StoreAddress + "," + Value;
		}
    }
}
