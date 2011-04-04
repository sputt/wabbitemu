using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
	class RemovePointlessGotos
	{
		public static void Run(Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				int nStatements = block.Statements.Count - 1;
				
				for (int i = 0; i < nStatements; i++)
				{
					Goto gotoStatement = block.Statements[i] as Goto;
					if (gotoStatement != null)
					{
						Label labelStatement = block.Statements[i + 1] as Label;
						if (labelStatement != null)
						{
							if (gotoStatement.TargetLabel == labelStatement)
							{
								block.Statements.Remove(gotoStatement);
								nStatements--;
								i--;
							}
						}
					}
				}
			}
		}
	}
}
