using System;
using System.Collections.Generic;
using System.Text;

namespace WabbitC
{
    public class Expression
    {
        List<Token> tokens;
		public List<Token> Tokens
		{
			get { return tokens; }
		}

		public Token Token
		{
			get
			{
				return tokens[0];
			}
		}

        public Expression(List<Token> tokens)
        {
            this.tokens = tokens;
        }

        public int Eval()
        {
			List<Token> rpn = ShuntingYard();
			Token result = CalcRPN(rpn);
            return 0;
        }

		private Token CalcRPN(List<Token> rpn)
		{
			Stack<Token> stack = new Stack<Token>();
			foreach (Token token in rpn)
			{
				switch (token.TokenType)
				{
					case TokenType.RealType:
					case TokenType.IntType:
						stack.Push(token);
						break;
					case TokenType.OperatorType:
						if (stack.Count < 2)
							throw new Exception("Not enough values on the stack");
						Token tok2 = stack.Pop();
						Token tok1 = stack.Pop();
						Token result = ApplyOperator(tok1, tok2, token);
						stack.Push(result);
						break;
					default:

						break;
				}
			}
			return stack.Pop();
		}

		private WabbitC.Token ApplyOperator(WabbitC.Token tok1, WabbitC.Token tok2, WabbitC.Token op)
		{
			Token result = new Token();
			dynamic var1, var2, resultvar = null;
			if (tok1.TokenType == TokenType.RealType || tok2.TokenType == TokenType.RealType)
			{
				result.TokenType = TokenType.RealType;
				var1 = double.Parse(tok1.TokenText);
				var2 = double.Parse(tok2.TokenText);
			}
			else
			{
				result.TokenType = TokenType.IntType;
				var1 = int.Parse(tok1.TokenText);
				var2 = int.Parse(tok2.TokenText);
			}
			switch (op.TokenText)
			{
				case "+":
					resultvar = var1 + var2;
					break;
				case "−":
				case "-":
					resultvar = var1 - var2;
					break;
				case "*":
					resultvar = var1 * var2;
					break;
				case "/":
					resultvar = var1 / var2;
					break;
				case "^":
					resultvar = var1 ^ var2;
					break;
			}
			result.TokenText = resultvar.ToString();
			return result;
		}

		int op_preced(Token token)
		{			
			switch (token.TokenText)
			{
				case "++":
				case "−−":
				case "--":
					return 12;
				case "!":
				case "~":
					return 11;
				case "*":
				case "/":
				case "%":
					return 10;
				case "+":
				case "-":
				case "−":
					return 9;
				case ">>":
				case "<<":
					return 8;
				case "<":
				case "<=":
				case ">":
				case ">=":
					return 7;
				case "==":
				case "!=":
					return 6;
				case "&":
					return 5;
				case "^":
					return 4;
				case "|":
					return 3;
				case "&&":
					return 2;
				case "||":
					return 1;
			}
			return 0;
		}

		bool op_left_assoc(Token token)
		{
			switch (token.TokenText)
			{
				// left to right
				case "++":
				case "--":
				case "−−":
				case "*":
				case "/":
				case "%":
				case "+":
				case "-":
				case "−":
				case ">>":
				case "<<":
				case "<":
				case "<=":
				case ">":
				case ">=":
				case "==":
				case "!=":
				case "&":
				case "^":
				case "|":
				case "&&":
				case "||":
					return true;
				// right to left
				case "!":
				case "~":
					return false;
			}
			return false;
		}

		uint op_arg_count(Token token)
		{
			char c = token.TokenText[0];
			switch (c)
			{
				case '*':
				case '/':
				case '%':
				case '+':
				case '-':
				case '=':
					return 2;
				case '!':
					return 1;
				default:
					return (uint)(c - 'A');
			}
		}


		private List<Token> ShuntingYard()
		{
			List<Token> output = new List<Token>();
			Stack<Token> stack = new Stack<Token>();

			foreach (Token token in tokens)
			{
				switch (token.TokenType)
				{
					case TokenType.RealType:
					case TokenType.IntType:
						output.Add(token);
						break;
					case TokenType.OperatorType:
						if (stack.Count > 0)
						{
							Token o2 = stack.Peek();
							while (o2.TokenType == TokenType.OperatorType &&
										((op_left_assoc(token) && (op_preced(token) <= op_preced(o2))) ||
										(!op_left_assoc(token) && (op_preced(token) < op_preced(o2)))))
							{
								o2 = stack.Pop();
								output.Add(o2);
								if (stack.Count > 0)
									o2 = stack.Peek();
								else
									break;
							}
						}
						stack.Push(token);
						break;
					default:
						if (token.TokenText == "(")
						{
							stack.Push(token);
							break;
						}
						else if (token.TokenText == ")")
						{
							Token nextTok = stack.Pop();
							while (nextTok.TokenText != "(")
							{
								output.Add(nextTok);
								if (stack.Count > 0)
									nextTok = stack.Pop();
								else
									throw new Exception("Mismatched parenthesises");
							}
							//stack.Pop();

						}
						break;
				}
			}
			while (stack.Count > 0)
			{
				Token token = stack.Pop();
				output.Add(token);
			}
			
			return output;
		}


		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();
			foreach (Token token in tokens)
				sb.Append(token.TokenText);
			return sb.ToString();
		}
    }
}
