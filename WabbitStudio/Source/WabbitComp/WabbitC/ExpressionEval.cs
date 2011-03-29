using System;
using System.Collections.Generic;
using System.Text;

namespace WabbitC
{
    public class Expression
    {
        List<Token> tokens;
		/// <summary>
		/// The list of tokens in the current expression
		/// </summary>
		public List<Token> Tokens
		{
			get { return tokens; }
		}

		/// <summary>
		/// Returns the first Token in the expression
		/// </summary>
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
		/// <summary>
		/// If this expression is a function, this returns a list of all parameters inputted for it.
		/// Returns null if not a function.
		/// </summary>
		public List<Expression> Args
		{
			get { return args; }
		}

		bool isCast;
		/// <summary>
		/// Returns true if this expression is the special cast operator
		/// Returns false otherwise
		/// </summary>
		public bool IsCast
		{
			get { return isCast; }
		}

		int operands;
		/// <summary>
		/// Returns the number of operands this expression needs to function
		/// If 0 is returned it is likely a Immediate or Declaration
		/// </summary>
		public int Operands
		{
			get { return operands; }
		}

        bool isPrefix = false;
		/// <summary>
		/// Returns true if this ++ or -- operator is a prefix operator
		/// Only valid if expression is ++ or --
		/// </summary>
		public bool IsPrefix
		{
			get { return isPrefix; }
		}

        public Expression(List<Token> tokens)
        {
            this.tokens = tokens;
        }

		private Expression(List<Token> tokens, int operands)
		{
			this.operands = operands;
			this.tokens = tokens;
		}

		public Expression(Token token)
		{
			this.tokens = new List<Token> { token };
		}

		/// <summary>
		/// Attempts to evaluate the current expression.
		/// Optimizes parens, then makes an RPN stack of the tokens.
		/// Finally attempts to calculate known parts of the stack and reduce them as much as possible
		/// </summary>
		/// <returns>An RPN stack of the evaluated tokens. Highest index of
		/// the list is the top of the stack</returns>
        public List<Expression> Eval()
        {
			ReplaceDefines();
			OptimizeTokens();
			var test = FillStack(this);
			CalculateStack(ref test);
			return test;
        }

		private static List<Token> ReadBetweenParens(List<Token> tokens, int i, int nParen, ref int j)
		{
			Token token = tokens[i];
			List<Token> tempList = new List<WabbitC.Token>();
			//skip function too
			int curParenLevel = nParen;
			j = i;
			while (!(token.Type == TokenType.CloseParen && curParenLevel == nParen))
			{
				token = tokens[j++];
				if (token.Type == TokenType.CloseParen)
					curParenLevel--;
				else if (token.Type == TokenType.OpenParen)
					curParenLevel++;
				tempList.Add(token);
			}
			return tempList;
		}

		private int SkipCasts(ref int i, ref int nParen, ref List<int> parenLoc, ref List<int> curOpLevel)
		{
			int j = -1;
			List<Token> tempList = ReadBetweenParens(tokens, i, nParen, ref j);
			if (CastHelper.IsCast(tempList))
			{
				i = j;
				if (tokens[i].Type == TokenType.OpenParen)
				{
					parenLoc.Add(i);
					curOpLevel.Add(-1);
					curOpLevel[nParen] = -1;		//impossible to remove
					nParen++;						//skip the first paren of the cast
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
						ReadBetweenParens(tokens, i, nParen, ref j);
						i = j;
					}
				} 
				else if (token.Type == TokenType.OpenParen)
				{
					int j = i;
					SkipCasts(ref i, ref nParen, ref parenLoc, ref curOpLevel);
					if (i == j)
					{
						nParen++;
						parenLoc.Add(i);
						curOpLevel.Add(-1);
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
					else if (curOpLevel[nParen] != level || IsBannedParenOptimze(token))
						canRemoveParen = false;
				}
			}
		}

        private bool IsBannedParenOptimze(Token token)
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
			//we've now calculated each part of the stack, lets put them together
			var listTokens = Expression.ToTokens(expressions);
			//and try to recalculate the thing as a whole
			var result = CalculateStack(ref listTokens);
			if (result != null)
			{
				//we've distilled it down to one exp, good work
				expressions.Clear();
				expressions.Add(result);
			}
		}

