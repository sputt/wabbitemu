using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

namespace WabbitC.TokenPasses
{
    class Bracer : TokenPass
    {
        private List<Token> SkipParens(ref List<Token>.Enumerator tokens)
        {
            Debug.Assert(tokens.Current.Type == TokenType.OpenParen);
            var tokenList = new List<Token>();

            tokenList.Add(tokens.Current);
            tokens.MoveNext();

            int nCount = 1;
            while (nCount > 0)
            {
                if (tokens.Current.Text == "(")
                    nCount++;
                else if (tokens.Current.Text == ")")
                    nCount--;

                tokenList.Add(tokens.Current);
                if (nCount > 0)
                {
                    tokens.MoveNext();
                }
            }
            return tokenList;
        }

        private List<Token> GetStatement(ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Token>();

            while (tokens.Current.Type != TokenType.StatementEnd)
            {
                tokenList.Add(tokens.Current);
                tokens.MoveNext();
            }
            tokenList.Add(tokens.Current);
            return tokenList;
        }

        private List<Token> BraceBlock(ref List<Token>.Enumerator tokens)
        {
            var tokenizer = new Tokenizer();
            var tokenList = new List<Token>();

            if (tokens.Current.Type == TokenType.OpenBlock)
            {
                tokenList.Add(tokens.Current);
            }
            else
            {
                tokenizer.Tokenize("{");
                tokenList.AddRange(tokenizer.Tokens);

                tokenList.AddRange(GetStatement(ref tokens));

                tokenizer.Tokenize("}");
                tokenList.AddRange(tokenizer.Tokens);
            }
            return tokenList;
        }

        private List<Token> RunBracer(ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Token>();
            while (tokens.MoveNext())
            {
                tokenList.Add(tokens.Current);

                if (tokens.Current.Type == TokenType.ReservedKeyword)
                {
                    switch (tokens.Current.Text)
                    {
                        case "if":
                        case "while":
                        case "for":
                            tokens.MoveNext();
                            tokenList.AddRange(SkipParens(ref tokens));
                            tokens.MoveNext();
                            tokenList.AddRange(BraceBlock(ref tokens));
                            break;

                        case "else":
                        case "do":
                            tokens.MoveNext();
                            tokenList.AddRange(BraceBlock(ref tokens));
                            break;
                    }
                }
            }

            return tokenList;
        }

        public override List<Token> Run(List<Token> tokenList)
        {
            var tokens = tokenList.GetEnumerator();
            return RunBracer(ref tokens);
        }
    }
}
