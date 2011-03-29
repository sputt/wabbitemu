using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;
using System.Diagnostics;

namespace WabbitC.StatementPasses
{
    static class AddStackDeclaration
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    Declaration lastDecl = functions.Current.Code.Declarations.Last<Declaration>();
                    int stackSize = lastDecl.StackOffset + lastDecl.Type.Size;
					Debug.Print("{0}", stackSize);
                    functions.Current.Code.Declarations.Insert(0, new StackDeclaration(stackSize));
                }
            }
        }
    }
}
