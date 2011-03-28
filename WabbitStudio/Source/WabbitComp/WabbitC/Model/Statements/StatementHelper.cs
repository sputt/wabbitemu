using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Statements.Math;
using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    static class StatementHelper
    {
        private const string StoreToken = "#store";

        private static void IdentifyStores(ref List<Expression> exprList)
        {
            for (int i = 0; i < exprList.Count - 1; i++)
            {
                if (exprList[i].Tokens.Count == 1)
                {
                    if (exprList[i].Tokens[0] == "=" &&
                        exprList[i + 1].Tokens[0] == "*")
                    {
                        exprList.RemoveAt(i + 1);
                        exprList[i].Tokens[0] = Tokenizer.ToToken("#store");
                        exprList[i].Tokens[0].Type = TokenType.OperatorType;
                    }
                }
            }
        }

        private static Token InternalParse(Block block, Expression expr)
        {
            var stack = new Stack<Token>();

            var exprList = expr.Eval();
            IdentifyStores(ref exprList);
            for (int i = exprList.Count - 1; i >= 0; i--)
            {
                Token token = null;
                if (exprList[i].IsCast)
                {
                    List<Token>.Enumerator tokens = exprList[i].Tokens.GetEnumerator();
                    tokens.MoveNext();
                    Type castType = TypeHelper.ParseType(ref tokens);
                    if (castType != null)
                    {
                        Declaration temp = block.CreateTempDeclaration(castType);
                        var castStatement = new Cast(temp, castType, Datum.Parse(block, stack.Pop()));
                        block.Statements.Add(castStatement);
                        stack.Push(Tokenizer.ToToken(temp.Name));
                    }
                    else
                    {
                        // Some kind of false positive
                    }
                }
                else if ((exprList[i].Tokens.Count > 1) && (exprList[i].Args == null))
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
                    List<Declaration> paramList = FunctionCall.BuildParams(block, 
                        (FunctionType)funcDecl.Type, exprList[i].Args);

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
                else if (token != null && token.Type == TokenType.OperatorType)
                {
                    Declaration decl = null;
					switch (exprList[i].Operands)
					{
						case 1:
							decl = ParseOperand(exprList[i], block, stack.Pop());
							break;
						case 2:
							decl = ParseOperand(exprList[i], block, stack.Pop(), stack.Pop());
							break;
						case 3:
							decl = ParseOperand(exprList[i], block, stack.Pop(), stack.Pop(), stack.Pop());
							break;
					}

                    if (decl != null)
                    {
                        stack.Push(Tokenizer.ToToken(decl.Name));
                    }
                }
                else if (token != null)
                {
                    stack.Push(token);
                }

            }

            return stack.Pop();
        }

		static Declaration ParseOperand(Expression exp, Block block, Token arg1)
		{
			Declaration decl = null;
			switch (exp.Tokens[0])
			{
                //TODO: fix for post/pre
                case "++":
                    if (exp.IsPrefix)
                    {
                        decl = block.FindDeclaration(arg1.Text);
                        block.Statements.Add(new Add(decl, Datum.Parse(block, Tokenizer.ToToken("1"))));
                    }
                    else
                    {

                    }
                    break;
                case "--":
                    if (exp.IsPrefix)
                    {
                        decl = block.FindDeclaration(arg1.Text);
                        block.Statements.Add(new Sub(decl, Datum.Parse(block, Tokenizer.ToToken("1"))));
                    }
                    else
                    {

                    }
                    break;
                case "*":
                    {
                        Type type = TypeHelper.GetType(block, arg1);
                        type.Dereference();
                        decl = block.CreateTempDeclaration(type);
                        block.Statements.Add(new Load(decl, Datum.Parse(block, arg1)));
                        break;
                    }

                case "&":
                    {
                        Type type = TypeHelper.GetType(block, arg1);
                        type.Reference();
                        decl = block.CreateTempDeclaration(type);
                        block.Statements.Add(new AddressOf(decl, block.FindDeclaration(arg1)));
                        break;
                    }
			}
			return decl;
		}

		static Declaration ParseOperand(Expression exp, Block block, Token arg1, Token arg2)
		{
			Declaration decl = null;
			switch (exp.Tokens[0])
			{
				case "+":
                case "-":
                case "*":
                case "/":
					decl = MathStatement.BuildStatements(block, exp, arg1, arg2);
					break;
                case StoreToken:
                    decl = block.CreateTempDeclaration(TypeHelper.GetType(block, arg2));
                    var assignCopy = AssignmentHelper.ParseSingle(block, decl, arg2);
                    block.Statements.Add(assignCopy);

                    var storeStatement = new Store(Datum.Parse(block, arg1), 
                        Datum.Parse(block, Tokenizer.ToToken(decl.Name)));
                    block.Statements.Add(storeStatement);
                    break;
				case "=":
					decl = block.FindDeclaration(arg1);
                    ValueStatement initialAssign = AssignmentHelper.ParseSingle(block, decl, arg2);
                    block.Statements.Add(initialAssign);
					break;
                case "==":
                case "<=":
                case "<":
                case ">":
                case ">=":
                case "!=":
                    Token condLHS;
                    Token condRHS;
                    Declaration tmpDecl = block.FindDeclaration(arg1);
                    if (tmpDecl == null)
                    {
                        condRHS = arg1;
                        condLHS = arg2;
                    }
                    else
                    {
                        condLHS = arg1;
                        condRHS = arg2;
                    }
                    decl = ConditionStatement.BuildStatements(block, exp, condLHS, condRHS);
                    break;
			}
			return decl;
		}

		static Declaration ParseOperand(Expression exp, Block block, Token arg1, Token arg2, Token arg3)
		{
			Declaration decl = null;
			decl = block.FindDeclaration(arg1);
			return decl;
		}

        public static void Parse(Block block, List<Token> tokenList)
        {
            Token token = InternalParse(block, new Expression(tokenList));
        }

        public static void Parse(Block block, Declaration LValue, List<Token> tokenList)
        {
            Token token = InternalParse(block, new Expression(tokenList));

            var finalAssign = AssignmentHelper.ParseSingle(block, LValue, token);
            block.Statements.Add(finalAssign);
        }
    }
}
