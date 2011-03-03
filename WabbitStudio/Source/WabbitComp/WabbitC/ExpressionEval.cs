using System;
using System.Collections.Generic;
using System.Text;

namespace WabbitC
{
    public class Expression
    {
        List<Token> tokens;
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

        public static int Eval()
        {
            return 0;
        }

		const string[] operators = { "+-", "*/", "^%" };
		void FillStack(Expression equation, List<Expression> stack)
		{
			stack.Add(equation);
			for (int i = 0; i < stack.Count; i++)
			{
				if (stack[i].Operator != '\0')
				{
					string str = stack[i].
				}
			}
			return null;
		}

		int GetOperator(string equation, string[] operators)
		{
			for (int opIndex = 0; opIndex < operators.Length; opIndex++)
			{
				int nOpen = 0;
				for (int i = equation.Length; i >= 0; i--)
				{
					char ch = equation[i];
					if (ch == ')')
						nOpen++;
					else if (ch == '(')
						nOpen--;
					else if (nOpen == 0 && operators[opIndex].IndexOf(ch) != -1)
					{
						if ((ch != '-' && ch != '+') || (i != 0 && IsRightSign(equation[i - 1], operators, i)))
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
