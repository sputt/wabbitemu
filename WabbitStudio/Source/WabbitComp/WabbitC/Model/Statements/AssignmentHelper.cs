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

        private static Token InternalParse(Block block, Expression expr)
        {
            var stack = new Stack<Token>();

            var exprList = expr.Eval();
            for (int i = exprList.Count - 1; i >= 0; i--)
            {
                Token token;
                if (exprList[i].Tokens.Count > 1)
                {
                    token = InternalParse(block, exprList[i]);
                }
                else
                {
                    token = exprList[i].Tokens[0];
                }

                if (exprList[i].Args != null)
                {
                    Declaration funcDecl = block.FindDeclaration(token);
                    List<Declaration> paramList = FunctionCall.BuildParams(block, (FunctionType)funcDecl.Type, exprList[i].Args);

                    Declaration returnDecl;
                    Type returnType = (funcDecl.Type as FunctionType).ReturnType;
                    if (returnType.Equals(new BuiltInType("void")))
                    {
                        throw new Exception("Invalid type");
                    }
                    else
                    {
                        returnDecl = block.CreateTempDeclaration(returnType);
                    }
                    var funcCall = new FunctionCall(returnDecl, funcDecl, paramList);
                    block.Statements.Add(funcCall);
                    stack.Push(Tokenizer.ToToken(returnDecl.Name));
                }
                else if (token.Type == TokenType.OperatorType)
                {
                    Declaration decl = null;
                    switch (token)
                    {
                        case "+":
                            {
                                var arg1 = stack.Pop();
                                decl = block.CreateTempDeclaration(TypeHelper.GetType(block, arg1));
                                ValueStatement initialAssign = ParseSingle(block, decl, arg1);
                                block.Statements.Add(initialAssign);
                                block.Statements.Add(new Add(decl, Datum.Parse(block, stack.Pop())));
                                break;
                            }
                        case "=":
                            {
                                decl = block.FindDeclaration(stack.Pop());
                                ValueStatement initialAssign = ParseSingle(block, decl, stack.Pop());
                                block.Statements.Add(initialAssign);
                            }
                            break;
                    }

                    if (decl != null)
                    {
                        stack.Push(Tokenizer.ToToken(decl.Name));
                    }
                }
                else
                {
                    stack.Push(token);
                }

            }

            return stack.Pop();
        }

        public static void Parse(Block block, Declaration LValue, List<Token> tokenList)
        {
            Token token = InternalParse(block, new Expression(tokenList));

            var finalAssign = ParseSingle(block, LValue, token);
            block.Statements.Add(finalAssign);
        }
    }
}
