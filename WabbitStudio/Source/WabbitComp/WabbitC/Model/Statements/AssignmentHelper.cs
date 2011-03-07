using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    static class AssignmentHelper
    {
        public static ValueStatement ParseSingle(Block block, Declaration LValue, Token RValue)
        {
            if (Immediate.IsImmediate(RValue) == false)
            {
                Declaration valDecl = block.FindDeclaration(RValue.Text);
                return new Move(LValue, valDecl);
            }
            else
            {
                Immediate valImm = new Immediate(RValue);
                return new Assignment(LValue, valImm);
            }
        }

        public static void Parse(Block block, Declaration LValue, List<Token> tokenList)
        {
            Expression expr = new Expression(tokenList);

            var rpnStack = new Stack<Token>();
            var rpnList = expr.Eval();
            for (int i = 0; i < rpnList.Count; i++)
            {
                if (rpnList[i].Type == TokenType.OperatorType)
                {
                    //TODO: Figure out what type this should be
                    Declaration decl = block.CreateTempDeclaration(new BuiltInType("int"));

                    var arg1 = rpnStack.Pop();
                    var arg2 = rpnStack.Pop();

                    ValueStatement initialAssign = ParseSingle(block, decl, arg1);
                    block.Statements.Add(initialAssign);

                    if (Immediate.IsImmediate(arg2))
                    {
                        var operation = new AddImmediate(decl, new Immediate(arg2));
                        block.Statements.Add(operation);
                    }
                    else
                    {
                        block.Statements.Add(new Add(decl, block.FindDeclaration(arg2.Text)));
                    }

                    rpnStack.Push(Tokenizer.ToToken(decl.Name));
                }
                else
                {
                    rpnStack.Push(rpnList[i]);
                }
            }

            var finalAssign = ParseSingle(block, LValue, rpnStack.Pop());
            block.Statements.Add(finalAssign);
        }
    }
}
