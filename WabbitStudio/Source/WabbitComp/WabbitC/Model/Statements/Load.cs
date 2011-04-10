using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Load : ValueStatement
    {
        public Declaration LValue;
        public Datum LoadAddress;

        public Load(Declaration LValue, Datum loadAddr)
        {
            this.LValue = LValue;
            this.LoadAddress = loadAddr;
        }


        public override ISet<Declaration> GetModifiedDeclarations()
        {
			return new HashSet<Declaration>() { LValue };
        }

        public override ISet<Declaration> GetReferencedDeclarations()
        {
			var Result = new HashSet<Declaration>();
            if (LoadAddress.GetType() == typeof(Declaration))
            {
                Result.Add(LoadAddress as Declaration);
            }
            return Result;
        }

        public override string ToString()
        {
            return this.LValue.Name + " = *(" + LValue.Type + "*) " + this.LoadAddress.ToString() + ";";
        }

		public override string ToAssemblyString()
		{
			return "ld " + LValue.Name + "(" + LoadAddress.ToString() + ")";
		}
    }
}
