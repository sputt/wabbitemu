using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Statements;
using WabbitC.Optimizer;
using System.Diagnostics;

namespace WabbitC.Model
{
	class BasicBlock : Block
	{
		/// <summary>
		/// Returns the variables live at the end of the block.
		/// </summary>
		public static List<Declaration> GetLiveVariables(Block block)
		{
			LiveChartClass chart = new LiveChartClass(block);
			chart.GenerateVariableChart();
			var liveVars = chart.Where(kvp => kvp.Value.Last() == true).Select(kvp => kvp.Key);
			return liveVars.ToList<Declaration>();
		}

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

			BasicBlock prevBlock = null;
			foreach (var workingBlock in blocks)
			{
				if (prevBlock != null)
				{
					workingBlock.EntryPoints.Add(prevBlock);
					prevBlock.ExitPoints.Add(workingBlock);
				}
				var lastStatement = workingBlock.Statements.Last();
				if (lastStatement is Goto)
				{
					var gotoStatement = ((Goto)lastStatement);
					prevBlock = gotoStatement.CondDecl == null ? null : workingBlock;
					foreach (var jumpBlock in blocks)
					{
						var label = jumpBlock.Statements.First() as Label;
						if (label == gotoStatement.TargetLabel)
						{
							
							jumpBlock.EntryPoints.Add(workingBlock);
							workingBlock.ExitPoints.Add(jumpBlock);
							break;
						}
					}
				}
				else
				{
					prevBlock = workingBlock;
				}
			}
			LiveAnalysisClass.CalculateInOutVars(blocks);
			return blocks;
		}

		public List<BasicBlock> EntryPoints;
		public List<BasicBlock> ExitPoints;
		public IEnumerable<Declaration> OutVars;
		public IEnumerable<Declaration> InVars;

		IEnumerable<Declaration> OldOutVars = null;
		IEnumerable<Declaration> OldInVars = null;

		/// <summary>
		/// Clones the list of InVars allowing them to retain info about their state at the end of the block
		/// </summary>
		public void FreezeInVars()
		{
			var newVars = new List<Declaration>();
			foreach (var inVar in InVars)
			{
				newVars.Add((Declaration)inVar.Clone());   
			}
			OldInVars = InVars;
			InVars = newVars;
		}

		/// <summary>
		/// Restores the InVars to their original state before a freeze. Must have called FreezeInVars previously
		/// </summary>
		public void UnFreezeInVars()
		{
			Debug.Assert(OldInVars != null);
			InVars = OldInVars;
		}

		/// <summary>
		/// Clones the list of OutVars allowing them to retain info about their state at the end of the block
		/// </summary>
		public void FreezeOutVars()
		{
			var newVars = new List<Declaration>();
			foreach (var outVar in OutVars)
			{
				var decl = (Declaration)outVar.Clone();
				newVars.Add(decl);
			}
			OldOutVars = newVars;
		}

		/// <summary>
		/// Restores the OutVars to their original state before a freeze. Must have called FreezeOutVars previously
		/// </summary>
		public void UnFreezeOutVars()
		{
			Debug.Assert(OldOutVars != null);
			OutVars = OldOutVars;
			OldOutVars = null;
		}
		
		public BasicBlock(Block parent)
		{
			Parent = parent;
			EntryPoints = new List<BasicBlock>();
			ExitPoints = new List<BasicBlock>();
		}

		public override string ToString()
		{
			return Statements.ToString();
		}
	}
}
