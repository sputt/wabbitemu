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
                {
                    RecurseCalls(module, functions, function);
                }
				level++;
			}
		}

		static List<Declaration> RecurseCalls(Module module, IEnumerable functions, Declaration function)
		{
			Block block = function.Code;
			var calledFunc = from s in block.Statements
							 where s is FunctionCall
							 select s;
			foreach (FunctionCall func in calledFunc)
			{
				RecurseCalls(module, functions, func.Function);
                block.GlobalVars.AddRange(func.Function.Code.GlobalVars);
			}

			return ReplaceLocals(module, block);
		}

		static List<Declaration> ReplaceLocals(Module module, Block block)
		{
            var globalsAdded = new List<Declaration>();
			foreach (var decl in block.Declarations)
			{
				var newDecl = module.AllocateGlobalVariable(decl, block);
                globalsAdded.Add(decl);
				foreach (var statement in block.Statements)
				{
					statement.ReplaceDeclaration(decl, newDecl);
				}
			}
			block.Declarations.Clear();
            return globalsAdded;
		}
	}
}