		private void CalculateStack(ref Expression exp)
		{
			//if its a function, an operator, or a cast dont fuck with it
			if (exp.Args != null || exp.Operands != 0 || exp.IsCast)
				return;
			//else eval it
			var evalExp = exp.Eval();
			//put it back together as one big thing
			var tokens = Expression.ToTokens(evalExp);
			//and calculate that
			var result = CalculateStack(ref tokens);
			//if we calculated something, give that, else return the original
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
				//TODO: use operands instead of assuming 2
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

		/// <summary>
		/// Tries to take a list of tokens and combine them into a single expression
		/// </summary>
		/// <param name="op">Operator to apply to tokens</param>
		/// <param name="tokens">List of operands necessary for op</param>
		/// <returns>Null if the tokens supplied cannot be distilled to a single token.
		///	Otherwise returns an expression of one token.</returns>
		private Expression ApplyOperator(Token op, List<Token> tokens)
		{
			switch (tokens.Count)
			{
				case 1:
					return ApplyOperator(op, tokens[0]);
				case 2:
					return ApplyOperator(op, tokens[0], tokens[1]);
			}
			//ternary falls through
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
					//negation is just subtraction from 0
                    result = Token.ZeroToken - tok1;
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
					//we cant eval =
                    result = null;
                    break;
				case "&":
					result = tok1 & tok2;
					break;
				case "|":
					result = tok1 | tok2;
					break;
				case "||":
					//cant override || or && :/
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

		/// <summary>
		/// Takes a single expression and breaks it apart into an RPN based stack
		/// </summary>
		/// <param name="input">Expression to split into components</param>
		/// <returns>A list of expression where the last element is the top of the RPN stack</returns>
		public List<Expression> FillStack(Expression input)
		{
			List<Expression> stack = new List<Expression>();
			stack.Add(input);
			for (int i = 0; i < stack.Count; i++)
			{
				Expression curExpr = stack[i];
				int numArgs;
				//we need to see if we can find an operator to split into two expressions
				int operatorIndex = Expression.GetOperator(curExpr, out numArgs);
				if (operatorIndex != -1)
				{
					//we've got something, lets break it into parts
					int j;
					Expression leftSide, rightSide, ternarySide = null, op;
					List<Token> tokenList = new List<Token>();
					for (j = 0; j < operatorIndex; j++)
						tokenList.Add(curExpr.Tokens[j]);
					//everything before op
					leftSide = new Expression(tokenList);

					tokenList = new List<Token>();
					//we've got a cast, lets treat it like as a single operator
					if (curExpr.Tokens[j].Type == TokenType.OpenParen)
					{
						int k = 0;
						op = new Expression(ReadBetweenParens(curExpr.Tokens, j, 0, ref k));
						j = k;
						op.isCast = true;
					}
					else
					{
						//get a single token for the operator
						op = new Expression(new List<Token> { curExpr.Tokens[j++] }, numArgs);
					}
					//special case for ternary, we need to read everything to the ':', then skip and
					//keep going to the end of the statement
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
						//otherwise get everything to the right of the op
						for (; j < curExpr.Tokens.Count; j++)
							tokenList.Add(curExpr.Tokens[j]);
						rightSide = new Expression(tokenList);
					}
					//remove the current part on the stack
					stack.Remove(curExpr);
					//and at in the new operator first, as per RPN
					stack.Insert(i, op);
					//and add in the args left (first) to right/ternary (last)
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
							//if we have a pre/post operator we need to make sure we specify which it is
                            if (op.Token == "++" || op.Token == "--")
                                if (leftSide.Token == null)
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
					//no operators, we should check for parens and functions
					if (curExpr.Tokens.Count < 1)
						continue;
					Token curToken = curExpr.Tokens[0];
					if (curToken.Type == TokenType.OpenParen)
					{
						//handle paren, just read everything between and place on the stack
						//we've already tried to remove parens so we dont want to mess with these
						int j = 0;
						List<Token> insideTokens = ReadBetweenParens(curExpr.Tokens, 0, 0, ref j);
						//returns with parens on each end, we dont need
						insideTokens.RemoveAt(insideTokens.Count - 1);
						insideTokens.RemoveAt(0);
						Expression insideExp = new Expression(insideTokens);
						stack[i] = insideExp;
					}
					//check for function (word with paren after)
					else if ((curToken.Type == TokenType.StringType || curToken == "sizeof") && curExpr.Tokens.Count > 1 &&
								curExpr.Tokens[1].Type == TokenType.OpenParen)
					{
						List<Expression> args = new List<Expression>();
						curToken = curExpr.Tokens[2];
						//read in all args
						int j = 0;
						List<Token> insideTokens = ReadBetweenParens(curExpr.Tokens, 1, 0, ref j);
						//returns with parens on each end, we dont need
						insideTokens.RemoveAt(insideTokens.Count - 1);
						insideTokens.RemoveAt(0);
						var arg = new List<Token>();
						Expression argExp;
						foreach (var token in insideTokens)
						{
							//check for seperator
							if (token == ",")
							{
								argExp = new Expression(arg);
								args.Add(argExp);
								arg = new List<Token>();
							}
							else
							{
								arg.Add(token);
							}
						}
						//add last arg
						argExp = new Expression(arg);
						args.Add(argExp);
						curExpr.args = args;
					}
				}
			}
			return stack;
		}

		/// <summary>
		/// This is a list of all operators handled by the evaluator. The larger the index, the lower the precedence.
		/// It is iterated through in GetOperator starting with the lowest precedence (thus the least sticking power)
		/// 
		/// based on: http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
		/// </summary>
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
																	new List<string> {"#cast", "!", "~", "&", "*", "+", "-", "++", "−−", "--"},
																	new List<string> {".", "++", "−−", "--" },
																};

