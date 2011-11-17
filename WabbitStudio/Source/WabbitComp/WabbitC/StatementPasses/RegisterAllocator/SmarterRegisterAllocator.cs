using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;
using System.Diagnostics;
using WabbitC.Optimizer;

namespace WabbitC.StatementPasses.RegisterAllocator
{
	static class SmarterRegisterAllocator
	{
		public static void Run(Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				var basicBlocks = BasicBlock.GetBasicBlocks(block);
				var irGraphs = BuildInterferenceGraph(basicBlocks);

				//block.Statements.Clear();
				//for (int i = 0; i < basicBlocks.Count; i++)
				//{
				//    Block basicBlock = basicBlocks[i];
				//    AllocateBlock(ref basicBlock);
				//    block.Statements.AddRange(basicBlock.Statements);
				//}

				//block.Statements.Insert(0, new StackFrameInit(block, block.stack.Size));
				//block.Statements.Add(new StackFrameCleanup(block, block.stack.Size));

				//block.Declarations.Clear();
			}
		}

		static List<Dictionary<Declaration, ISet<Declaration>>> BuildInterferenceGraph(List<BasicBlock> blocks)
		{
			var graphs = new List<Dictionary<Declaration, ISet<Declaration>>>();
			foreach (var block in blocks)
			{
				var graph = new Dictionary<Declaration, ISet<Declaration>>();
				LiveChartClass liveChart = new LiveChartClass(block);
				liveChart.GenerateVariableChart(true);
				foreach (var keyValuePair in liveChart)
				{
					foreach (var declToCompare in liveChart)
					{
						if (keyValuePair.Equals(declToCompare))
							continue;
						bool constructEdge = false;
						for (int i = 0; i < keyValuePair.Value.Count; i++)
						{
							if (keyValuePair.Value[i] && declToCompare.Value[i])
							{
								constructEdge = true;
							}
						}
						if (constructEdge)
						{
							//we don't check if these are flipped so we store 2x extra data
							//i think this will be good later so we don't have to do two lookups
							//just need to remember to treat them as the same
							ISet<Declaration> interferenceNodes;
							if (!graph.TryGetValue(keyValuePair.Key, out interferenceNodes))
							{
								interferenceNodes = new HashSet<Declaration>();
								graph[keyValuePair.Key] = interferenceNodes;
							}
							interferenceNodes.Add(declToCompare.Key);
						}
					}
				}
				graphs.Add(graph);
			}

			return graphs;
		}

		static void AllocateBlock(ref Block block)
		{
			RegisterHelper helper = new RegisterHelper(block);
			var statements = from Statement st in block select st;
			foreach (Statement statement in statements)
			{
				int index = block.Statements.IndexOf(statement);
				block.Statements.Remove(statement);

				var newStatements = new List<Statement>();

				foreach (var decl in statement.GetReferencedDeclarations().Union(statement.GetModifiedDeclarations()))
				{
					if (block.Module.Registers.Contains(decl))
					{
						helper.ReserveRegister(decl);
					}
				}

				foreach (var decl in statement.GetReferencedDeclarations().Union(statement.GetModifiedDeclarations()))
				{
					var allocStatements = new List<Statement>();
					Declaration reg = helper.AllocateRegister(decl, ref allocStatements);
					Debug.Assert(decl != null);
					Debug.Assert(reg != null);

					if (reg != decl && statement.GetReferencedDeclarations().Contains(decl))
					{
						newStatements.Add(new StackLoad(reg, decl));
					}
					newStatements.AddRange(allocStatements);
					statement.ReplaceDeclaration(decl, reg);
				}

				newStatements.Add(statement);

				foreach (var decl in statement.GetModifiedDeclarations())
				{
					Declaration slot = helper.GetAssignedVariable(decl);
					Debug.Assert(slot != null);
					Debug.Assert(decl != null);
					if (slot != decl)
					{
						newStatements.Add(new StackStore(slot, decl));
					}
				}

				block.Statements.InsertRange(index, newStatements);

				foreach (var decl in statement.GetReferencedDeclarations().Union(statement.GetModifiedDeclarations()))
				{
					helper.FreeRegister(decl);
				}
			}
		}

