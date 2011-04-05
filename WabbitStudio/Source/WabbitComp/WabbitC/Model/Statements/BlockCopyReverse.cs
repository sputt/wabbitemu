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
			var sb = new StringBuilder();
			sb.AppendLine("memmove((unsigned char *) __de - __bc + 1, (unsigned char *) __hl - __bc + 1, __bc);");
			sb.AppendLine("__hl -= __bc;");
			sb.AppendLine("__de -= __bc;");
			sb.AppendLine("__bc = 0;");
			return sb.ToString();
		}

		public override string ToAssemblyString()
		{
			return "lddr";
		}

	}
}
