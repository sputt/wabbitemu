using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC;
using System.IO;

namespace WabbitC.Optimizer.Loop
{
	static class Optimizer
	{
		static bool StrengthReduction = false;
		static bool LoopInvarianceReduction = true;
		public static void RunOptimizer(ref Module module, Compiler.OptimizeLevel opLevel)
		{
            if (WabbitC.Optimizer.Optimizer.ConstantTracking)
            {
                Loop.ConstantsOptimizer.Optimize(ref module);
            }
			if (LoopInvarianceReduction)
			{
				Loop.LoopInvarianceReduction.Optimize(ref module);
			}
			
		}
	}
}
