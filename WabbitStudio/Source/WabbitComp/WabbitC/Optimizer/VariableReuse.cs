using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using System.Diagnostics;

namespace WabbitC.Optimizer
{
    static class VariableReuse
    {
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
				var modifiedVars = statement.GetModifiedDeclarations().ToList();
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
							if (statementToChange is Move && ((Move)statementToChange).LValue == ((Move)statementToChange).RValue)
							{
								block.Statements.Remove(statementToChange);
								for (int index = 0; index < liveChart.Count; index++)
									liveChart[index].livePoints.RemoveAt(k);
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
    }    
}
