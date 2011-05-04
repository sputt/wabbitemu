using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Statements;
using WabbitC.Optimizer;

namespace WabbitC.Model
{
    class BasicBlock : Block
    {
        public static List<BasicBlock> GetBasicBlocks(Block block)
        {
            var blocks = new List<BasicBlock>();
            var currentBlock = new BasicBlock(block);
            currentBlock.Declarations = block.Declarations;
            currentBlock.Function = block.Function;
            currentBlock.stack = block.stack;
            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
                if (statement.GetType() == typeof(Goto) || statement.GetType() == typeof(Return))
                {
                    blocks.Add(currentBlock);
                    currentBlock.Statements.Add(statement);
                    currentBlock = new BasicBlock(block);
                    currentBlock.Declarations = block.Declarations;
                    currentBlock.Function = block.Function;
                    currentBlock.stack = block.stack;
                }
                else if (statement.GetType() == typeof(Label) && currentBlock.Statements.Count > 0)
                {
                    blocks.Add(currentBlock);
                    currentBlock = new BasicBlock(block);
                    currentBlock.Declarations = block.Declarations;
                    currentBlock.Function = block.Function;
                    currentBlock.stack = block.stack;
                    currentBlock.Statements.Add(statement);
                }
                else
                {
                    currentBlock.Statements.Add(statement);
                }
            }
            if (currentBlock.Statements.Count > 0)
                blocks.Add(currentBlock);

            Block prevBlock = null;
            foreach (var tempblock in blocks)
            {
                if (prevBlock != null)
                    tempblock.EntryPoints.Add(new EntryPoint(prevBlock));
                var lastStatement = tempblock.Statements.Last();
                if (lastStatement.GetType() == typeof(Goto))
                {
                    var gotoStatement = ((Goto)lastStatement);
                    if (gotoStatement.CondDecl == null)
                    {
                        prevBlock = tempblock;
                        foreach (var jumpBlock in blocks)
                        {
                            var label = jumpBlock.Statements.First() as Label;
                            if (label == gotoStatement.TargetLabel)
                                jumpBlock.EntryPoints.Add(new EntryPoint(tempblock));
                        }
                    }
                    else
                    {
                        prevBlock = null;
                    }
                }
                else
                {
                    prevBlock = tempblock;
                }
            }
            LiveAnalysisClass.CalculateInOutVars(blocks);
            return blocks;
        }

        public List<EntryPoint> EntryPoints;
        public IEnumerable<Declaration> OutVars;
        public IEnumerable<Declaration> InVars;

        
        public BasicBlock(Block parent)
        {
            Parent = parent;
            EntryPoints = new List<EntryPoint>();
        }
    }
}
