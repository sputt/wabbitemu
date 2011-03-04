using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Parser
{
	public class ValueClass
	{
		readonly TypeClass type;
		public TypeClass Type
		{
			get { return type; }
		}

		public ValueClass(TypeClass type)
		{
			this.type = type;
		}
	}
}
