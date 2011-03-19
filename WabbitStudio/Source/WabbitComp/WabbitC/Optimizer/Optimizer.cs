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
        public static void RunOptimizer(ref Module module)
        {
            Block mainModule = (Block) module;
            if (ConstantTracking)
                ConstantsOptimizer.Optimize(ref mainModule);
            if (DeadCodeOptimization)
                DeadCodeOptimizer.Optimize(ref module);
        }
    }
}
