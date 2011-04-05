using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
	class BlockCopyReverse : Statement
	{
		public BlockCopyReverse()
		{
		}

		public override string ToString()
		{
			return "memmove((unsigned char *) __de, (unsigned char *) __hl, __bc);";
		}

		public override string ToAssemblyString()
		{
			return "lddr";
		}

	}
}
