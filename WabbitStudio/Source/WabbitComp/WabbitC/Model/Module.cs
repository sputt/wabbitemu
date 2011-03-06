using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;

namespace WabbitC.Model
{
    class Module : Block
    {
        private Module(Block block)
        {
            this.Types = block.Types;
            this.Declarations = block.Declarations;
            this.Parent = null;
        }
        static public Module ParseModule(ref List<Token>.Enumerator tokens)
        {
            var block = Block.ParseBlock(ref tokens, null);
            // Make sure there's no stuff that doesn't belong in module
            return new Module(block);
        }
    }
}
