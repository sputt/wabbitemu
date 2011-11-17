using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC;
using System.IO;

namespace WabbitC.Optimizer
{
	static class Optimizer
	{

		static bool ConstantTracking = false;
		static bool DeadCodeOptimization = true;
		static bool DeadVariableRemoval = true;
		static bool VariableReduction = true;
		static bool ReuseVariables = true;
		static bool RemoveCrapMath = true;
		static bool UseCSE = true;
		public static void RunOptimizer(ref Module module, Compiler.OptimizeLevel opLevel)
		{
			Block mainModule = (Block) module;
			if (ConstantTracking)
			    ConstantsOptimizer.Optimize(ref mainModule);
			if (RemoveCrapMath)
				RemoveUselessMath.Optimize(module);
			if (VariableReduction)
				VariableReducer.Optimize(ref module);
			if (DeadCodeOptimization)
				DeadCodeOptimizer.Optimize(ref module);
			if (ReuseVariables) {
				VariableReuse.Optimize(ref module);
				if (DeadCodeOptimization)
					DeadCodeOptimizer.Optimize(ref module);
			}
			if (DeadVariableRemoval)
				DeadVariableOptimizer.Optimize(ref module);
			if (UseCSE)
			{
				bool hasChanged;
				do
				{
					hasChanged = false;
					hasChanged |= CSE.Optimize(ref module);
					if (hasChanged)
					{
						if (ReuseVariables)
						{
							VariableReuse.Optimize(ref module);
							if (DeadCodeOptimization)
								DeadCodeOptimizer.Optimize(ref module);
						}
					}
				}
				while (hasChanged);
				if (DeadVariableRemoval)
					DeadVariableOptimizer.Optimize(ref module);
			}
			
		}

		public static Compiler.OptimizeLevel ParseCommandLine(string arg)
		{
			if (string.IsNullOrEmpty(arg))
				return Compiler.OptimizeLevel.OptimizeNone;
			
			if (arg[0] == '-')
			{	
				switch (arg[1])
				{
					case 'O':
						if (!char.IsDigit(arg[2]))
							throw new InvalidDataException("-O switch does not have an appropriate optimization level");
						int optimizeLevel = int.Parse(arg[2].ToString());
						switch (optimizeLevel)
						{
							case 1:
								return Compiler.OptimizeLevel.OptimizeSome;
							case 2:
								return Compiler.OptimizeLevel.OptimizeAlot;
							case 3:
								return Compiler.OptimizeLevel.OptimizeMax;
						}
						break;
				}
			}
			return Compiler.OptimizeLevel.OptimizeNone;
		}

		public static Compiler.OptimizeLevel ParseCommandLine(string[] args)
		{
			Compiler.OptimizeLevel opLevel = Compiler.OptimizeLevel.OptimizeNone;

			foreach (string arg in args)
			{
				opLevel = ParseCommandLine(arg);
				if (opLevel != Compiler.OptimizeLevel.OptimizeNone)
					break;
			}

			return opLevel;
		}
	}
}
