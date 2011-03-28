using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
    class StackAllocator
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    int nOffset = 0;
                    foreach (Declaration decl in (functions.Current.Type as FunctionType).Params)
                    {
                        decl.StackOffset = nOffset;
                        nOffset += decl.Type.Size;
                    }

                    foreach (Declaration decl in functions.Current.Code.Declarations)
                    {
                        decl.StackOffset = nOffset;
                        nOffset += decl.Type.Size;
                    }
                }
            }
        }
    }
}
