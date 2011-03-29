using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC;

namespace WabbitC.Optimizer
{
    static class Optimizer
    {
        static bool ConstantTracking = true;
        static bool DeadCodeOptimization = true;
        static bool DeadVariableRemoval = true;
		static bool VariableReduction = true;
        public static void RunOptimizer(ref Module module, Compiler.OptimizeLevel opLevel)
        {
            Block mainModule = (Block) module;
            if (ConstantTracking)
                ConstantsOptimizer.Optimize(ref mainModule);
			if (VariableReduction)
				VariableReducer.Optimize(ref module);
            if (DeadCodeOptimization)
                DeadCodeOptimizer.Optimize(ref module);
            if (DeadVariableRemoval)
                DeadVariableOptimizer.Optimize(ref module);
        }
    }
}
