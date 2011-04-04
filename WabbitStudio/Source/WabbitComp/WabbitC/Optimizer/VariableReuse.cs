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
			var liveChart =  new LiveChartClass(block);
			liveChart.GenerateVariableChart();
            for (int i = 0; i < block.Statements.Count; i++)
            {
                var statement = block.Statements[i];
				var modifiedVars = statement.GetModifiedDeclarations();
				if (modifiedVars.Count == 1)
				{
					int j;
					var modified = modifiedVars[0];
					int modIndex = liveChart.FindVar(modified);
					//pst hi i love you
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
					List<int> score = new List<int>();
					for (j = 0; j < liveChart.Count; j++ )
					{
						if (j != modIndex && block.Declarations[liveChart[modIndex].blockIndex].Type.ToString() == 
							block.Declarations[liveChart[j].blockIndex].Type.ToString())
						{
							int scoreVal;
							if (liveChart.CompareSections(j, modIndex, i, endLine, out scoreVal))
							{
								possibles.Add(j);
								score.Add(scoreVal);
							}
						}
					}
					Declaration oldDecl = null, newDecl = null;
					if (possibles.Count >= 1)
					{
						j = possibles[score.IndexOf(score.Max())];
						oldDecl = liveChart[modIndex].decl;
						newDecl = liveChart[j].decl;
					}
					if (j < liveChart.Count)
					{
						
						//we've found a variable of the same type and dead on this line
						for (int k = i; k <= endLine; k++)
						{
							var statementToChange = block.Statements[k];
							liveChart[j].livePoints[k] = true;
							liveChart[modIndex].livePoints[k] = false;
							statementToChange.ReplaceDeclaration(oldDecl, newDecl);
							//remove test = test; type statements
							if (statementToChange.GetType() == typeof(Move) && ((Move)statementToChange).LValue == ((Move)statementToChange).RValue)
							{
								block.Statements.Remove(statementToChange);
								endLine--;
								k--;
							}
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
					//i = endLine;
				}
            }
        }

		public class LiveChartClass : List<VariableReuseClass>
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
						bool[] chart = GenerateLiveChart(block, block.Function.Params[i]);
						foreach (var liveValue in chart)
							varLive |= liveValue;
						if (varLive)
							this.Add(new VariableReuseClass(block.Function.Params[i], i, numVars++, chart));
					}
				}
				for (int i = 0; i < block.Declarations.Count; i++)
				{
					bool varLive = false;
					bool[] chart = GenerateLiveChart(block, block.Declarations[i]);
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
						var mod = block.Statements[startLine].GetModifiedDeclarations();
						var refed = block.Statements[startLine].GetReferencedDeclarations();
						if (mod.Count > 0 && mod.Count > 0 && (mod[mod.Count - 1] == this[compareIndex].decl
							&& refed[refed.Count - 1] == this[modIndex].decl) || (mod[mod.Count - 1] == this[modIndex].decl
							&& refed[refed.Count - 1] == this[compareIndex].decl))
							continue;
						return false;
					}
				}
				score = this[0].livePoints.Length;
				for (int i = 0; i < this[0].livePoints.Length; i++)
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

			bool[] GenerateLiveChart(Block block, Declaration decl)
			{
				int assigned = -1;
				bool[] livePoints = new bool[block.Statements.Count];
				for (int i = 0; i < block.Statements.Count; i++)
				{
					var statement = block.Statements[i];
					var modified = statement.GetModifiedDeclarations();
					if (modified.Contains(decl) && statement.GetType().BaseType != typeof(MathStatement))
					{
						livePoints[i] = true;
						assigned = i;
					}
					if (assigned == -1 && block.FindDeclaration(decl.Name) != null)
					{
						assigned = 0;
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
		}

		public class VariableReuseClass
		{
			public Declaration decl;
			public int blockIndex;
			public int chartIndex;
			public bool[] livePoints;
			public int usedCount = 0;

			public VariableReuseClass(Declaration decl, int i, int varNum, bool[] chart)
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
}
