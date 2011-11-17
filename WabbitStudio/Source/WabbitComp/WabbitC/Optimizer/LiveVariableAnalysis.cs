using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements.Math;
using System.Collections;

namespace WabbitC.Optimizer
{
	class LiveChartClass : Dictionary<Declaration, List<bool>>
	{
		readonly Block block;
		public LiveChartClass(Block block)
		{
			this.block = block;
		}

		public void GenerateVariableChart(bool addAllVars = false)
		{
			var assignedList = new Dictionary<Declaration, int>();
			for (int i = 0; i < block.Statements.Count; i++)
			{
				var statement = block.Statements[i];
				var modified = statement.GetModifiedDeclarations();
				foreach (var modifiedVar in modified)
				{
					if (!this.ContainsKey(modifiedVar))
					{
						var newList = new List<bool>();
						for (int j = 0; j < block.Statements.Count; j++)
						{
							newList.Add(false);
						}
						this.Add(modifiedVar, newList);
					}
					if (!(statement is MathStatement))
					{
						assignedList[modifiedVar] = i;
					}
				}
				var refed = statement.GetReferencedDeclarations();
				foreach (var refedVar in refed)
				{
					if (!this.ContainsKey(refedVar))
					{
						var newList = new List<bool>();
						for (int j = 0; j < block.Statements.Count; j++)
						{
							newList.Add(false);
						}
						this.Add(refedVar, newList);
					}

					//assume if -1 that it was live coming in
					if (!assignedList.ContainsKey(refedVar) || assignedList[refedVar] == -1)
					{
						assignedList[refedVar] = 0;
					}
					for (int j = assignedList[refedVar]; j <= i; j++)
					{
						this[refedVar][j] = true;
					}
				}
			}
		}

		public bool CompareSections(Declaration decl, Declaration comparerDecl, int startLine, int endLine, out int score)
		{
			score = -1;
			for (; startLine < endLine; startLine++)
			{
				if (this[decl][startLine])
				{
					var mod = block.Statements[startLine].GetModifiedDeclarations().ToList();
					var refed = block.Statements[startLine].GetReferencedDeclarations().ToList();
					if (mod.Count > 0 && refed.Count > 0 && ((mod[mod.Count - 1] == decl
						&& refed[refed.Count - 1] == decl)))
						continue;
					return false;
				}
			}
			score = this[decl].Count;
			for (int i = 0; i < this[decl].Count; i++)
			{
				if (this[comparerDecl][i] != this[decl][i])
				{
					score--;
				}
			}
			return true;
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
			block.InVars = genVars.Union(block.OutVars.Except(killVars)).ToList();
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
								   //where !(statement is MathStatement)
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
								   //where !(statement is MathStatement)
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
		public Declaration Declaration;
		public List<bool> livePoints;
		public int usedCount = 0;

		public VariableReuseClass(Declaration decl, int i, int varNum, List<bool> chart)
		{
			this.Declaration = decl;
			this.livePoints = chart;
		}
		
		public override string ToString()
		{
			return Declaration.Name;
		}
	}
}
