using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    class EntryPoint
    {
        public BasicBlock entryPoint;
        public EntryPoint(BasicBlock entryBlock)
        {
            entryPoint = entryBlock;
        }

        public override string ToString()
        {
            return entryPoint.Statements.ToString();
        }
    }
}
