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

        public List<Token> Eval()
        {
			ReplaceDefines();
			var rpn = ShuntingYard(tokens);
			var result = CalcRPN(rpn);
			var tokenizer = new Tokenizer();
			tokenizer.Tokenize(result.Text);
			rpn = ShuntingYard(tokenizer.Tokens);
            return rpn;
        }

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
					break;*/
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
						//handle functions here
						if (i + 1 >= input.Count || input[i + 1].Type != TokenType.OpenParen)
							output.Add(token);
						else
							stack.Push(token);
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
				sb.Append(token.Text);
			return sb.ToString();
		}
    }
}
