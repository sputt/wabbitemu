using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    class EntryPoint
    {
        public Block entryPoint;
        public EntryPoint(Block entryBlock)
        {
            entryPoint = entryBlock;
        }
    }
}
