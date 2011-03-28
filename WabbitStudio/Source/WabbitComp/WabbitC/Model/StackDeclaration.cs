using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model
{
    class StackDeclaration : Declaration
    {
        public StackDeclaration(int stackSize)
        {
            Name = "__stack";
            Type = new Types.Array(new BuiltInType("unsigned char"), "[" + stackSize + "]");
        }
    }
}
