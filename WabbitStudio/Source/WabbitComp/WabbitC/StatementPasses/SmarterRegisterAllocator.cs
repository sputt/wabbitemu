using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;
using System.Diagnostics;

namespace WabbitC.StatementPasses
{
    static class SmarterRegisterAllocator
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    Block block = functions.Current.Code;
					StackAllocator stack = block.stack;
					registerStates = new List<RegisterContentState>();
					var blocks = block.GetBasicBlocks();
					block.Statements.Clear();

					for (int i = 0; i < blocks.Count; i++)
					{
						AllocateBlock(ref module, ref stack, blocks[i]);
						block.Statements.AddRange(blocks[i]);
					}
					
					foreach (Declaration decl in block.Declarations)
					{
						var Array = decl.Type as WabbitC.Model.Types.Array;
						if (Array != null)
						{
							stack.ReserveSpace(decl);
						}
					}
                    block.Declarations.Clear();

					Debug.Print("{0}", stack.Size);
					
					functions.Current.Code.Statements.Insert(0, new StackFrameInit(block, stack.Size));
					functions.Current.Code.Statements.Add(new StackFrameCleanup(block, stack.Size));
                }
            }
        }

		static List<RegisterContentState> registerStates;

		public static void AllocateBlock(ref Module module, ref StackAllocator stack, Block block)
		{

			var liveChart = new VariableReuse.LiveChartClass(block);
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
				if (statement.GetType() == typeof(Push))
					continue;
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

				var usedLValues = statement.GetModifiedDeclarations();
				List<Declaration> usedDecls = statement.GetReferencedDeclarations();

				if (usedLValues.Count == 1)
				{
					if (usedLValues[0] == RegisterContents[0] || RegisterContents[0] == null)
					{
						var test = block.Declarations.Contains(usedLValues[0]);
						if (test == false)
						{
							stack.GetOffset(usedLValues[0]);
							replacementList.Add(new StackLoad(module.FindDeclaration("__hl"), usedLValues[0]));
						}
						RegisterContents[0] = usedLValues[0];
						statement.ReplaceDeclaration(usedLValues[0], module.FindDeclaration("__hl"));
					}
					else
					{
						int index = RegisterContents.IndexOf(null);
						if (index == -1)
						{
							if (usedDecls.Count > 0)
							{
								stack.ReserveSpace(usedDecls[0]);
								var store = new StackStore(usedDecls[0], module.FindDeclaration("__hl"));
								replacementList.Add(store);
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
						bool alreadyInRegister = false;
						for (int j = 0; j < RegisterContents.Count && !alreadyInRegister; j++)
						{
							if (RegisterContents[j] == usedDecls[i])
							{
								var decl = j == 0 ? module.FindDeclaration("__hl") : RegistersAvailable[j - 1];
								if (usedDecls[i] == RegisterContents[j])
								{
									block.Statements.Remove(statement);
									statement = null;
								}
								else
								{
									statement.ReplaceDeclaration(usedDecls[i], decl);
								}
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

				if (statement != null)
					replacementList.Add(statement);

				block.Statements.InsertRange(nPos, replacementList);
				nPos += replacementList.Count - 1;
			}
		}
    }

	class RegisterContentState
	{
		public Block targetBlock;
		public List<Datum> registerContents;
		public RegisterContentState(Block block, List<Datum> registers)
		{
			targetBlock = block;
			registerContents = registers;
		}
	}
}
