using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Types;
using WabbitC.Model.Statements;

namespace WabbitC.StatementPasses
{
    static class MarkRecursiveFunctions
    {
		public static void Run(Module module)
        {
            var functions = from d in module.Declarations 
                            where d.Type.GetType() == typeof(FunctionType) && d.Code != null
                            select d;
            foreach (var decl in functions)
            {
				var funcCalls = from func in decl.Code.Statements
								where func.GetType() == typeof(FunctionCall)
								select func;
				foreach (FunctionCall call in funcCalls)
				{
					if (call.Function == decl)
					{
						if (!decl.Code.Function.UseStack)
							MessageSystem.Instance.ThrowNewWarning("Recursive function, forcing use of stack variables");
						decl.Code.Function.IsRecursive = true;
					}
				}
            }
        }
    }
}
