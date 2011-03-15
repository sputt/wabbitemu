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
			bool isIfStatement = false;
			if (tokens.Current == "if")
				isIfStatement = true;
            var tokenList = new List<Token>();
            while (tokens.Current.Type != TokenType.StatementEnd)
            {
                tokenList.Add(tokens.Current);
                tokens.MoveNext();
            }
			tokenList.Add(tokens.Current);
			var temp = tokens;
			tokens.MoveNext();
			if (tokens.Current == "else" && isIfStatement)
			{
				while (tokens.Current == "else" && isIfStatement)
				{
					tokenList.Add(tokens.Current);
					tokens.MoveNext();
					if (tokens.Current == "if")
					{
						tokenList.Add(tokens.Current);
						tokenList.AddRange(HandleIfWhileFor(ref tokens));
						temp = tokens;
						tokens.MoveNext();
					}
					else
					{
						tokenList.AddRange(BraceBlock(ref tokens));
					}
				}
			}
			else
			{
				tokens = temp;
			}
            return tokenList;
        }

        private List<Token> BraceBlock(ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Token>();

            if (tokens.Current.Type == TokenType.OpenBlock)
            {
				int nParen = 0;
                tokenList.Add(tokens.Current);
				tokens.MoveNext();
				while (tokens.Current.Type != TokenType.CloseBlock)
				{
					if (tokens.Current.Type == TokenType.OpenBlock)
						nParen++;
					else if (tokens.Current.Type == TokenType.CloseBlock && nParen > 0)
						nParen--;
					tokenList.Add(tokens.Current);
					tokens.MoveNext();
				}
				tokenList.Add(tokens.Current);
            }
            else
            {
                tokenList.Add(Token.OpenBraceToken);

				var listStatement = GetStatement(ref tokens);
				tokenList.AddRange(this.Run(listStatement));
				tokenList.Add(Token.CloseBraceToken);				
            }
            return tokenList;
        }

		private List<Token> HandleIfWhileFor(ref List<Token>.Enumerator tokens)
		{
			var tokenList = new List<Token>();
			tokens.MoveNext();
			tokenList.AddRange(SkipParens(ref tokens));
			tokens.MoveNext();
			tokenList.AddRange(BraceBlock(ref tokens));
			return tokenList;
		}

		private List<Token> HandleElseDo(ref List<Token>.Enumerator tokens)
		{
			var tokenList = new List<Token>();
			var tempSave = tokens;
			tokens.MoveNext();
			if (tokens.Current == "if")
			{
				tokenList.Add(Token.OpenBraceToken);
				tokenList.Add(tokens.Current);
				tokenList.AddRange(HandleIfWhileFor(ref tokens));
				tempSave = tokens;
				tokens.MoveNext();
				if (tokens.Current == "else")
				{
					tokenList.Add(tokens.Current);
					tokenList.AddRange(HandleElseDo(ref tokens));
				}
				else
					tokens = tempSave;
				tokenList.Add(Token.CloseBraceToken);
			}
			else
			{
				tokenList.AddRange(BraceBlock(ref tokens));
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
							tokenList.AddRange(HandleIfWhileFor(ref tokens));
                            break;

                        case "else":
                        case "do":
							tokenList.AddRange(HandleElseDo(ref tokens));
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
