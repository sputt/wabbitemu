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
				StackAllocator stack = new StackAllocator(block);
				block.stack = stack;

				// Parameters
				var function = functions.Current.Type as FunctionType;
				for (int i = 0; i < function.Params.Count; i++)
				{
					stack.ReserveSpace(function.Params[i]);
				}

				// Return address
				stack.ReserveSpace(new BuiltInType("void *").Size);

				// Saved registers
				stack.ReserveSpace(new BuiltInType("void *").Size * 3);

				stack.ReserveSpace(0);


				var statements = from Statement s in block
								 where s is FunctionCall
								 select s;
				foreach (FunctionCall call in statements)
				{
					int index = block.Statements.IndexOf(call);
					block.Statements.Remove(call);

                    List<Statement> newStatements = null;
                    switch (block.Function.CallingConvention)
                    {
                        case FunctionType.CallConvention.CalleeSave:
                            newStatements = FormatCallCalleeSave(block, call);
                            break;
                        case FunctionType.CallConvention.CallerSave:
                            newStatements = FormatCallCallerSave(block, call);
                            break;
                    }
					

					block.Statements.InsertRange(index, newStatements);


				}

				Label endLabel = new Label("__function_cleanup");

				var returns = from Statement s in block
					where s is Return
					select s;
				foreach (Return curReturn in returns)
				{
					int index = block.Statements.IndexOf(curReturn);
					block.Statements.Remove(curReturn);

					var newStatements = new List<Statement>();

					var assignment = block.Statements[index - 1];
					Datum returnVal = curReturn.ReturnReg;
					if (assignment is Assignment)
					{
						block.Statements.Remove(assignment);
						index--;
						returnVal = ((Assignment)assignment).RValue;
					}
					newStatements.Add(new ReturnMove(returnVal));
					newStatements.Add(new Goto(endLabel));
					block.Statements.InsertRange(index, newStatements);
				}

				block.Statements.Add(endLabel);

				var funcType = functions.Current.Type as FunctionType;
				var newFuncType = new StrippedFunctionType(funcType);
				functions.Current.Type = newFuncType;
			}
		}

        private static List<Statement> FormatCallCallerSave(Block block, FunctionCall call)
        {
            var newStatements = new List<Statement>();
			////save regs (we can remove undeeded pushs later
			//newStatements.Add(new Push(block.FindDeclaration("__hl")));
			//newStatements.Add(new Push(block.FindDeclaration("__de")));
			//newStatements.Add(new Push(block.FindDeclaration("__bc")));
			//for (int i = 0; i < call.Params.Count; i++)
			//{
			//    newStatements.Add(new Push(call.Params[i]));
			//}

			//// Garbage push representing return address
			//newStatements.Add(new ReturnAddress(block.FindDeclaration("__hl")));

			//newStatements.Add(new Call(call.Function, call.LValue));
			///*if (call.LValue != null)
			//{
			//    newStatements.Add(new Move(call.LValue, block.FindDeclaration("__hl")));
			//}*/
			////restore regs
			//newStatements.Add(new Pop(block.FindDeclaration("__bc")));
			//newStatements.Add(new Pop(block.FindDeclaration("__de")));
			//newStatements.Add(new Pop(block.FindDeclaration("__hl")));
            return newStatements;
        }

        private static List<Statement> FormatCallCalleeSave(Block block, FunctionCall call)
        {
            var newStatements = new List<Statement>();
            for (int i = 0; i < call.Params.Count; i++)
            {
                newStatements.Add(new Push(call.Params[i]));
            }

            // Garbage push representing return address
            newStatements.Add(new ReturnAddress(block.FindDeclaration("__hl")));

			Call newCall = new Call(call.Function);
            newStatements.Add(newCall);
            if (call.LValue != null)
            {
				newCall.Block = block;
                newStatements.Add(new Move(call.LValue, newCall.ReturnRegister));
            }
            return newStatements;
        }
	}
}
