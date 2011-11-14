using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
    class LabelMerger
    {
		private static void ReplaceLabelReferences(Block block, Label labelToReplace, Label newLabel)
		{
			var statements = from Statement s in block
							 where s is Goto
							 select s;
			foreach (Goto jump in statements)
			{
				if (jump.TargetLabel == labelToReplace)
				{
					jump.TargetLabel = newLabel;
				}
			}
		}

        public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
				Block block = functions.Current.Code;
                var statements = from Statement s in block
                                 select s;

                Label prevLab = null;
                foreach (Statement statement in statements)
                {
                    Label curLab = null;
                    if (statement is Label)
                    {
						curLab = statement as Label;
                    }
					if (curLab != null && prevLab != null)
					{
						functions.Current.Code.Statements.Remove(curLab);
						ReplaceLabelReferences(block, curLab, prevLab);
					}
					else
					{
						prevLab = curLab;
					}
                }
            }
        }
    }
}
