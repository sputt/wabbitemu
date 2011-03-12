using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

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

		int operands;
		public int Operands
		{
			get { return operands; }
		}

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
			MoveConstantsToEnd(ref test);
			CalculateStack(ref test);
			return test;
        }

		private void MoveConstantsToEnd(ref List<Expression> expressions)
		{
			List<int> numLocs = new List<int>();
			Token opTok = null;
			int i;
			for (i = 0; i < expressions.Count; i++)
			{
				Expression exp = expressions[i];
				var token = exp.Tokens[0];
				if (opTok == null && ((token == "*" && exp.Operands == 2) || token == "+"))
					if (opTok != null && opTok != token)
						break;
					else
						opTok = token;
				else if (token.Type == TokenType.IntType || token.Type == TokenType.RealType)
					numLocs.Add(i);
			}
			//we broke early
			if (i != expressions.Count)
				return;
			int locToSwap = expressions.Count - 1;
			foreach (int loc in numLocs)
			{
				while (numLocs.Contains(locToSwap))
					locToSwap--;
				if (locToSwap <= loc)
					break;
				expressions.Swap(loc, locToSwap);
				locToSwap--;
			}

			return;
			/*int totalOps;
			string lastTok;
			for (int i = 0; i < expressions.Count; i++)
			{
				lastTok = null;
				totalOps = 0;
				Expression exp = expressions[i];
				while (((exp.Tokens[0] == "*" && exp.Operands == 2) || exp.Tokens[0] == "+") && (lastTok == null || exp.Tokens[0] == lastTok))
				{
					exp = expressions[++i];
					totalOps++;
					lastTok = exp.Tokens[0];
				}
				if (lastTok != null)
				{
					while (totalOps > 0)
					{
						if (exp.Tokens[0].Type == TokenType.IntType || exp.Tokens[0].Type == TokenType.RealType)
						{
							if (i + totalOps >= tokens.Count)
								break;
							if (expressions[i + totalOps].Tokens[0].Type != TokenType.IntType && expressions[i + totalOps].Tokens[0].Type != TokenType.RealType)
							{
								expressions.Swap(i, i + totalOps);
								break;
							}
							else
								totalOps--;
						}
						else if (i + 1 < tokens.Count)
							exp = expressions[++i];
						else
							break;
					}

				}
			}*/
		}

		private void OptimizeTokens()
		{
			//remove parens around entire expression
			/*if (tokens[0].Type == TokenType.OpenParen && tokens[tokens.Count - 1].Type == TokenType.CloseParen)
			{
				tokens.RemoveAt(0);
				tokens.RemoveAt(tokens.Count - 1);
			}*/
			int nParen = 0;
			List<int> curOpLevel = new List<int>();
			List<int> parenLoc = new List<int>();
			bool canRemoveParen = true;
			curOpLevel.Add(-1);
			for (int i = 0; i < tokens.Count; i++)
			{
				Token token = tokens[i];
				if (token.Type == TokenType.OpenParen)
				{
					
					if (i > 0 && tokens[i - 1].Type != TokenType.StringType)
					{
						nParen++;
						parenLoc.Add(i);
						curOpLevel.Add(-1);
					}
					else
					{
						//skip function
						int curParenLevel = nParen;
						while (token.Type != TokenType.CloseParen && curParenLevel == nParen)
							token = tokens[++i];
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
					else if (curOpLevel[nParen] != level)
						canRemoveParen = false;
				}
			}
		}

		private void CalculateStack(ref List<Expression> expressions)
		{
			if (expressions.Count == 1 && expressions[0].Tokens.Count > 1 && expressions[0].Args == null)
			{
				expressions = new Expression(expressions[0].Tokens).Eval();
			}
			else if (expressions[0].Args == null)
			{
				var temp = Expression.ToTokens(expressions);
				int count = temp.Count;
				CalculateStack(ref temp);
				if (count != temp.Count)
				{
					expressions.Clear();
					foreach (Token token in temp)
						expressions.Add(new Expression(token));
				}
			}
		}

		private void CalculateStack(ref Expression exp)
		{
			if (exp.Args != null || exp.Operands != 0)
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
				return new Expression(tokens);
			Stack<Token> stack = new Stack<Token>();
			int i = tokens.Count;
			Expression result = null;
			while (i > 0)
			{
				while (tokens[--i].Type != TokenType.OperatorType)
					stack.Push(tokens[i]);
				Token op = tokens[i];
				int operatorAt = i;
				List<Token> toks = new List<Token>();
				while (stack.Count > 0 && toks.Count < 2)
					toks.Add(stack.Pop());
				var exp = ApplyOperator(op, toks);
				if (exp != null)
				{
					result = exp;
					tokens[tokens.Count - 1 - i] = result.Tokens[0];
					tokens.RemoveAt(operatorAt);
					tokens.RemoveAt(tokens.Count - i);
				}
				if (i > 0 && result != null)
					stack.Push(result.Tokens[0]);
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
					switch (numArgs)
					{
						case 3:
							stack.Insert(i, op);
							stack.Insert(i + 1, leftSide);
							stack.Insert(i + 2, rightSide);
							stack.Insert(i + 3, ternarySide);
							break;
						case 2:
							var testleft = leftSide.Eval();
							var testright = rightSide.Eval();
							stack.Insert(i, op);
							stack.InsertRange(i + 1, testleft);
							stack.InsertRange(i + 2 + testleft.Count - 1, testright);
							i = i + 2 + testleft.Count - 1;
							/*if ((testleft.Count == 1 && (testleft[0].Tokens[0].Type == TokenType.IntType || testleft[0].Tokens[0].Type == TokenType.RealType)) &&
									(testright.Count == 1 && (testright[0].Tokens[0].Type == TokenType.IntType || testright[0].Tokens[0].Type == TokenType.RealType)))
							{
								var resultExp = ApplyOperator(op.Tokens[0], testleft[0].Tokens[0], testright[0].Tokens[0]);
								stack.Insert(i, resultExp);
							}
							else
							{
								stack.Insert(i, op);
								stack.Insert(i + 1, leftSide);
								stack.Insert(i + 2, rightSide);
							}*/
							break;
						case 1:
							stack.Insert(i, op);
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
		static List<List<string>> operators = new List<List<string>> { 
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
																	new List<string> {"++", "−−", "--", "!", "~", "&", "*", "+", "-" },
																	new List<string> {".", "++", "−−", "--" },
																};
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
							if ((token.Text != "-" && token.Text != "+" && token.Text != "*" && token.Text != "++" && token.Text != "--" && token.Text != "&")
								|| (i > 0 && tokens[i - 1].Type != TokenType.OperatorType))
								return i;
						}
						else
						{
							if ((token.Text != "-" && token.Text != "+" && token.Text != "*" && token.Text != "++" && token.Text != "--" && token.Text != "&")
								|| (i + 1 < tokens.Count && tokens[i + 1].Type != TokenType.OperatorType))
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
				case 12:
					return 1;
				case 13:
					if (tok.Text == ".")
						return 2;
					return 1;
				case 1:
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
				case 0:
				case 1:
				case 12:
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
