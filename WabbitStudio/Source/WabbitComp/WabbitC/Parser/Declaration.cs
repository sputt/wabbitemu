using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Parser
{
	public class Declaration
	{
		TypeClass type;
		ValueClass initialValue;
		public TypeClass Type { get { return type; } }
		public ValueClass InitialValue { get { return initialValue; } }

		public Declaration(TypeClass type, ValueClass initialValue)
		{
			this.type = type;
			this.initialValue = initialValue;
		}
	}
}
