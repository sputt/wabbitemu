using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model;

namespace WabbitC.StatementPasses
{
    class LoopGotoConversion
    {
        public static void ConvertLoops(Block block)
        {
            IEnumerator<Block> blocks = new BlockEnumerator(block);

            while (blocks.MoveNext())
            {
                // Do something
                Debug.WriteLine(blocks.Current.Statements[0]);
            }
        }
    }
}
