using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
    static class DumbRegisterAllocator
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    Block block = functions.Current.Code;
                    List<Declaration> RegistersAvailable = new List<Declaration>()
                    {
                        module.FindDeclaration("__de"),
                        module.FindDeclaration("__bc")
                    };

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
                            statement.ReplaceDeclaration(usedLValues[0], module.FindDeclaration("__hl"));
                        }

                        for (int i = 0; i < usedDecls.Count; i++)
                        {
                            bool fSkip = false;
                            if (usedLValues.Count == 1)
                            {
                                if (usedLValues[0] == usedDecls[i])
                                {
                                    replacementList.Add(new StackLoad(module.FindDeclaration("__hl"), usedDecls[i]));
                                    fSkip = true;
                                }
                            }
                            if (fSkip == false)
                            {
                                replacementList.Add(new StackLoad(RegistersAvailable[i], usedDecls[i]));
                                statement.ReplaceDeclaration(usedDecls[i], RegistersAvailable[i]);
                            }
                        }

                        replacementList.Add(statement);

                        if (usedLValues.Count == 1)
                        {
                            replacementList.Add(new StackStore(usedLValues[0], module.FindDeclaration("__hl")));
                        }

                        block.Statements.InsertRange(nPos, replacementList);
                    }

                    Declaration stackDecl = block.Declarations[0];
                    block.Declarations.Clear();
                    block.Declarations.Add(stackDecl);

                    foreach (Declaration param in (functions.Current.Type as FunctionType).Params)
                    {
                        block.Statements.Insert(0, new StackStore(param, param));
                    }
                }
            }
        }
    }
}
