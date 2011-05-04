using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements.Math;

namespace WabbitC.Optimizer
{
    class LiveChartClass : List<VariableReuseClass>
    {
        readonly Block block;
        public LiveChartClass(Block block)
        {
            this.block = block;
        }

        public void GenerateVariableChart()
        {
            int numVars = 0;
            if (block.Function != null)
            {
                for (int i = 0; i < block.Function.Params.Count; i++)
                {
                    bool varLive = false;
                    var chart = GenerateLiveChart(block, block.Function.Params[i]);
                    foreach (var liveValue in chart)
                        varLive |= liveValue;
                    if (varLive)
                        this.Add(new VariableReuseClass(block.Function.Params[i], i, numVars++, chart));
                }
            }
            for (int i = 0; i < block.Declarations.Count; i++)
            {
                bool varLive = false;
                var chart = GenerateLiveChart(block, block.Declarations[i]);
                foreach (var liveValue in chart)
                    varLive |= liveValue;
                if (varLive)
                    this.Add(new VariableReuseClass(block.Declarations[i], i, numVars++, chart));
            }
        }

        public bool CompareSections(int compareIndex, int modIndex, int startLine, int endLine, out int score)
        {
            score = -1;
            for (; startLine < endLine; startLine++)
            {
                if (this[compareIndex].livePoints[startLine])
                {
                    var mod = block.Statements[startLine].GetModifiedDeclarations().ToList();
                    var refed = block.Statements[startLine].GetReferencedDeclarations().ToList();
                    if (mod.Count > 0 && refed.Count > 0 && ((mod[mod.Count - 1] == this[compareIndex].decl
                        && refed[refed.Count - 1] == this[modIndex].decl) || (mod[mod.Count - 1] == this[modIndex].decl
                        && refed[refed.Count - 1] == this[compareIndex].decl)))
                        continue;
                    return false;
                }
            }
            score = this[0].livePoints.Count;
            for (int i = 0; i < this[0].livePoints.Count; i++)
            {
                if (this[compareIndex].livePoints[i] != this[modIndex].livePoints[i])
                    score--;
            }
            return true;
        }

        public int FindVar(Declaration modified)
        {
            for (int i = 0; i < this.Count; i++)
            {
                if (modified == this[i].decl)
                    return i;
            }
            return -1;
        }

        List<bool> GenerateLiveChart(Block block, Declaration decl)
        {
            int assigned = -1;
            var livePoints = new List<bool>();
            for (int i = 0; i < block.Statements.Count; i++)
                livePoints.Add(false);

            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
                var modified = statement.GetModifiedDeclarations();
                if (modified.Contains(decl) && statement.GetType().BaseType != typeof(MathStatement))
                    assigned = i;

                var refed = statement.GetReferencedDeclarations();
                if (refed.Contains(decl))
                {
                    //assume if -1 that it was live coming in
                    if (assigned == -1)
                        assigned = 0;
                    for (int j = assigned; j <= i; j++)
                        livePoints[j] = true;
                }
            }
            return livePoints;
        }
    }
    class LiveAnalysisClass
    {
        public static void CalculateInOutVars(List<BasicBlock> blocks)
        {
            for (int i = blocks.Count - 1; i >= 0; i--)
            {
                LiveIn(blocks, blocks[i]);
            }
        }

        public static IEnumerable<Declaration> LiveIn(List<BasicBlock> blocks, BasicBlock block)
        {
            var genVars = Gen(blocks, block);
            var killVars = Kill(blocks, block);
            block.OutVars = LiveOut(blocks, block);
            block.InVars = genVars.Union(block.OutVars.Except(killVars));
            return block.InVars;
        }

        public static ISet<Declaration> LiveOut(List<BasicBlock> blocks, BasicBlock block)
        {
            var outVars = new HashSet<Declaration>();
            int index = blocks.IndexOf(block);
            if (index == blocks.Count - 1)
                return outVars;
            for (int i = index + 1; i < blocks.Count; i++)
            {
                outVars.UnionWith(LiveIn(blocks, blocks[i]));
            }
            return outVars;
        }

        static ISet<Declaration> Gen(List<BasicBlock> blocks, BasicBlock block)
        {
            var genVars = new HashSet<Declaration>();
            foreach (var decl in block.Declarations)
            {
                foreach (var statement in block.Statements)
                {
                    var refed = statement.GetReferencedDeclarations();
                    foreach (var varRefed in refed)
                    {
                        if (!genVars.Contains(varRefed) && !varRefed.Properties.Contains("assigned"))
                            genVars.Add(varRefed);

                    }
                    var modified = from s in statement.GetModifiedDeclarations()
                                   where s.GetType().BaseType != typeof(MathStatement)
                                   select s;
                    foreach (var mod in modified)
                    {
                        if (!mod.Properties.Contains("assigned"))
                            mod.Properties.Add("assigned");
                    }
                }
            }
            foreach (var decl in block.Declarations)
                decl.Properties.Remove("assigned");
            
            return genVars;
        }

        static ISet<Declaration> Kill(List<BasicBlock> blocks, BasicBlock block)
        {
            var killVars = new HashSet<Declaration>();
            foreach (var decl in block.Declarations)
            {
                foreach (var statement in block.Statements)
                {
                    var modified = from s in statement.GetModifiedDeclarations()
                                   where s.GetType().BaseType != typeof(MathStatement)
                                   select s;
                    foreach (var mod in modified)
                        killVars.Add(mod);
                }
            }

            return killVars;
        }

    }

    class VariableReuseClass
    {
        public Declaration decl;
        public int blockIndex;
        public int chartIndex;
        public List<bool> livePoints;
        public int usedCount = 0;

        public VariableReuseClass(Declaration decl, int i, int varNum, List<bool> chart)
        {
            this.decl = decl;
            this.blockIndex = i;
            this.chartIndex = varNum;
            this.livePoints = chart;
        }
        
        public override string ToString()
        {
            return decl.Name;
        }
    }
}
