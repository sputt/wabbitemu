using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.TokenPasses
{
	class TernaryOperator : TokenPass
	{

		public override List<Token> Run(List<Token> tokenList)
		{
			var newTokenList = new List<Token>();
			var tokens = tokenList.GetEnumerator();
			while (tokens.MoveNext())
			{
				var currentTokens = tokens;
				var result = IsTernaryOperator(ref currentTokens);

				if (result.Count > 0)
				{
					tokens = currentTokens;
					newTokenList.Add(Tokenizer.ToToken("if"));
					newTokenList.Add(Token.OpenParenToken);


					newTokenList.Add(Token.CloseParenToken);
					newTokenList.Add(Token.OpenBraceToken);

					newTokenList.Add(Token.CloseBraceToken);

				}
				else
				{
					newTokenList.Add(tokens.Current);
				}
			}

			return newTokenList;
		}

		private List<Token> IsTernaryOperator(ref List<Token>.Enumerator tokens)
		{
			bool foundTernary = false;
			var tokensList = new List<Token>();
			int nParen = 0;
			while (tokens.Current.Type != TokenType.StatementEnd)
			{
				tokensList.Add(tokens.Current);
				if (tokens.Current.Type == TokenType.TernaryConditional)
					foundTernary = true;
				else if (tokens.Current.Type == TokenType.OpenParen)
				{
					nParen++;
					tokensList.Clear();
				}
				else if (tokens.Current.Type == TokenType.CloseParen)
				{
					nParen--;
					if (foundTernary)
						return tokensList;
				}
				else if (tokens.Current.Text == "=")
					//if we've found an assingment operator, we can't have a ternary before that.
					//everything after is fair game (and probably is)
					tokensList.Clear();

				tokens.MoveNext();
			}
			if (!foundTernary)
				tokensList.Clear();
			return tokensList;
		}
	}
}
