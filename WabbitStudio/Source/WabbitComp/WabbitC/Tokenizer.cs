using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace WabbitC
{
    public enum TokenType
    {
        Preprocessor,
        ReservedKeyword,
        StringType,
        IntType,
        RealType,
        CommentType,
		OperatorType,
        OpenBlock,
        CloseBlock,
        OpenParen,
        CloseParen,
        StatementEnd,
		UndefinedType,
		ArgSeparator,
		TernaryConditional,
    };

    public static class Tokenizer
    {
        const string SingleLineComment = "//";
        const string MultiLineCommentStart = "/*";
        const string MultiLineCommentEnd = "*/";
        const string DoubleQuote = "\"";
        static List<string> Preprocessor = new List<string>() { "#include", "#define", "#undef", "#error", "#warning", "#if", "#ifndef", 
                                                           "#ifdef", "#elif", "#else", "#endif", "#region", "#endregion" };
        static List<string> ReservedKeywords = new List<string>() { "auto", "double", "int", "struct", "break", "else", "long", "switch",
                                              "case", "enum", "register", "typedef", "char", "extern", "return", "union",
                                              "const", "float", "short", "unsigned", "continue", "for", "signed", "void",
                                              "default", "goto", "sizeof", "volatile", "do", "if", "static", "while" };
        //string inputContents = null;
        static int line = 0;

        //public List<Token> Tokens { get; private set; }
        public static List<Token> Tokenize(string infile)
        {
			var tokens = new List<Token>();
			int index = 0;
			int length = infile.Length;
			while (index < length)
			{
				tokens.Add(ReadToken(ref index, infile));
				SkipWhitespace(ref index, infile);
			}
			return tokens;
        }

        public static Token ToToken(string input)
        {
            var tokens = Tokenize(input);
            return tokens[0];
        }

        public static List<Token> GetStatement(ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Token>();
            while (tokens.Current.Type != TokenType.StatementEnd)
            {
                tokenList.Add(tokens.Current);
                tokens.MoveNext();
            }

            return tokenList;
        }

        public static List<Token> GetArgument(ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Token>();
            int nParenBalance = 0;
            while (!((nParenBalance == 0 && tokens.Current.Text == ",") || (nParenBalance == 0 && tokens.Current.Text == ")")))
            {
                if (tokens.Current.Type == TokenType.OpenParen)
                {
                    nParenBalance++;
                }
                else if (tokens.Current.Type == TokenType.CloseParen)
                {
                    nParenBalance--;
                }
                tokenList.Add(tokens.Current);
                tokens.MoveNext();
            }

            return tokenList;
        }

        private static Token ReadToken(ref int index, string inputContents)
        {
            Token tokenToAdd = new Token();
            int oldIndex = index;
            tokenToAdd.LineNumber = line;
            tokenToAdd.CharNumber = index;
            tokenToAdd.Text = ReadWord(ref index, inputContents);
            GetTokenType(ref tokenToAdd);
            return tokenToAdd;
        }

        private static void GetTokenType(ref Token tokenToAdd)
        {
            float floatCheck;
            int intCheck;
			if (CheckReservedKeyword(tokenToAdd.Text))
				tokenToAdd.Type = TokenType.ReservedKeyword;
			else if (CheckPreprocessorKeyword(tokenToAdd.Text))
				tokenToAdd.Type = TokenType.Preprocessor;
			else if (CheckComment(tokenToAdd.Text))
				tokenToAdd.Type = TokenType.CommentType;
			else if (float.TryParse(tokenToAdd.Text, out floatCheck) && (tokenToAdd.Text.Contains("E") || tokenToAdd.Text.Contains("e") || tokenToAdd.Text.Contains(".")))
				tokenToAdd.Type = TokenType.RealType;
			else if (int.TryParse(tokenToAdd.Text, out intCheck))
				tokenToAdd.Type = TokenType.IntType;
			else if (CheckOperators(tokenToAdd.Text))
				tokenToAdd.Type = TokenType.OperatorType;
			else if (tokenToAdd.Text == "{")
				tokenToAdd.Type = TokenType.OpenBlock;
			else if (tokenToAdd.Text == "}")
				tokenToAdd.Type = TokenType.CloseBlock;
			else if (tokenToAdd.Text == "(")
				tokenToAdd.Type = TokenType.OpenParen;
			else if (tokenToAdd.Text == ")")
				tokenToAdd.Type = TokenType.CloseParen;
			else if (tokenToAdd.Text == ";")
				tokenToAdd.Type = TokenType.StatementEnd;
			else
				tokenToAdd.Type = TokenType.StringType;

        }

		private static bool CheckOperators(string text)
		{
			if (operators.Contains(text))
				return true;
			else
				return false;
		}

        private static bool CheckComment(string text)
        {
            return text.StartsWith(MultiLineCommentStart) || text.StartsWith(SingleLineComment);
        }

        private static bool CheckPreprocessorKeyword(string text)
        {
            return Preprocessor.Contains(text);
        }

        private static bool CheckReservedKeyword(string text)
        {
            return ReservedKeywords.Contains(text);
        }

		const string delimeters = "&<−->~!%^*()+=|\\/{}[]:;\"' \n\t\r?,.";
		const string operators = "&&<<=-->>=?!=~%^=*=−−==-=++=||/=,";
        private static string ReadWord(ref int index, string inputContents)
        {
            int newIndex = index;
            char test = inputContents[index];
            if (delimeters.IndexOf(test) >= 0)
            {
                newIndex++;
            }
            else
            {
                while (index > 0 && delimeters.IndexOf(test) == -1)
                    test = inputContents[--index];
                if (index != newIndex)
                    index++;
                test = inputContents[newIndex];
                while (newIndex + 1 < inputContents.Length && delimeters.IndexOf(test) == -1)
                    test = inputContents[++newIndex];
				if (newIndex + 1 >= inputContents.Length && delimeters.IndexOf(inputContents[newIndex]) == -1)
					newIndex++;
                if (newIndex < index)
                    return null;
            }
            string word = inputContents.Substring(index, newIndex - index);
            if (word.Length > 0 && delimeters.IndexOf(word[0]) >= 0)
            {
                if (word == DoubleQuote)
                {
                    test = inputContents[newIndex + 1];
                    while (newIndex < inputContents.Length && !(test == '\"' && inputContents[newIndex - 1] != '\\'))
                        test = inputContents[++newIndex];
                    newIndex++;
                }
                else if (word == "/")
                {
                    //check for comments
					string comment = word + inputContents[newIndex];
                    if (comment == MultiLineCommentStart)
                    {
                        while (newIndex < inputContents.Length && !(test == '*' && inputContents[newIndex + 1] == '/'))
                        {
                            test = inputContents[++newIndex];
                            if (test == '\n')
                                line++;
                        }
                        newIndex += 2;
						word = comment;
                    }
                    else if (comment == SingleLineComment)
                    {
                        while (newIndex < inputContents.Length && test != '\n')
                            test = inputContents[++newIndex];
						word = comment;
                    }
                }
                else
                {
                    if (IsValidIndex(newIndex, inputContents))
                    {
                        test = word[0];
						char nextCh =  inputContents[newIndex];
                        if (operators.IndexOf(test) >= 0)
							if (((test == '+' || test == '-' || test == '|' || test == '=' || test == '&') && nextCh == test) || nextCh == '=' || (test == '-' && nextCh == '>'))
								newIndex++;
                    }
                }
                word = inputContents.Substring(index, newIndex - index);
            }
            
            index = newIndex;
            return word;
        }

        private static void SkipWhitespace(ref int index, string inputContents)
        {
            while (IsValidIndex(index, inputContents) && char.IsWhiteSpace(inputContents[index]))
            {
                if (inputContents[index] == '\n')
                    line++;
                index++;
            }
        }

        private static bool IsValidIndex(int index, string inputContents)
        {
            return index < inputContents.Length;
        }

    }

    public class Token
	{
		#region Constant Tokens
		public static Token OpenParenToken = new Token() { Type = TokenType.OpenParen, Text = "(" };
		public static Token CloseParenToken = new Token() { Type = TokenType.CloseParen, Text = ")" };
		public static Token OpenBraceToken = new Token() { Type = TokenType.OpenBlock, Text = "{" };
		public static Token CloseBraceToken = new Token() { Type = TokenType.CloseBlock, Text = "}" };
		public static Token AssignmentOperatorToken = new Token() { Text = "=", Type = TokenType.OperatorType };
		public static Token StatementEndToken = new Token() { Text = ";", Type = TokenType.StatementEnd };
		
		public static Token BitNOTOperatorToken = new Token() { Text = "~", Type = TokenType.OperatorType };
		public static Token BitXOROperatorToken = new Token() { Text = "^", Type = TokenType.OperatorType };
		public static Token BitANDOperatorToken = new Token() { Text = "&", Type = TokenType.OperatorType };
		public static Token BitOROperatorToken = new Token() { Text = "|", Type = TokenType.OperatorType };

		public static Token NOTOperatorToken = new Token() { Text = "!", Type = TokenType.OperatorType };
		public static Token ANDOperatorToken = new Token() { Text = "!", Type = TokenType.OperatorType };
		public static Token OROperatorToken = new Token() { Text = "!", Type = TokenType.OperatorType };
		
		public static Token AddOperatorToken = new Token() { Text = "+", Type = TokenType.OperatorType };
		public static Token SubOperatorToken = new Token() { Text = "-", Type = TokenType.OperatorType };
		public static Token MultOperatorToken = new Token() { Text = "*", Type = TokenType.OperatorType };
		public static Token DivOperatorToken = new Token() { Text = "/", Type = TokenType.OperatorType };
		#endregion

		public TokenType Type { get; set; }
        public string Text { get; set; }
        public int LineNumber { get; set; }
        public int CharNumber { get; set; }
        
        public Token()
        {
            
        }

        public static implicit operator string(Token t)
        {
			if (t == null)
				return null;
            return t.Text;
        }

        public override string ToString()
        {
			if (Type == TokenType.UndefinedType)
				return "undefined";
            return Text;
        }

		public override bool Equals(object obj)
		{
			if (obj.GetType() != typeof(Token))
				return base.Equals(obj);
			Token tok = (Token)obj;
			return (Text == tok.Text) && (Type == tok.Type);
		}

		#region Operator Overloads
		public static bool operator ==(Token t1, Token t2)
        {
            if (object.Equals(t1, t2))
            {
                return true;
            }
            if (object.Equals(t1, null) || object.Equals(t2, null))
            {
                return false;
            }
            return t1.Equals(t2);
        }

        public static bool operator !=(Token t1, Token t2)
        {
            return !(t1 == t2);
        }

		public static Expression operator +(Token t1, Token t2)
		{
			var result = new Token();
			if (t2.Type == TokenType.UndefinedType || t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(result);
			}
			if (t2.Type == TokenType.RealType || t1.Type == TokenType.RealType)
			{
				var double1 = double.Parse(t1.Text);
				var double2 = double.Parse(t2.Text);
				result.Text = (double1 + double2).ToString();
				result.Type = TokenType.RealType;
				return new Expression(result);
			}
			else
			{
				int int1, int2;
				result.Type = TokenType.IntType;
				if (!int.TryParse(t1.Text, out int1))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(AddOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				if (!int.TryParse(t2.Text, out int2))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(AddOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				result.Text = (int1 + int2).ToString();
				result.Type = TokenType.IntType;
			}
			return new Expression(result);
		}

		public static Expression operator -(Token t1, Token t2)
		{
			var result = new Token();
			if (t2.Type == TokenType.UndefinedType || t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(result);
			}
			if (t2.Type == TokenType.RealType || t1.Type == TokenType.RealType)
			{
				var double1 = double.Parse(t1.Text);
				var double2 = double.Parse(t2.Text);
				result.Text = (double1 - double2).ToString();
				result.Type = TokenType.RealType;
				return new Expression(result);
			}
			else
			{
				int int1, int2;
				result.Type = TokenType.IntType;
				if (!int.TryParse(t1.Text, out int1))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(SubOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				if (!int.TryParse(t2.Text, out int2))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(SubOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				result.Text = (int1 - int2).ToString();
				result.Type = TokenType.IntType;
			}
			return new Expression(result);
		}

		public static Expression operator *(Token t1, Token t2)
		{
			var result = new Token();			
			if (t2.Type == TokenType.UndefinedType || t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(result);
			}
			if (t2.Type == TokenType.RealType || t1.Type == TokenType.RealType)
			{
				var double1 = double.Parse(t1.Text);
				var double2 = double.Parse(t2.Text);
				result.Text = (double1 * double2).ToString();
				result.Type = TokenType.RealType;
				return new Expression(result);
			}
			else
			{
				int int1, int2;
				result.Type = TokenType.IntType;
				if (!int.TryParse(t1.Text, out int1))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(MultOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				if (!int.TryParse(t2.Text, out int2))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(MultOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				result.Text = (int1 * int2).ToString();
				result.Type = TokenType.IntType;
			}
			return new Expression(result);
		}

		public static Expression operator /(Token t1, Token t2)
		{
			var result = new Token();
			
			if (t2.Type == TokenType.UndefinedType || t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
			}
			if (t2.Type == TokenType.RealType || t1.Type == TokenType.RealType)
			{
				var double1 = double.Parse(t1.Text);
				var double2 = double.Parse(t2.Text);
				result.Text = (double1 / double2).ToString();
				result.Type = TokenType.RealType;
			}
			else
			{
				int int1, int2;
				result.Type = TokenType.IntType;
				if (!int.TryParse(t1.Text, out int1))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(DivOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				if (!int.TryParse(t2.Text, out int2))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(DivOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				result.Text = (int1 / int2).ToString();
				result.Type = TokenType.IntType;
			}
			return new Expression(result);
		}

		public static Expression operator ^(Token t1, Token t2)
		{
			var result = new Token();
			if (t2.Type == TokenType.UndefinedType || t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(result);
			}
			if (t2.Type == TokenType.RealType || t1.Type == TokenType.RealType)
			{
				throw new Exception("Unable to apply ^ to type RealType");
			}
			else
			{
				int int1, int2;
				result.Type = TokenType.IntType;
				if (!int.TryParse(t1.Text, out int1))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(BitXOROperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				if (!int.TryParse(t2.Text, out int2))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(BitXOROperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				result.Text = (int1 ^ int2).ToString();
				result.Type = TokenType.IntType;
			}
			return new Expression(result);
		}

		public static Expression operator &(Token t1, Token t2)
		{
			var result = new Token();
			if (t2.Type == TokenType.UndefinedType || t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(result);
			}
			if (t2.Type == TokenType.RealType || t1.Type == TokenType.RealType)
			{
				throw new Exception("Unable to apply & to type RealType");
			}
			else
			{
				int int1, int2;
				result.Type = TokenType.IntType;
				if (!int.TryParse(t1.Text, out int1))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(BitANDOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				if (!int.TryParse(t2.Text, out int2))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(BitANDOperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				result.Text = (int1 & int2).ToString();
				result.Type = TokenType.IntType;
			}
			return new Expression(result);
		}

		public static Expression operator |(Token t1, Token t2)
		{
			var result = new Token();
			if (t2.Type == TokenType.UndefinedType || t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(result);
			}
			if (t2.Type == TokenType.RealType || t1.Type == TokenType.RealType)
			{
				throw new Exception("Unable to apply | to type RealType");
			}
			else
			{
				int int1, int2;
				result.Type = TokenType.IntType;
				if (!int.TryParse(t1.Text, out int1))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(BitOROperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				if (!int.TryParse(t2.Text, out int2))
				{
					return null;
					var resultList = new List<Token>();
					resultList.Add(BitOROperatorToken);
					resultList.Add(t2);
					resultList.Add(t1);
					return new Expression(resultList);
				}
				result.Text = (int1 | int2).ToString();
				result.Type = TokenType.IntType;
			}
			return new Expression(result);
		}

		public static Expression operator !(Token t1)
		{
			var result = new Token();
			var resultList = new List<Token>();
			if (t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(resultList);
			}
			if (t1.Type == TokenType.RealType)
			{
				throw new Exception("Unable to apply ^ to type RealType");
			}
			int int1;
			result.Type = TokenType.IntType;
			if (!int.TryParse(t1.Text, out int1))
			{
				return null;
				resultList.Add(NOTOperatorToken);
				resultList.Add(t1);
				return new Expression(resultList);
			}
			result.Text = (Convert.ToInt16(!Convert.ToBoolean(int1))).ToString();
			result.Type = TokenType.IntType;
			return new Expression(resultList);
		}

		public static Expression operator ~(Token t1)
		{
			var result = new Token();
			var resultList = new List<Token>();
			if (t1.Type == TokenType.UndefinedType)
			{
				result.Type = TokenType.UndefinedType;
				return new Expression(resultList);
			}
			if (t1.Type == TokenType.RealType)
			{
				throw new Exception("Unable to apply ^ to type RealType");
			}
			int int1;
			result.Type = TokenType.IntType;
			if (!int.TryParse(t1.Text, out int1))
			{
				return null;
				resultList.Add(BitNOTOperatorToken);
				resultList.Add(t1);
				return new Expression(resultList);
			}
			result.Text = (~int1).ToString();
			result.Type = TokenType.IntType;
			return new Expression(resultList);
		}

        public static Expression OpEquals(Token t1, Token t2)
        {
            var resultList = new List<Token>();
			resultList.Add(AssignmentOperatorToken);
            resultList.Add(t2);
            resultList.Add(t1);
            return new Expression(resultList);
        }

		#endregion
	}
}
