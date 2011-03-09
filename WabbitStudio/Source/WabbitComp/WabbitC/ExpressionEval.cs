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

		List<Expression> args;
		public List<Expression> Args
		{
			get { return args; }
		}

        public Expression(List<Token> tokens)
        {
            this.tokens = tokens;
        }

		public Expression(Token token)
		{
			this.tokens = new List<Token> { token };
		}

        public List<Expression> Eval()
        {
			ReplaceDefines();
			var test = FillStack(this);
			CalculateStack(ref test);

			return test;
        }

		private void CalculateStack(ref List<Expression> expressions)
		{
			for (int i = expressions.Count - 1; i > 0; i--)
			{
				var exp = expressions[i];
				CalculateStack(ref exp);
				expressions[i] = exp;
			}
			var listTokens = Expression.ToTokens(expressions);
			var result = CalculateStack(ref listTokens);
			if (result != null)
			{
				expressions.Clear();
				expressions.Add(result);
			}
		}

		private void CalculateStack(ref Expression exp)
		{
			if (exp.Args != null)
				return;
			var evalExp = exp.Eval();
			var tokens = Expression.ToTokens(evalExp);
			var result = CalculateStack(ref tokens);
			exp = result == null ? exp : result;
		}

		private Expression CalculateStack(ref List<Token> tokens)
		{
			if (tokens.Count < 2)
				return null;
			if (tokens.Count == 2)
			{
				Token op = tokens[0];
				Token value = tokens[1];
				var result = ApplyOperator(value, op);
				return result;
			}
			else if (tokens.Count == 3 )
			{
				Token op = tokens[0];
				if (op.Type == TokenType.OperatorType)
				{
					Token value1 = tokens[1];
					Token value2 = tokens[2];
					Expression result = ApplyOperator(value1, value2, op);
					return result;
				}
			}
			return null;
		}

		private Expression ApplyOperator(WabbitC.Token tok1, WabbitC.Token op)
		{
			Expression result = null;
			switch (op.Text)
			{
				case "!":
					result = !tok1;
					break;
				case "~":
					result = ~tok1;
					break;
			}
			return result;
		}

		private Expression ApplyOperator(WabbitC.Token tok1, WabbitC.Token tok2, WabbitC.Token op)
		{
			int int1, int2;
			Token token;
			Expression result = null;
			switch (op.Text)
			{
				case "+":
					result = tok1 + tok2;
					break;
				case "−":
				case "-":
					result = tok1 - tok2;
					break;
				case "*":
					result = tok1 * tok2;
					break;
				case "/":
					result = tok1 / tok2;
					break;
				case "^":
					result = tok1 ^ tok2;
					break;
                case "=":
                    result = (Token.OpEquals(tok1, tok2));
                    break;
				case "&":
					result = tok1 & tok2;
					break;
				case "|":
					result = tok1 | tok2;
					break;
				case "||":
					if (!int.TryParse(tok1.Text, out int1) || !int.TryParse(tok2.Text, out int2))
					{
						var resultList = new List<Token> { Token.OROperatorToken, tok2, tok1 };
						return new Expression(resultList);
					}
					token = new Token() { Text = (Convert.ToBoolean(int1) || Convert.ToBoolean(int2)).ToString(), Type = TokenType.IntType };
					return new Expression(token);
				case "&&":
					if (!int.TryParse(tok1.Text, out int1) || !int.TryParse(tok2.Text, out int2))
					{
						var resultList = new List<Token> { Token.OROperatorToken, tok2, tok1 };
						return new Expression(resultList);
					}
					token = new Token() { Text = (Convert.ToBoolean(int1) && Convert.ToBoolean(int2)).ToString(), Type = TokenType.IntType };
					return new Expression(token);
			}
			return result;
		}

		public List<Expression> FillStack(Expression input)
		{
			List<Expression> stack = new List<Expression>();
			stack.Add(input);
			for (int i = 0; i < stack.Count; i++)
			{
				Expression curExpr = stack[i];
				int operatorIndex = Expression.GetOperator(curExpr);
				if (operatorIndex != -1)
				{
					int j;
					Expression leftSide, rightSide, op;
					List<Token> tokenList = new List<Token>();
					for (j = 0; j < operatorIndex; j++)
						tokenList.Add(curExpr.Tokens[j]);
					leftSide = new Expression(tokenList);
					tokenList = new List<Token>();
					op = new Expression(new List<Token> { curExpr.Tokens[j++] });
					for (; j < curExpr.Tokens.Count; j++)
						tokenList.Add(curExpr.Tokens[j]);
					rightSide = new Expression(tokenList);
					stack.Remove(curExpr);
					stack.Insert(i, op);
					stack.Insert(i + 1, leftSide);
					stack.Insert(i + 2, rightSide);
				}
				else
				{
					if (curExpr.Tokens.Count < 1)
						continue;
					Token curToken = curExpr.Tokens[0];
					if (curToken.Type == TokenType.OpenParen)
					{
						//handle paren
						int nParen = 0;
						int j = 1;
						curToken = curExpr.Tokens[j];
						List<Token> insideTokens = new List<Token>();
						while (!(curToken.Type == TokenType.CloseParen && nParen == 0))
						{
							if (curToken.Type == TokenType.CloseParen)
								nParen--;
							else if (curToken.Type == TokenType.OpenParen)
								nParen++;
							insideTokens.Add(curToken);
							curToken = curExpr.Tokens[++j];
						}
						Expression insideExp = new Expression(insideTokens);
						stack[i] = insideExp;
					}
					else if (curToken.Type == TokenType.StringType && curExpr.Tokens.Count > 1 &&
								curExpr.Tokens[1].Type == TokenType.OpenParen)
					{
						//its a func!
						List<Expression> args = new List<Expression>();
						curToken = curExpr.Tokens[2];
						while (curToken.Type != TokenType.CloseParen)
						{
							int nParen = 0;
							List<Token> arg = new List<Token>();
							while (!((curToken.Type == TokenType.ArgSeparator || curToken.Type == TokenType.CloseParen) && nParen == 0))
							{
								if (curToken.Type == TokenType.CloseParen)
									nParen++;
								else if (curToken.Type == TokenType.OpenParen)
									nParen--;
								arg.Add(curToken);
								curExpr.Tokens.Remove(curToken);
								curToken = curExpr.Tokens[2];
							}
							Expression argExp = new Expression(arg);
							args.Add(argExp);
							if (3 < curExpr.Tokens.Count)
							{
								curExpr.Tokens.Remove(curToken);
								curToken = curExpr.Tokens[2];
							}
						}
						curExpr.args = args;
					}
				}
			}
			return stack;
		}

		static List<List<string>> operators = new List<List<string>> { new List<string> {"||"},
																	new List<string> {"&&"}, 
																	new List<string> {"|"},
																	new List<string> {"^"},
																	new List<string> {"&"},
																	new List<string> {"==", "!="},
																	new List<string> {"<", "<=", ">", ">="},
																	new List<string> {">>", "<<"},
																	new List<string> {"+", "-", "−"},
																	new List<string> {"*", "/", "%"},
																	new List<string> {"!", "~"},
																	new List<string> {"++",  "−−", "--"},
                                                                    new List<string> {"="},
																};
		public static int GetOperator(Expression expr)
		{
			foreach (List<string> operatorLevel in operators)
			{
				List<Token> tokens = expr.Tokens;
				int nParen = 0;
				for (int i = tokens.Count - 1; i >= 0; i--)
				{
					Token token = tokens[i];
					if (token.Type == TokenType.CloseParen)
						nParen++;
					else if (token.Type == TokenType.OpenParen)
						nParen--;
					else if (nParen == 0 && token.Type == TokenType.OperatorType && operatorLevel.Contains(token.Text)) 
					{
						if ((token.Text != "-" && token.Text != "+" && token.Text != "*") 
								|| (i > 0 && tokens[i - 1].Type != TokenType.OperatorType))
							return i;
					}
				}
			}
			return -1;
		}

		#region Defines
		private void ReplaceDefines()
		{
			for(int i = 0; i < tokens.Count; i++)
			{
				Token token = tokens[i];
				CheckTokenReplace(ref token);
				tokens[i] = token;
			}
		}

		private void CheckTokenReplace(ref WabbitC.Token token)
		{
			if (token.Type == TokenType.StringType)
			{
				PreprocessorDefine define = PreprocessorParser.DefineValue(token);
				if (define != null)
				{
					Type defineType = define.GetType();
					if (defineType == typeof(ReplacementDefine))
					{
						ReplacementDefine replaceDefine = (ReplacementDefine)define;
						token = replaceDefine.Value;
						CheckTokenReplace(ref token);
					}
				}
			}
		}
		#endregion

		#region Old Method
		/*
		private Token CalcRPN(List<Token> rpn)
		{
			Stack<Token> stack = new Stack<Token>();
			foreach (Token token in rpn)
			{
				switch (token.Type)
				{
					case TokenType.RealType:
					case TokenType.IntType:
						stack.Push(token);
						break;
					case TokenType.OperatorType:
						int argsRequired = GetArgCount(token);
						if (stack.Count < argsRequired)
							throw new Exception("Not enough values on the stack");
						Token result, tok1 = null, tok2 = stack.Pop();
						if (argsRequired > 1)
						{
							tok1 = stack.Pop();
							result = ApplyOperator(tok1, tok2, token);
						}
						else
						{
							result = ApplyOperator(tok2, token);
						}
						
						stack.Push(result);
						break;
					default:
						//functions/vars here
						stack.Push(token);
						break;
				}
			}
			return stack.Pop();
		}

		private int GetArgCount(WabbitC.Token token)
		{
			switch (token.Text)
			{
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
					return 2;
				case "++":
				case "--":
				case "−−":
				case "!":
				case "~":
					return 1;
				default:
					return 2;
			}
		}

		private WabbitC.Token ApplyOperator(WabbitC.Token tok1, WabbitC.Token op)
		{
			Token result = new Token();
			dynamic var1, resultvar = null;
			if (tok1.Type == TokenType.RealType)
			{
				result.Type = TokenType.RealType;
				var1 = double.Parse(tok1.Text);
			}
			else
			{
				result.Type = TokenType.IntType;
				if (tok1.Type == TokenType.IntType)
					var1 = int.Parse(tok1.Text);
				else
					var1 = tok1.Text;
			}
			switch (op.Text)
			{
				case "!":
					resultvar = !tok1;
					break;
				case "~":
					resultvar = ~tok1;
					break;
			}
			result.Text = resultvar.ToString();
			return result;
		}

		private WabbitC.Token ApplyOperator(WabbitC.Token tok1, WabbitC.Token tok2, WabbitC.Token op)
		{
			Token result = new Token();
			dynamic var1, var2;
			if (tok1.Type == TokenType.RealType || tok2.Type == TokenType.RealType)
			{
				result.Type = TokenType.RealType;
				var1 = double.Parse(tok1.Text);
				var2 = double.Parse(tok2.Text);
			}
			else 
			{
				result.Type = TokenType.IntType;
				if (tok1.Type == TokenType.IntType)
					var1 = int.Parse(tok1.Text);
				else
					var1 = tok1.Text;
				if (tok2.Type == TokenType.IntType)
					var2 = int.Parse(tok2.Text);
				else
					var2 = tok1.Text;
			}
			switch (op.Text)
			{
				case "+":
					result = tok1 + tok2;
					break;
				case "−":
				case "-":
					result = tok1 - tok2;
					break;
				case "*":
					result = tok1 * tok2;
					break;
				case "/":
					result = tok1 / tok2;
					break;
				/*case "^":
					resultvar = var1 ^ var2;
					break;
				case "||":
					resultvar = Convert.ToBoolean(var1) || Convert.ToBoolean(var2);
					break;
				case "&&":
					resultvar = Convert.ToBoolean(var1) && Convert.ToBoolean(var2);
					break;*
			}
			return result;
		}

		int op_preced(Token token)
		{			
			switch (token.Text)
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
			switch (token.Text)
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
			char c = token.Text[0];
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


		private List<Token> ShuntingYard(List<Token> input)
		{
			List<Token> output = new List<Token>();
			Stack<Token> stack = new Stack<Token>();

			for(int i = 0; i < input.Count; i++)
			{
				Token token = input[i];
				switch (token.Type)
				{
					case TokenType.RealType:
					case TokenType.IntType:
						output.Add(token);
						break;
					case TokenType.OperatorType:
						if (stack.Count > 0)
						{
							Token o2 = stack.Peek();
							while (o2.Type == TokenType.OperatorType &&
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
					case TokenType.ArgSeparator:
						bool foundParen = false;
						List<Token> arg = new List<Token>();
						while (stack.Count > 0)
						{
							Token o1 = stack.Peek();
							if (o1.Type == TokenType.OpenParen)
							{
								foundParen = true;
								break;
							}
							else
							{
								arg.Add(o1);
								stack.Pop();
							}
						}
						if (!foundParen)
								throw new Exception("Mismatched parenthesises or incorrect separator");
						Expression expr = new Expression(arg);
						expr.Eval();
						break;
					case TokenType.OpenParen:
						stack.Push(token);
						break;
					case TokenType.CloseParen:
						Token nextTok = stack.Pop();
						while (nextTok.Type != TokenType.OpenParen)
						{
							output.Add(nextTok);
							if (stack.Count > 0)
								nextTok = stack.Pop();
							else
								throw new Exception("Mismatched parenthesises");
						}
						break;
					default:
						if (i + 1 >= input.Count || input[i + 1].Type != TokenType.OpenParen)
							output.Add(token);
						else
						{
							//its a function
							stack.Push(token);
							token.Type = TokenType.FunctionType;
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
		}*/
		#endregion

		public static List<Token> ToTokens(List<Expression> exps)
		{
			var evalTokens = new List<Token>();
			foreach(Expression exp in exps)
				evalTokens.AddRange(exp.Tokens);
			return evalTokens;
		}

		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();
			for(int i = 0; i < tokens.Count - 1; i++)
			{
				sb.Append(tokens[i].Text);
			}
			if (args != null)
			{
				foreach (Expression arg in args)
				{
					sb.Append(arg.ToString());
					sb.Append(",");
				}
				sb.Remove(sb.Length - 1, 1);
			}
			sb.Append(tokens[tokens.Count - 1]);
			return sb.ToString();
		}
    }

	public class ExpressionResult
	{
		public ExpressionResult()
		{

		}
	}
}