		/*public static void Run(Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				if (functions.Current.Code != null)
				{
					Block block = functions.Current.Code;
					registerStates = new List<RegisterContentState>();
					var blocks = block.GetBasicBlocks();
					block.Statements.Clear();

					for (int i = 0; i < blocks.Count; i++)
					{
						AllocateBlock(ref module, blocks[i]);
						block.Statements.AddRange(blocks[i]);
					}
					
					foreach (Declaration decl in block.Declarations)
					{
						var Array = decl.Type as WabbitC.Model.Types.Array;
						if (Array != null)
						{
							block.stack.ReserveSpace(decl);
						}
					}
					block.Declarations.Clear();

					Debug.Print("{0}", block.stack.Size);
					
					functions.Current.Code.Statements.Insert(0, new StackFrameInit(block, block.stack.Size));
					functions.Current.Code.Statements.Add(new StackFrameCleanup(block, block.stack.Size));
				}
			}
		}

		static List<RegisterContentState> registerStates;

		public static void AllocateBlock(ref Module module, Block block)
		{

			var liveChart = new WabbitC.Optimizer.VariableReuse.LiveChartClass(block);
			liveChart.GenerateVariableChart();
			var RegistersAvailable = new List<Declaration>
					{
						module.FindDeclaration("__de"),
						module.FindDeclaration("__bc"),
					};
			var RegisterContents = new List<Datum>
					{
						null,
						null,
						null
					};

			var CurrentMappings = new List<KeyValuePair<Declaration, Declaration>>();

			int adjustedPos = 0;
			for (int nPos = 0; nPos < block.Statements.Count; nPos++, adjustedPos++)
			{
				Statement statement = block.Statements[nPos];
				block.Statements.Remove(statement);
				for (int i = 0; i < RegisterContents.Count; i++)
				{
					var content = RegisterContents[i] as Declaration;
					if (content != null)
					{
						var index = liveChart.FindVar(content);
						if (index != -1 && liveChart[index].livePoints[adjustedPos] == false)
							RegisterContents[i] = null;
					}
				}

				List<Statement> replacementList = new List<Statement>();

				List<Declaration> usedLValues = statement.GetModifiedDeclarations().ToList<Declaration>();
				List<Declaration> usedDecls = statement.GetReferencedDeclarations().ToList<Declaration>();

				if (usedLValues.Count == 1)
				{
					if (usedLValues[0] == RegisterContents[0] || RegisterContents[0] == null)
					{
						var test = block.Declarations.Contains(usedLValues[0]);
						if (test == false && block.FindDeclaration("__hl") != usedLValues[0] && !RegistersAvailable.Contains(usedLValues[0]))
						{
							block.stack.GetOffset(usedLValues[0]);
							replacementList.Add(new StackLoad(module.FindDeclaration("__hl"), usedLValues[0]));
						}
						RegisterContents[0] = usedLValues[0];
						statement.ReplaceDeclaration(usedLValues[0], module.FindDeclaration("__hl"));
					}
					else
					{
						int index = RegisterContents.IndexOf(null);
						//the last part of that is added to make sure we dont try to load an old value from a register
						//mainly due to variable reuse, because I made it so awesome
						if (index == -1 || statement.GetType() == typeof(Move))
						{
							if (usedDecls.Count > 0 && block.Declarations.Contains(usedDecls[0]))
							{
								block.stack.ReserveSpace(usedDecls[0]);
								var store = new StackStore(usedDecls[0], module.FindDeclaration("__hl"));
								replacementList.Add(store);
							}
							if (block.Declarations.Contains(usedLValues[0]))
							{
								var saveDecl = RegisterContents[0] as Declaration;
								bool regSwitch = false;
								for (int j = 1; j < RegisterContents.Count && !regSwitch; j++)
								{
									if (RegisterContents[j] == null)
									{
										RegisterContents[j] = saveDecl;
										replacementList.Add(new Move(RegistersAvailable[j - 1], module.FindDeclaration("__hl")));
										regSwitch = true;
									}
								}
								if (!regSwitch)
								{
									block.stack.ReserveSpace(saveDecl);
									var store = new StackStore(saveDecl, module.FindDeclaration("__hl"));
									replacementList.Add(store);
								}
							}
							statement.ReplaceDeclaration(usedLValues[0], module.FindDeclaration("__hl"));
							RegisterContents[0] = usedLValues[0];
						}
						else
						{
							statement.ReplaceDeclaration(usedLValues[0], RegistersAvailable[index - 1]);
							RegisterContents[index] = usedLValues[0];
						}
					}
				}

				for (int i = 0; i < usedDecls.Count; i++)
				{
					bool fSkip = false;
					if (usedLValues.Count == 1)
					{
						if (usedLValues[0] == usedDecls[i])
						{
							fSkip = true;
							if (RegisterContents[0] != usedDecls[i])
							{
								replacementList.Add(new StackLoad(module.FindDeclaration("__hl"), usedDecls[i]));
							}
						}
					}
					if (fSkip == false)
					{
						if (block.FindDeclaration("__hl") != usedDecls[i] && !RegistersAvailable.Contains(usedDecls[i]))
						{
							bool alreadyInRegister = false;
							for (int j = 0; j < RegisterContents.Count && !alreadyInRegister; j++)
							{
								if (RegisterContents[j] == usedDecls[i])
								{
									var decl = j == 0 ? module.FindDeclaration("__hl") : RegistersAvailable[j - 1];
									statement.ReplaceDeclaration(usedDecls[i], decl);
									alreadyInRegister = true;
								}
							}

							if (!alreadyInRegister)
							{
								replacementList.Add(new StackLoad(RegistersAvailable[i], usedDecls[i]));
								RegisterContents[i + 1] = usedDecls[i];
								statement.ReplaceDeclaration(usedDecls[i], RegistersAvailable[i]);
							}
						}
					}
				}

				if (statement != null)
					replacementList.Add(statement);

				block.Statements.InsertRange(nPos, replacementList);
				nPos += replacementList.Count - 1;
			}
		}*/
	}
}
