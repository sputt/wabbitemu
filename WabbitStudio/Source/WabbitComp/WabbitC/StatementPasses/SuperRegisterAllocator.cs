using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
    static class SuperRegisterAllocator
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    Block block = functions.Current.Code;
                    

                    // copy the params to the stack (on z80 these will already be on the stack)
                    foreach (Declaration param in (functions.Current.Type as FunctionType).Params)
                    {
						var store = new StackStore(param, param);
                        block.Statements.Insert(0, store);
                    }
                }
            }
        }
    }
}