		private const int TernaryLevel = 2;
		private const int CastLevel = 13;
		private const int ParenLevel = 14;

		/// <summary>
		/// These operators can mean two things depending on the context
		/// eg: & can be bitwise AND or Address Of
		/// </summary>
		static List<string> DoubleOps = new List<string> { "&", "-", "+", "*", "++", "--" };
		public static int GetOperator(Expression expr, out int numArgs)
		{
			int foundOp = -1;
			numArgs = -1;
			//go through each level of operators looking for one in the current statement
			for (int level = 0; level < operators.Count; level++)
			{
				List<string> operatorLevel = operators[level];
				List<Token> tokens = expr.Tokens;
				//this determines the direction we look through the tokens
				//left to right means we start need move backwards through the tokens, and right to left vice versa
				bool leftToRight =  GetOpAssoc(level);
				//current paren level
				int nParen = 0;
				for (int i = leftToRight ? tokens.Count - 1 : 0;  leftToRight ? i >= 0 : i < tokens.Count; i += leftToRight ? -1 : 1)
				{
					int j = -1;
					bool isCast = false;
					Token token = tokens[i];
					List<Token> parenTokens = null;
					if (token.Type == TokenType.CloseParen)
						nParen--;
					else if (token.Type == TokenType.OpenParen)
					{
						if (level == CastLevel)
						{
							parenTokens = ReadBetweenParens(tokens, i, nParen, ref j);
							isCast = CastHelper.IsCast(parenTokens);
							if (i > 0 && isCast)
								isCast = tokens[i - 1].Type != TokenType.StringType && tokens[i - 1] != "sizeof";
						}
						if (!isCast)
							nParen++;
					}
					//almost all code before this is just to check for the special cast operator to see if we found one
					//casts are horrible horrible creatures and cause tons of problems
					if (nParen == 0 && ((token.Type == TokenType.OperatorType && operatorLevel.Contains(token.Text))
								|| isCast == true))
					{
						//if we've found something and this new operator is not a cast screw it, its almost gauranteed
						//not to work. This is meant to fix issues like (#cast) #DoubleOp #var
						if (foundOp >= 0 && !isCast)
							continue;
						numArgs = GetNumArgs(level, token);
						//we've found an operator, now we need to see if its actually what we want
						if (leftToRight)
						{
							//if its not a double op were good
							if (!DoubleOps.Contains(token))
								return i;
							//otherwise check if the context is valid. If so mark that we found it
							if (i > 0 && (tokens[i - 1].Type != TokenType.OperatorType ||
								(token == "*" && tokens[i - 1] == "*" && numArgs == 1)))
								foundOp = i;
						}
						else
						{
							if (isCast)
							{
								//if its a cast and its not at the very end were good
								if (j < tokens.Count)
									return i;
							}
							//same as above
							else if (!DoubleOps.Contains(token))
								return i;
							if (i + 1 < tokens.Count && (tokens[i + 1].Type != TokenType.OperatorType ||
									(token == "*" && tokens[i + 1] == "*" && numArgs == 1)))
								foundOp = i;
						}
					}
					//skip over what we thought was a cast
					if (isCast)
						i += parenTokens.Count - 1;
				}
			}
			return foundOp;
		}

		/// <summary>
		/// Returns the expected number of operands for this particular token
		/// </summary>
		/// <param name="level">Current level of operators</param>
		/// <param name="tok">Token to resolve special cases</param>
		/// <returns>Number of operands the token operates on</returns>
		private static int GetNumArgs(int level, Token tok)
		{
			switch (level)
			{
				case CastLevel:
					return 1;
				case ParenLevel:
					if (tok.Text == ".")
						return 2;
					return 1;
				case TernaryLevel:
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
