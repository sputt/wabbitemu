using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;

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

					Declaration lastDecl = functions.Current.Code.Declarations.Last<Declaration>();
                    int stackSize = lastDecl.StackOffset + lastDecl.Type.Size;
					bool[] usedStack = new bool[stackSize];

                    var CurrentMappings = new List<KeyValuePair<Declaration, Declaration>>();

                    var statements = from Statement st in functions.Current.Code
                                     select st;
                    foreach (Statement statement in statements)
                    {
                        int nPos = block.Statements.IndexOf(statement);
                        block.Statements.Remove(statement);

                        List<Statement> replacementList = new List<Statement>();

                        var usedLValues = statement.GetModifiedDeclarations();
                        List<Declaration> usedDecls = statement.GetReferencedDeclarations();

                        if (usedLValues.Count == 1)
                        {
							if (usedLValues[0] == RegisterContents[0] || RegisterContents[0] == null)
							{
								RegisterContents[0] = usedLValues[0];
								statement.ReplaceDeclaration(usedLValues[0], module.FindDeclaration("__hl"));
							}
							else
							{
								int index = RegisterContents.IndexOf(null);
								if (index == -1)
								{
									var store = new StackStore(usedDecls[0], module.FindDeclaration("__hl"));
									replacementList.Add(store);
									usedStack[store.StackOffset] = true;
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
								for (int j = 0; j < RegisterContents.Count && !alreadyInRegister; j++) {
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

                        replacementList.Add(statement);

                        block.Statements.InsertRange(nPos, replacementList);
                    }

                    Declaration stackDecl = block.Declarations[0];
                    block.Declarations.Clear();
                    block.Declarations.Add(stackDecl);

                    // copy the params to the stack (on z80 these will already be on the stack)
                    foreach (Declaration param in (functions.Current.Type as FunctionType).Params)
                    {
						var store = new StackStore(param, param);
						usedStack[store.StackOffset] = true;
                        block.Statements.Insert(0, store);
                    }
                }
            }
        }
    }
}
