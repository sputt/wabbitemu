using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.TokenPasses
{
    class ArrayDereference : TokenPass
    {
        private List<Token> IsArrayDereference(ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Token>();
            if (tokens.Current.Type == TokenType.StringType)
            {
                tokens.MoveNext();

                if (tokens.Current.Text == "[")
                {
                    tokens.MoveNext();

                    int nBrackets = 1;
                    while (nBrackets > 0)
                    {
                        if (tokens.Current.Text == "[")
                            nBrackets++;
                        else if (tokens.Current.Text == "]")
                            nBrackets--;

                        if (nBrackets > 0)
                        {
                            tokenList.Add(tokens.Current);
                            tokens.MoveNext();
                        }
                    }
                }
            }

            return tokenList;
        }

        public override List<Token> Run(List<Token> tokenList)
        {
            
            var newTokenList = new List<Token>();
            var tokens = tokenList.GetEnumerator();
            while (tokens.MoveNext())
            {
                var currentTokens = tokens;
                var result = IsArrayDereference(ref currentTokens);

                if (result.Count > 0)
                {
                    
                    newTokenList.AddRange(Tokenizer.Tokenize("*("));
                    // Add the name
                    newTokenList.Add(tokens.Current);
                    
                    newTokenList.AddRange(Tokenizer.Tokenize("+("));
                    newTokenList.AddRange(result);

                    newTokenList.AddRange(Tokenizer.Tokenize("))"));

                    tokens = currentTokens;
                }
                else
                {
                    newTokenList.Add(tokens.Current);
                }
            }

            return newTokenList;
        }
    }
}
