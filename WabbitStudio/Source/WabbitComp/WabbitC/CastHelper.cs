using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC
{
    public static class CastHelper
    {
        private static List<string> ReservedModifiers = 
            new List<string>() {"const", "volatile", "struct", "union", "enum"};

        static bool HandleIndirectionSection(ref List<Token>.Enumerator tokens)
        {
            int parenCount = 0;
            while (!(parenCount == 0 && tokens.Current.Text == ")"))
            {

                tokens.MoveNext();
            }
            return true;
        }

        public static bool IsCast(List<Token> tokenList)
        {
            List<Token>.Enumerator tokens = tokenList.GetEnumerator();
            tokens.MoveNext();
            if (tokens.Current != "(")
            {
                return false;
            }
            tokens.MoveNext();

            // Loop through reserved modifiers
            while (ReservedModifiers.Contains(tokens.Current))
            {

                tokens.MoveNext();
            }

            // Loop through some strings, must have 1
            int nCount = 0;
            while (tokens.Current != "*" && tokens.Current != ")")
            {
                if (tokens.Current.Type != TokenType.StringType && 
                    tokens.Current.Type != TokenType.ReservedKeyword)
                {
                    return false;
                }
                nCount++;
                tokens.MoveNext();
            }

            if (nCount == 0)
            {
                return false;
            }

           HandleIndirectionSection(ref tokens);

           if (tokens.Current != ")")
           {
               return false;
           }
           return true;
        }
    }
}
