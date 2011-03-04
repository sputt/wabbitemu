using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Parser
{
	class DeclarationClass
	{
		readonly TypeClass type;
		public TypeClass Type
		{
			get { return type; }
		}

		readonly ValueClass value;
		public ValueClass Value
		{
			get { return value; }
		}

		public DeclarationClass(TypeClass type, ValueClass value)
		{
			this.type = type;
			this.value = value;
		}
	}
}
