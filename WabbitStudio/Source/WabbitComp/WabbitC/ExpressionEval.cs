using System;
using System.Collections.Generic;
using System.Text;

namespace WabbitC
{
    public class Expression
    {
        List<Token> tokens;
        public Expression(List<Token> tokens)
        {
            this.tokens = tokens;
        }

        public static int Eval()
        {
            return 0;
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
