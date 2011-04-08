using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model;
using WabbitC.Model.Statements;

namespace WabbitC.StatementPasses
{
    class LoopGotoConversion
    {
        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    Block block = functions.Current.Code;

                    var whileLoops = from Statement st in block
                                     where st.GetType() == typeof(While)
                                     select st;
					var doWhileLoops = from Statement st in block
									 where st.GetType() == typeof(DoWhile)
									 select st;
                    foreach (While loop in whileLoops)
                    {
                        Block loopBlock = loop.Block;
                        int nPos = loopBlock.Statements.IndexOf(loop);
                        loopBlock.Statements.Remove(loop);

                        List<Statement> whileReplacement = new List<Statement>();
                        whileReplacement.Add(new Goto(loop.Label));
                        Label whileStartLbl = block.CreateTempLabel();
						List<Statement> condStatements = new List<Statement>();
                        whileReplacement.Add(whileStartLbl);
                        whileReplacement.AddRange(loop.Body);
                        whileReplacement.Add(loop.Label);
                        whileReplacement.AddRange(Goto.ParseConditionStatements(loop.Condition.Statements, loop.Label));

                        loopBlock.Statements.InsertRange(nPos, whileReplacement);
                    }
					foreach (DoWhile loop in doWhileLoops)
					{
						Block loopBlock = loop.Block;
						int nPos = loopBlock.Statements.IndexOf(loop);
						loopBlock.Statements.Remove(loop);

						List<Statement> whileReplacement = new List<Statement>();
						Label whileStartLbl = block.CreateTempLabel();
						whileReplacement.Add(whileStartLbl);
						whileReplacement.AddRange(loop.Body);
						whileReplacement.Add(loop.Label);
						whileReplacement.AddRange(Goto.ParseConditionStatements(loop.Condition.Statements, loop.Label));

						loopBlock.Statements.InsertRange(nPos, whileReplacement);
					}
                }
            }
        }
    }
}
