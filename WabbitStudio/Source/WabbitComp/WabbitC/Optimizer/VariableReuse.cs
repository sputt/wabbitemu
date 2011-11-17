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
			var liveChart = new LiveChartClass(block);
			liveChart.GenerateVariableChart();
			Dictionary<Declaration, int> usedCount = new Dictionary<Declaration, int>();
			foreach (var keyValue in liveChart)
			{
				usedCount[keyValue.Key] = 0;
			}
			for (int i = 0; i < block.Statements.Count; i++)
			{
				var statement = block.Statements[i];
				var modifiedVars = statement.GetModifiedDeclarations().ToList();
				if (modifiedVars.Count == 1)
				{
					var modified = modifiedVars[0];
					List<bool> chart;
					//if we cant find, get out of here or if we've used previously, no need to replace
					if (!liveChart.TryGetValue(modified, out chart) || usedCount[modified] > 0)
					{
						continue;
					}
					int endLine = i;
					while (endLine < block.Statements.Count && chart[endLine])
					{
						endLine++;
					}
					endLine--;
					if (endLine < i)
					{
						continue;
					}
					SortedDictionary<int, Declaration> possibles = new SortedDictionary<int, Declaration>();
					foreach (var possibleVar in liveChart)
					{
						if (possibleVar.Key.Equals(modified) && modified.Type.ToString() == possibleVar.Key.Type.ToString())
						{
							int scoreVal;
							if (liveChart.CompareSections(modified, possibleVar.Key, i, endLine, out scoreVal))
							{
								if (!block.Declarations.Contains(possibleVar.Key))
								{
									//arbitrarily large value
									scoreVal += 100;
								}
								possibles.Add(scoreVal, possibleVar.Key);
							}
						}
					}
					Declaration newDecl = null;
					if (possibles.Count >= 1)
					{
						newDecl = possibles.Max().Value;
				
						//we've found a variable of the same type and dead on this line
						for (int k = i; k <= endLine; k++)
						{
							var statementToChange = block.Statements[k];
							liveChart[newDecl][k] = true;
							liveChart[modified][k] = false;
							statementToChange.ReplaceDeclaration(modified, newDecl);
							//remove test = test; type statements
							if (statementToChange is Move && ((Move)statementToChange).LValue == ((Move)statementToChange).RValue)
							{
								block.Statements.Remove(statementToChange);
								foreach (var varToChange in liveChart)
								{
									varToChange.Value.RemoveAt(k);
								}
								endLine--;
								k--;
							}
						}
						usedCount[newDecl]++;
						bool varLive = false;
						foreach (var liveValue in liveChart[modified])
						{
							varLive |= liveValue;
						}
						if (!varLive)
						{
							liveChart.Remove(modified);
						}
					}
					//i = endLine;
				}
			}
		}
	}    
}
