using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using WabbitC.Model.Types;
using System.Collections;

namespace WabbitC.StatementPasses
{
	class ReplaceLocalsWithGlobals
	{
		static int level;
		public static void Run(Module module)
		{
			level = 0;
			var functions = from d in module.Declarations
							where d.Type is FunctionType && d.Code != null
							select d;
			foreach (var function in functions)
			{
				if (!function.Code.Function.UseStack)
					RecurseCalls(module, functions, function);
				level++;
			}
		}

		static void RecurseCalls(Module module, IEnumerable functions, Declaration function)
		{
			Block block = function.Code;
			var calledFunc = from s in block.Statements
							 where s is FunctionCall
							 select s;
			foreach (FunctionCall func in calledFunc)
			{
				RecurseCalls(module, functions, func.Function);
			}

			ReplaceLocals(module, block);
		}

		static void ReplaceLocals(Module module, Block block)
		{
			foreach (var decl in block.Declarations)
			{
				var newDecl = module.AllocateGlobalVariable(decl, level);
				foreach (var statement in block.Statements)
				{
					statement.ReplaceDeclaration(decl, newDecl);
				}
			}
			block.Declarations.Clear();
		}
	}
}
