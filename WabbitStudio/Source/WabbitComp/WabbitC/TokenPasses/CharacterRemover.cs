using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.TokenPasses
{
    class CharacterRemover : TokenPass
    {
        private bool IsCharacter(ref List<Token>.Enumerator tokens, out char parsedChar)
        {
            var tokenList = new List<Token>();
			parsedChar = '\0';
            if (tokens.Current.Text == "'")
            {
                tokens.MoveNext();

				if (tokens.Current.Text == "\\")
				{
					tokens.MoveNext();
					switch (tokens.Current.Text)
					{
						case "a":
							parsedChar = '\a';
							break;
						case "b":
							parsedChar = '\b';
							break;
						case "f":
							parsedChar = '\f';
							break;
						case "n":
							parsedChar = '\n';
							break;
						case "r":
							parsedChar = '\r';
							break;
						case "t":
							parsedChar = '\t';
							break;
						case "v":
							parsedChar = '\v';
							break;
						case "'":
							parsedChar = '\'';
							break;
						case "\"":
							parsedChar = '\"';
							break;
						case "\\":
							parsedChar = '\\';
							break;
						case "?":
							parsedChar = '?';
							break;
						case "0":
							parsedChar = '\0';
							break;
						case "x":
							tokens.MoveNext();
							parsedChar = Convert.ToChar(Convert.ToInt16(tokens.Current.Text));
							break;
					}
				}
				else
				{
					parsedChar = tokens.Current.Text[0];
				}
				tokens.MoveNext();
				if (tokens.Current.Text != "'")
					throw new Exception("Invalid character");
			
				return true;
            }
			return false;
        }

        public override List<Token> Run(List<Token> tokenList)
        {
            var newTokenList = new List<Token>();
            var tokens = tokenList.GetEnumerator();
            while (tokens.MoveNext())
            {
                var currentTokens = tokens;
				char parsedChar;
                var result = IsCharacter(ref currentTokens, out parsedChar);

                if (result)
                {
                    newTokenList.Add(Tokenizer.ToToken(Convert.ToInt16(parsedChar).ToString()));
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
