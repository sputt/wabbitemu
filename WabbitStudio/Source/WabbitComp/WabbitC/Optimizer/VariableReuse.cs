using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using System.Diagnostics;

namespace WabbitC
{
    static class VariableReuse
    {
        static List<OptimizerSymbol> symbolTable = new List<OptimizerSymbol>();
        public static void Optimize(ref Module module)
        {
            for (int i = 0; i < module.Declarations.Count; i++)
            {
                Declaration decl = module.Declarations[i];
				if (decl.Code != null)
					OptimizeBlock(ref decl.Code);
            }
        }

        public static void OptimizeBlock(ref Block block)
        {
			int numVars = 0;
			List<VariableReuseClass> liveChart = new List<VariableReuseClass>();
            for (int i = 0; i < block.Declarations.Count; i++)
            {
				bool varLive = false;
				bool[] chart = GenerateLiveChart(block, block.Declarations[i]);
                foreach (var liveValue in chart)
				{
					varLive |= liveValue;
				}
				if (varLive)
					liveChart.Add(new VariableReuseClass(block.Declarations[i].Name, i, numVars++, chart));
            }
            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
				var modifiedVars = statement.GetModifiedDeclarations();
				if (modifiedVars.Count == 1)
				{
					int j;
					var modified = modifiedVars[0];
					int modIndex = FindVar(modified, ref liveChart);
					//if we cant find, get out of here or if we've used previously, no need to replace
					if (modIndex < 0 || liveChart[modIndex].usedCount > 0)
						continue;
					int endLine = i;
					while (endLine < block.Statements.Count && liveChart[modIndex].livePoints[endLine])
						endLine++;
					endLine--;
					if (endLine < i)
						continue;
					List<int> possibles = new List<int>();
					for (j = 0; j < liveChart.Count; j++ )
					{
						if (block.Declarations[liveChart[modIndex].blockIndex].Type.ToString() == 
							block.Declarations[liveChart[j].blockIndex].Type.ToString())
						{
							if (CompareSections(j, i, endLine, ref liveChart))
								possibles.Add(j);
						}
					}
					if (possibles.Count > 1)
					{
						j = 0;
						//perfer variables that are already used
						foreach (var possible in possibles)
						{
							if (liveChart[possible].usedCount > liveChart[j].usedCount)
								j = possible;
						}
					}
					if (j < liveChart.Count)
					{
						//we've found a variable of the same type and dead on this line
						for (int k = i; k <= endLine; k++)
						{
							var statementToChange = block.Statements[k];
							liveChart[j].livePoints[k] = true;
							liveChart[modIndex].livePoints[k] = false;
							statementToChange.ReplaceDeclaration(block.Declarations[liveChart[modIndex].blockIndex],
									block.Declarations[liveChart[j].blockIndex]);
						}
						liveChart[j].usedCount++;
						bool varLive = false;
						foreach (var liveValue in liveChart[modIndex].livePoints)
						{
							varLive |= liveValue;
						}
						if (!varLive)
							liveChart.RemoveAt(modIndex);
					}
					i = endLine;
				}
            }
        }

		private static bool CompareSections(int compareIndex, int startLine, int endLine, ref List<VariableReuseClass> liveChart)
		{
			for (; startLine < endLine; startLine++)
			{
				if (liveChart[compareIndex].livePoints[startLine])
					return false;
			}
			return true;
		}

		private static int FindVar(Declaration modified, ref List<VariableReuseClass> liveChart)
		{
			for (int i = 0; i < liveChart.Count; i++)
			{
				if (modified.Name == liveChart[i].name)
					return i;
			}
			return -1;
		}

		static bool[] GenerateLiveChart(Block block, Declaration decl)
		{
			int assigned = -1;
			bool[] livePoints = new bool[block.Statements.Count];
			for (int i = 0; i < block.Statements.Count; i++)
			{
				var statement = block.Statements[i];
				var modified = statement.GetModifiedDeclarations();
				if (modified.Contains(decl))
				{
					livePoints[i] = true;
					assigned = i;
				}
				var refed = statement.GetReferencedDeclarations();
				if (refed.Contains(decl))
				{
					for (int j = assigned; j <= i; j++)
						livePoints[j] = true;
				}
			}
			return livePoints;
		}

		class VariableReuseClass
		{
			public string name;
			public int blockIndex;
			public int chartIndex;
			public bool[] livePoints;
			public int usedCount = 0;

			public VariableReuseClass(string name, int i, int varNum, bool[] chart)
			{
				this.name = name;
				this.blockIndex = i;
				this.chartIndex = varNum;
				this.livePoints = chart;
			}

		}

		static OptimizerSymbol FindSymbol(Declaration decl)
        {
            var symbolToFind = OptimizerSymbol.Parse(decl);
            var index = symbolTable.IndexOf(symbolToFind);
            if (index == -1)
                return null;
            return symbolTable[index];
        }
    }    
}
