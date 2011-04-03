using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
	class ApplyCallingConvention
	{
		public static void Run(Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;

				var statements = from Statement s in block
								 where s.GetType() == typeof(FunctionCall)
								 select s;
				foreach (FunctionCall call in statements)
				{
					int index = block.Statements.IndexOf(call);
					block.Statements.Remove(call);

					var newStatements = new List<Statement>();

					for (int i = 0; i < call.Params.Count; i++)
					{
						newStatements.Add(new Push(call.Params[i]));
					}

					// Garbage push representing return address
					newStatements.Add(new Push(block.FindDeclaration("__hl")));

					newStatements.Add(new Call(call.Function));
					if (call.LValue != null)
					{
						newStatements.Add(new Move(call.LValue, block.FindDeclaration("__hl")));
					}

					block.Statements.InsertRange(index, newStatements);


				}

				//// Add pushing regs
				//block.Statements.Insert(0, new Push(block.FindDeclaration("__hl")));
				//block.Statements.Insert(1, new Push(block.FindDeclaration("__de")));
				//block.Statements.Insert(2, new Push(block.FindDeclaration("__bc")));

				// Add popping regs
				Label endLabel = new Label("__function_cleanup");

				var returns = from Statement s in block
					where s.GetType() == typeof(Return)
					select s;
				foreach (Return curReturn in returns)
				{
					int index = block.Statements.IndexOf(curReturn);
					block.Statements.Remove(curReturn);

					var newStatements = new List<Statement>();
					newStatements.Add(AssignmentHelper.ParseSingle(block, 
						block.FindDeclaration("__hl"), new Token(curReturn.ReturnReg.ToString())));
					newStatements.Add(new Goto(endLabel));
					block.Statements.InsertRange(index, newStatements);
				}

				block.Statements.Add(endLabel);

				var funcType = functions.Current.Type as FunctionType;
				var newFuncType = new StrippedFunctionType(funcType);
				functions.Current.Type = newFuncType;
			}
		}
	}
}
