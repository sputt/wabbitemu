﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.TokenPasses
{
    class CompoundAssignmentRemover : TokenPass
    {
		string[] compoundOperators = { "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=" };
		private List<Token> IsCompoundAssign(ref List<Token>.Enumerator tokens)
        {
			var tokenList = new List<Token>();
			if (tokens.Current.Type == TokenType.StringType)
			{
				var nameToken = tokens.Current;
				tokens.MoveNext();
				if (tokens.Current.Type == TokenType.OperatorType && compoundOperators.Contains(tokens.Current.Text))
				{
					tokenList.Add(nameToken);
					tokenList.Add(Token.AssignmentOperatorToken);
					tokenList.Add(nameToken);
					Token operatorToken = tokens.Current;
					operatorToken.Text = operatorToken.Text.Remove(operatorToken.Text.Length - 1);
					tokenList.Add(operatorToken);
					tokenList.Add(Token.OpenParenToken);
					tokens.MoveNext();

					while (tokens.Current.Type != TokenType.StatementEnd)
					{
						tokenList.Add(tokens.Current);
						tokens.MoveNext();
					}
					tokenList.Add(Token.CloseParenToken);
					tokenList.Add(tokens.Current);
					return tokenList;
				}
			}
			return tokenList;
        }

        public override List<Token> Run(List<Token> tokenList)
        {
            var tokenizer = new Tokenizer();
            var newTokenList = new List<Token>();
            var tokens = tokenList.GetEnumerator();
            while (tokens.MoveNext())
            {
				var currentTokens = tokens;
                var result = IsCompoundAssign(ref currentTokens);

                if (result.Count > 0)
                {
					foreach (Token token in result)
						newTokenList.Add(token);
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