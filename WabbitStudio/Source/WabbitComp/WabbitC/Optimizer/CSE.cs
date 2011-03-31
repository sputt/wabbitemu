using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;

namespace WabbitC.Optimizer
{
	static class CSE
	{
		public static void Optimize(ref Module module)
        {
            for (int i = 0; i < module.Declarations.Count; i++)
            {
                Declaration decl = module.Declarations[i];
				if (decl.Code != null)
					OptimizeBlock(ref decl.Code);
            }
        }

		public static void OptimizeBlock(ref Block block)
		{
			for (int i = 0; i < block.Statements.Count; i++)
			{
				var statement = block.Statements[i];

			}
		}
	}
}
