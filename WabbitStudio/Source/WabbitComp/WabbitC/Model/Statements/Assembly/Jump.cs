using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements.Assembly
{
	class Jump : ControlStatement
	{
		public enum Condition
		{
			Z,
			NZ,
			C,
			NC
		};

		Condition condition;
		string label;
		public Jump(string label)
		{
			this.label = label;
		}

		public Jump(string label, Token condOp) : this(label)
		{
			switch (condOp)
			{
				case "==":
					condition = Condition.Z;
					break;
				case "!=":
					condition = Condition.NZ;
					break;
			}
		}

		public override string ToString()
		{
			string jumpString = "jp ";
			if (condition != null)
			{
				switch(condition) 
				{
					case Condition.Z:
						jumpString += "z,";
						break;
				}
			}
			return jumpString + label;
		}
	}
}
