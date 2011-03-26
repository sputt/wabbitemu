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
                if (tokens.Count == 0)
                    return null;
                else
                    return tokens[0];
            }
        }

		List<Expression> args;
		public List<Expression> Args
		{
			get { return args; }
		}

		bool isCast;
		public bool IsCast
		{
			get { return isCast; }
		}

		int operands;
		public int Operands
		{
			get { return operands; }
		}

        bool isPrefix = false;
        public bool IsPrefix { get { return isPrefix; } }

        public Expression(List<Token> tokens)
        {
            this.tokens = tokens;
        }

		public Expression(List<Token> tokens, int operands)
		{
			this.operands = operands;
			this.tokens = tokens;
		}

		public Expression(Token token)
		{
			this.tokens = new List<Token> { token };
		}

        public List<Expression> Eval()
        {
			ReplaceDefines();
			OptimizeTokens();
			var test = FillStack(this);
			CalculateStack(ref test);

			return test;
        }

		private List<Token> ReadBetweenParens(int i, int nParen, ref int j)
		{
			Token token = tokens[i];
			List<Token> tempList = new List<WabbitC.Token>();
			//skip function too
			int curParenLevel = nParen;
			tempList.Add(token);
			j = i + 1;
			while (token.Type != TokenType.CloseParen || curParenLevel == nParen)
			{
				if (token.Type == TokenType.CloseParen)
					curParenLevel--;
				else if (token.Type == TokenType.OpenParen)
					curParenLevel++;
				token = tokens[j++];
				tempList.Add(token);
			}
			return tempList;
		}

		private int SkipCasts(ref int i, ref int nParen, ref List<int> parenLoc, ref List<int> curOpLevel)
		{
			int j = -1;
			List<Token> tempList = ReadBetweenParens(i, nParen, ref j);
			if (CastHelper.IsCast(tempList))
			{
				i = j;
				if (tokens[i].Type == TokenType.OpenParen)
				{
					parenLoc.Add(i);
					curOpLevel.Add(-1);
					curOpLevel[nParen] = -1;		//impossible to remove
					nParen++;		//skip the first paren of the cast
					SkipCasts(ref i, ref nParen, ref parenLoc, ref curOpLevel);
				}
			}
			return j;
		}

		private void OptimizeTokens()
		{
			int nParen = 0;
			List<int> curOpLevel = new List<int>();
			List<int> parenLoc = new List<int>();
			bool canRemoveParen = true;
			curOpLevel.Add(-1);
			for (int i = 0; i < tokens.Count; i++)
			{
				Token token = tokens[i];
				if (token.Type == TokenType.StringType || token == "sizeof")
				{
					if (i + 1 < tokens.Count && tokens[i + 1].Type == TokenType.OpenParen)
					{
						int j = 0;
						ReadBetweenParens(i, nParen, ref j);
						i = j;
					}
				} 
				else if (token.Type == TokenType.OpenParen)
				{
					int j = SkipCasts(ref i, ref nParen, ref parenLoc, ref curOpLevel);
					if (i > 0 && tokens[i - 1].Type != TokenType.StringType && tokens[i - 1].Text != "sizeof")
					{
						nParen++;
						parenLoc.Add(i);
						curOpLevel.Add(-1);
					}
					else
					{
						i = j;
					}
				}
				else if (token.Type == TokenType.CloseParen)
				{
					nParen--;
					if (canRemoveParen && curOpLevel[nParen] == curOpLevel[nParen + 1])
					{
						tokens.RemoveAt(i);
						tokens.RemoveAt(parenLoc[parenLoc.Count - 1]);
					}
					curOpLevel[nParen + 1] = -1;
					curOpLevel[nParen] = -1;
					parenLoc.RemoveAt(parenLoc.Count - 1);
				}
				else if (token.Type == TokenType.OperatorType)
				{
					int level = 0;
					foreach (List<string> ops in operators)
					{
						if (ops.Contains(token))
							break;
						level++;
					}
					//ignore assignment operator
					if (curOpLevel[nParen] == -1)
						curOpLevel[nParen] = level;
					else if (curOpLevel[nParen] != level || IsBanned(token))
						canRemoveParen = false;
				}
			}
		}

        private bool IsBanned(Token token)
        {
            return token == "-" || token == "/";
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
			if (exp.Args != null || exp.Operands != 0 || exp.IsCast)
				return;
			var evalExp = exp.Eval();
			var tokens = Expression.ToTokens(evalExp);
			var result = CalculateStack(ref tokens);
			exp = result == null ? exp : result;
		}

		private Expression CalculateStack(ref List<Token> tokens)
		{
			if (tokens.Count < 1)
				return null;
            if (tokens.Count == 1)
                if (tokens[0].Type == TokenType.OperatorType)
                    return null;
                else
                    return new Expression(tokens);
			Stack<Token> stack = new Stack<Token>();
			int i = tokens.Count;
			Expression result = null;
			while (i > 0)
			{
				while (--i >= 0 && tokens[i].Type != TokenType.OperatorType)
					stack.Push(tokens[i]);
				if (i < 0)
					return null;
				Token op = tokens[i];
				List<Token> toks = new List<Token>();
				while (stack.Count > 0 && toks.Count < 2)
					toks.Add(stack.Pop());
				var exp = ApplyOperator(op, toks);
				if (exp != null) 
					result = exp;
				if (i > 0 && result != null)
					stack.Push(result.Tokens[0]);
				else
					return exp;
			}
			return result;
		}

		private Expression ApplyOperator(Token op, List<Token> tokens)
		{
			switch (tokens.Count)
			{
				case 1:
					return ApplyOperator(op, tokens[0]);
				case 2:
					return ApplyOperator(op, tokens[0], tokens[1]);
			}
			return null;
		}

		private Expression ApplyOperator(Token op, Token tok1)
		{
			Expression result = null;
			switch (op.Text)
			{
					//* and & pass through
				case "!":
					result = !tok1;
					break;
				case "~":
					result = ~tok1;
					break;
                case "-":
                    result = Tokenizer.ToToken("0") - tok1;
                    break;
			}
			return result;
		}

		private Expression ApplyOperator(Token op, Token tok1, Token tok2)
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
                    result = null;//(Token.OpEquals(tok1, tok2));
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
				int numArgs;
				int operatorIndex = Expression.GetOperator(curExpr, out numArgs);
				if (operatorIndex != -1)
				{
					int j;
					Expression leftSide, rightSide, ternarySide = null, op;
					List<Token> tokenList = new List<Token>();
					for (j = 0; j < operatorIndex; j++)
						tokenList.Add(curExpr.Tokens[j]);
					leftSide = new Expression(tokenList);

					tokenList = new List<Token>();
					op = new Expression(new List<Token> { curExpr.Tokens[j++] }, numArgs);
					if (numArgs == 3)
					{
						for (; j < curExpr.Tokens.Count && curExpr.Tokens[j].Text != ":"; j++)
							tokenList.Add(curExpr.Tokens[j]);
						rightSide = new Expression(tokenList);
						tokenList = new List<Token>();
						j++;		//skip :
						for (; j < curExpr.Tokens.Count; j++)
							tokenList.Add(curExpr.Tokens[j]);
						ternarySide = new Expression(tokenList);
					}
					else
					{
						for (; j < curExpr.Tokens.Count; j++)
							tokenList.Add(curExpr.Tokens[j]);
						rightSide = new Expression(tokenList);
					}
					stack.Remove(curExpr);
					stack.Insert(i, op);
					switch (numArgs)
					{
						case 3:
							stack.Insert(i + 1, leftSide);
							stack.Insert(i + 2, rightSide);
							stack.Insert(i + 3, ternarySide);
							break;
						case 2:
							stack.Insert(i + 1, leftSide);
							stack.Insert(i + 2, rightSide);
							break;
						case 1:
                            if (op.Token == "++" || op.Token == "--")
                                if (leftSide.Token != null)
                                    op.isPrefix = true;
                                else
                                    op.isPrefix = false;
							if (leftSide.Tokens.Count > 0)
								stack.Insert(i + 1, leftSide);
							else
								stack.Insert(i + 1, rightSide);
							break;
					}
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
						List<Token> castTokens = new List<Token>();
						castTokens.Add(Token.OpenParenToken);
						while (!(curToken.Type == TokenType.CloseParen && nParen == 0))
						{
							if (curToken.Type == TokenType.CloseParen)
								nParen--;
							else if (curToken.Type == TokenType.OpenParen)
								nParen++;
							insideTokens.Add(curToken);
							castTokens.Add(curToken);
							curToken = curExpr.Tokens[++j];
						}
						castTokens.Add(Token.CloseParenToken);
						Expression insideExp = new Expression(insideTokens);
						stack[i] = insideExp;
						
                        if (CastHelper.IsCast(castTokens))
						{
							j++;
							var castedTokens = new List<Token>();
							for (; j < curExpr.Tokens.Count; j++)
								castedTokens.Add(curExpr.Tokens[j]);
							if (castedTokens.Count > 0)
							{
								var castedExp = new Expression(castedTokens);
								insideExp.isCast = true;
								stack.Insert(i + 1, castedExp);
							}
						}
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
                            while ((curToken.Text != "," && curToken.Type != TokenType.CloseParen) && nParen == 0)
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

		private bool IsCasting(List<Token> insideTokens)
		{
			return false;
		}
		static List<List<string>> operators = new List<List<string>> { 
																	new List<string> {","},
																	new List<string> {"="},
																	new List<string> {"?"},
																	new List<string> {"||"},
																	new List<string> {"&&"}, 
																	new List<string> {"|"},
																	new List<string> {"^"},
																	new List<string> {"&"},
																	new List<string> {"==", "!="},
																	new List<string> {"<", "<=", ">", ">="},
																	new List<string> {">>", "<<"},
																	new List<string> {"+", "-", "−"},
																	new List<string> {"*", "/", "%"},
																	new List<string> {"!", "~", "&", "*", "+", "-", "++", "−−", "--"},
																	new List<string> {".", "++", "−−", "--" },
																};
		static List<string> DoubleOps = new List<string> { "&", "-", "+", "*", "++", "--" };
		public static int GetOperator(Expression expr, out int numArgs)
		{
			for (int level = 0; level < operators.Count; level++)
			{
				List<string> operatorLevel = operators[level];
				List<Token> tokens = expr.Tokens;
				int nParen = 0;
				bool leftToRight =  GetOpAssoc(level);
				for (int i = leftToRight ? tokens.Count - 1 : 0;  leftToRight ? i >= 0 : i < tokens.Count; i += leftToRight ? -1 : 1)
				{
					Token token = tokens[i];
					if (token.Type == TokenType.CloseParen)
						nParen++;
					else if (token.Type == TokenType.OpenParen)
						nParen--;
					else if (nParen == 0 && token.Type == TokenType.OperatorType && operatorLevel.Contains(token.Text)) 
					{
						numArgs = GetNumArgs(level, token);
						//we've found an operator, now we need to see if its acutally what we want
						if (leftToRight)
						{
							if (!DoubleOps.Contains(token) || (i > 0 && (tokens[i - 1].Type != TokenType.OperatorType ||
								(token == "*" && tokens[i - 1] == "*" && numArgs == 1))))
								return i;
						}
						else
						{
							if (!DoubleOps.Contains(token) || (i + 1 < tokens.Count && (tokens[i + 1].Type != TokenType.OperatorType ||
								(token == "*" && tokens[i + 1] == "*" && numArgs == 1))))
								return i;
						}
					}
				}
			}
			numArgs = -1;
			return -1;
		}

		private static int GetNumArgs(int level, Token tok)
		{
			switch (level)
			{
				case 13:
					return 1;
				case 14:
					if (tok.Text == ".")
						return 2;
					return 1;
				case 2:
					return 3;
				default:
					return 2;
			}
		}


		/// <summary>
		/// Checks if this operator goes right to left or left right
		/// </summary>
		/// <param name="level">level of operators were on</param>
		/// <returns>True if the Association is left to right. False otherwise</returns>
		private static bool GetOpAssoc(int level)
		{
			switch (level)
			{
				case 1:
				case 2:
				case 13:
					return false;
				default:
					return true;
			}
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

		public static List<Token> ToTokens(List<Expression> exps)
		{
			var evalTokens = new List<Token>();
			foreach(Expression exp in exps)
				if (exp.Args == null)
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
}
