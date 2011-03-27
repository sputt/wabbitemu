using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;

namespace WabbitC.StatementPasses
{
    class IfGotoConversion
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    var ifs = from Statement st in functions.Current.Code
                                     where st.GetType() == typeof(If)
                                     select st;
                    foreach (If ifStatement in ifs)
                    {
                        Block block = ifStatement.Block;
                        int nPos = block.Statements.IndexOf(ifStatement);
                        block.Statements.Remove(ifStatement);

                        List<Statement> ifReplacement = new List<Statement>();
                        Label trueSkipLabel = block.CreateTempLabel();

                        ifReplacement.Add(new Goto(trueSkipLabel, ifStatement.Condition));
                        ifReplacement.AddRange(ifStatement.TrueCase.Statements);
                        if (ifStatement.FalseCase == null)
                        {
                            ifReplacement.Add(trueSkipLabel);
                        }
                        else
                        {
                            Label falseSkipLabel = block.CreateTempLabel();
                            ifReplacement.Add(new Goto(falseSkipLabel));
                            ifReplacement.Add(trueSkipLabel);
                            ifReplacement.AddRange(ifStatement.FalseCase.Statements);
                            ifReplacement.Add(falseSkipLabel);
                        }

                        block.Statements.InsertRange(nPos, ifReplacement);
                    }
                }
            }
        }
    }
}
