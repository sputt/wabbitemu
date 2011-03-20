using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace WabbitC.TokenPasses
{
	class ForLoopRemover : TokenPass
	{
		public override List<Token> Run(List<Token> tokenList)
		{
			var newTokenList = new List<Token>();
			var tokens = tokenList.GetEnumerator();
			while (tokens.MoveNext())
			{
				var currentTokens = tokens;
				var result = IsForLoop(ref currentTokens);

				if (result.Count > 0)
				{
					tokens = currentTokens;
					newTokenList.AddRange(result[0]);
					newTokenList.Add(Tokenizer.ToToken("while"));
					newTokenList.Add(Token.OpenParenToken);
					//add in loop
					newTokenList.AddRange(result[1]);
					newTokenList.Add(Token.CloseParenToken);
					newTokenList.Add(Token.OpenBraceToken);
					newTokenList.AddRange(result[2]);
					newTokenList.Add(Token.CloseBraceToken);
				}
				else
				{
					newTokenList.Add(tokens.Current);
				}
			}

			return newTokenList;
		}

		private List<Token> GetStatement(ref List<Token>.Enumerator tokens)
		{
			var statement = new List<Token>();
			int nParen = 0;
			while (tokens.Current.Type != TokenType.StatementEnd && !(tokens.Current.Type == TokenType.CloseParen && nParen == 0))
			{
				if (tokens.Current.Type == TokenType.OpenParen)
					nParen++;
				else if (tokens.Current.Type == TokenType.CloseParen)
					nParen--;
				statement.Add(tokens.Current);
				tokens.MoveNext();
			}
			//add ; so it will be parsed correctly
			if (tokens.Current.Type == TokenType.StatementEnd)
				statement.Add(tokens.Current);
			else
				statement.Add(Token.StatementEndToken);
			tokens.MoveNext();
			return statement;
		}

		private List<List<Token>> IsForLoop(ref List<Token>.Enumerator tokens)
		{
			var tokensList = new List<List<Token>>();
			
			if (tokens.Current.Text == "for")
			{
				tokens.MoveNext();
				Debug.Assert(tokens.Current.Type == TokenType.OpenParen);
				tokens.MoveNext();			//skip for(

				var initialStatement = GetStatement(ref tokens);
				tokensList.Add(initialStatement);

				var conditionStatement = GetStatement(ref tokens);
				conditionStatement.Remove(Token.StatementEndToken);
                if (conditionStatement.Count == 0)
                {
                    conditionStatement.Add(new Token("1"));
                }
				tokensList.Add(conditionStatement);

				var finalStatement = GetStatement(ref tokens);
				

                if (tokens.Current.Type == TokenType.OpenBlock)
                {
                    tokens.MoveNext();			//skip {

                    var internalBlock = new List<Token>();
                    int nParen = 0;
                    while (!(tokens.Current.Type == TokenType.CloseBlock && nParen == 0))
                    {
                        if (tokens.Current.Type == TokenType.OpenBlock)
                            nParen++;
                        else if (tokens.Current.Type == TokenType.CloseBlock)
                            nParen--;
                        internalBlock.Add(tokens.Current);
                        tokens.MoveNext();
                    }
                    internalBlock.AddRange(finalStatement);
                    tokensList.Add(internalBlock);
                }
                else if (tokens.Current.Type == TokenType.StatementEnd)
                {
                    var internalBlock = new List<Token>();
                    internalBlock.AddRange(finalStatement);
                    tokensList.Add(internalBlock);
                }
                else
                {
                    throw new Exception("Invalid token of some kind");
                }
				//return with tokens.Current == "}" or token.Current == ";"
			}
			return tokensList;
		}
	}
}
