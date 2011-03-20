using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;

namespace WabbitC.Optimizer
{
    static class Optimizer
    {
        static bool ConstantTracking = true;
        static bool DeadCodeOptimization = true;
        static bool DeadVariableRemoval = true;
		static bool VariableReduction = true;
        public static void RunOptimizer(ref Module module)
        {
            Block mainModule = (Block) module;
            if (ConstantTracking)
                ConstantsOptimizer.Optimize(ref mainModule);
            if (DeadCodeOptimization)
                DeadCodeOptimizer.Optimize(ref module);
            if (DeadVariableRemoval)
                DeadVariableOptimizer.Optimize(ref module);
			if (VariableReduction)
				VariableReducer.Optimize(ref module);
        }
    }
}
