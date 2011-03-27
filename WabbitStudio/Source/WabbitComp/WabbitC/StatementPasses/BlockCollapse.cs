using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
    static class BlockCollapse
    {
        private static void RemoveAndRenameDeclarations(Block blockParent, Block block, int suffix)
        {
            for (int i = 0; i < block.Declarations.Count; i++)
            {
                block.Declarations[i].Name = block.Declarations[i].Name + "_" + suffix.ToString();
                if (block != blockParent)
                {
                    blockParent.Declarations.Add(block.Declarations[i]);
                }
            }
            if (block != blockParent)
            {
                block.Declarations.Clear();
            }
        }

        public static void Run(Module module)
        {
            var functions = from d in module.Declarations 
                            where d.Type.GetType() == typeof(FunctionType) && d.Code != null
                            select d.Code;
            foreach (Block block in functions)
            {
                Block parentBlock = block;
                var blocks = from Block b in parentBlock select b;
                int nBlockCount = 0;
                foreach (var childBlock in blocks)
                {
                    RemoveAndRenameDeclarations(parentBlock, childBlock, nBlockCount);
                    nBlockCount++;
                }
            }
        }
    }
}
