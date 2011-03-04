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

		public char Operator
		{
			get
			{
				if (tokens.Count != 1)
					return '\0';
				foreach(string ops in operators) {
					if (ops.Contains(tokens[0].TokenText))
						return tokens[0].TokenText[0];
				}
				return '\0';
			}
		}

        public Expression(List<Token> tokens)
        {
            this.tokens = tokens;
        }

        public int Eval()
        {
			List<Expression> stack = new List<Expression>();
			FillStack(this, stack);
			double output = CalculateStack(stack);
            return 0;
        }

		private double CalculateStack(List<Expression> stack)
		{
			double output = 0;
			Expression left = stack[stack.Count - 1];
			for (int i = stack.Count - 2; i >= 0; i--)
			{
				Expression right = stack[i--];
				Expression op = stack[i];
				output = CalcExpression(left, right, op);
			}
			return output;
		}

		private Expression CalcExpression(Expression left, Expression right, Expression op)
		{
			double output = 0.0;
			double leftNum = float.Parse(left.Token.TokenText);
			double rightNum = float.Parse(right.Token.TokenText);
			switch (op.Token.TokenText)
			{
				case "+":
					output = leftNum + rightNum;
					break;
				case "-":
					output = leftNum - rightNum;
					break;
				case "*":
					output = leftNum * rightNum;
					break;
				case "/":
					output = leftNum / rightNum;
					break;
				case "%":
					output = leftNum % rightNum;
					break;
			}
			return output;
		}

		string[] operators = { "+-", "*/", "%" };
		void FillStack(Expression equation, List<Expression> stack)
		{
			stack.Add(equation);
			for (int i = 0; i < stack.Count; i++)
			{
				if (stack[i].Operator == '\0')
				{
					int opIndex = GetOperator(stack[i], operators);
					if (opIndex != -1)
					{
						Expression oldExpression = stack[i];
						int j = 0;
						List<Token> list = new List<Token>();
						for (; j < opIndex; j++)
							list.Add(stack[i].Tokens[j]);
						Expression left = new Expression(list);
						Expression op = new Expression(new List<Token>() { stack[i].Tokens[j++] });
						list = new List<Token>();
						for (; j < stack[i].Tokens.Count; j++)
							list.Add(stack[i].Tokens[j]);
						Expression right = new Expression(list);
						stack.Remove(oldExpression);
						stack.Insert(i, op);
						stack.Insert(i + 1, left);
						stack.Insert(i + 2, right);
					}
				}
			}
		}

		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();
			foreach (Token token in tokens)
				sb.Append(token.TokenText);
			return sb.ToString();
		}

		int GetOperator(Expression expression, string[] operators)
		{
			for (int opIndex = 0; opIndex < operators.Length; opIndex++)
			{
				int nOpen = 0;
				for (int i = expression.Tokens.Count - 1; i >= 0; i--)
				{
					Token token = expression.Tokens[i];
					char ch = token.TokenText[0];
					if (ch == ')')
						nOpen++;
					else if (ch == '(')
						nOpen--;
					else if (nOpen == 0 && operators[opIndex].IndexOf(ch) != -1)
					{
						if ((ch != '-' && ch != '+') || (i != 0 && IsRightSign(tokens[i - 1].TokenText[0], operators, i)))
							return i;
					}
				}
			}
			return -1;
		}

		private bool IsRightSign(char ch, string[] operators, int i)
		{
			for (; i < operators.Length; i++)
				if (operators[i].IndexOf(ch) != -1)
					return false;
			return true;
		}


		bool IsOperator(Token token)
        {
            return token.TokenText == "+" || token.TokenText == "-" || token.TokenText == "*" || token.TokenText == "/" || token.TokenText == "%" ||
                token.TokenText == "&" || token.TokenText == "|" || token.TokenText == "^" || token.TokenText == "~" ||
                token.TokenText == "<<" || token.TokenText == ">>" ||
                token.TokenText == "++" || token.TokenText == "--" ||
                token.TokenText == "==" || token.TokenText == "!=" || token.TokenText == ">" || token.TokenText == "<" || token.TokenText == "<=" || token.TokenText == ">=" ||
                token.TokenText == "!" || token.TokenText == "&&" || token.TokenText == "||";
        }
    }
}
