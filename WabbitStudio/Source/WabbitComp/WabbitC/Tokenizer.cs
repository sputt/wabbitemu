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
    };

    public class Tokenizer
    {
        const string SingleLineComment = "//";
        const string MultiLineCommentStart = "/*";
        const string MultiLineCommentEnd = "*/";
        const string DoubleQuote = "\"";
        List<string> Preprocessor = new List<string>() { "#include", "#define", "#error", "#warning", "#if", "#ifdef",
                                                            "#elsif", "#else", "#endif", "#region" };
        List<string> ReservedKeywords = new List<string>() { "auto", "double", "int", "struct", "break", "else", "long", "switch",
                                              "case", "enum", "register", "typedef", "char", "extern", "return", "union",
                                              "const", "float", "short", "unsigned", "continue", "for", "signed", "void",
                                              "default", "goto", "sizeof", "volatile", "do", "if", "static", "while" };
        string inputContents = null;
        int line = 0;

        public List<Token> Tokens { get; private set; }
        public void Tokenize(string infile)
        {
            inputContents = infile;
            Tokens = Tokenize();
        }

        private List<Token> Tokenize()
        {
            List<Token> tokens = new List<Token>();
            int index = 0;
            int length = inputContents.Length;
            while (index < length)
            {
                tokens.Add(ReadToken(ref index));
                SkipWhitespace(ref index);
            }
            return tokens;
        }

        private Token ReadToken(ref int index)
        {
            Token tokenToAdd = new Token();
            int oldIndex = index;
            tokenToAdd.LineNumber = line;
            tokenToAdd.CharNumber = index;
            tokenToAdd.TokenText = ReadWord(ref index);
            GetTokenType(ref tokenToAdd);
            return tokenToAdd;
        }

        private void GetTokenType(ref Token tokenToAdd)
        {
            float floatCheck;
            int intCheck;
            if (CheckReservedKeyword(tokenToAdd.TokenText))
                tokenToAdd.TokenType = TokenType.ReservedKeyword;
            else if (CheckPreprocessorKeyword(tokenToAdd.TokenText))
                tokenToAdd.TokenType = TokenType.Preprocessor;
            else if (CheckComment(tokenToAdd.TokenText))
                tokenToAdd.TokenType = TokenType.CommentType;
            else if (float.TryParse(tokenToAdd.TokenText, out floatCheck) && (tokenToAdd.TokenText.Contains("E") || tokenToAdd.TokenText.Contains(".")))
                tokenToAdd.TokenType = TokenType.RealType;
            else if (int.TryParse(tokenToAdd.TokenText, out intCheck))
                tokenToAdd.TokenType = TokenType.IntType;
            else if (CheckOperators(tokenToAdd.TokenText))
                tokenToAdd.TokenType = TokenType.OperatorType;
            else if (tokenToAdd.TokenText == "{")
                tokenToAdd.TokenType = TokenType.OpenBlock;
            else if (tokenToAdd.TokenText == "}")
                tokenToAdd.TokenType = TokenType.CloseBlock;
            else if (tokenToAdd.TokenText == "(")
                tokenToAdd.TokenType = TokenType.OpenParen;
            else if (tokenToAdd.TokenText == ")")
                tokenToAdd.TokenType = TokenType.CloseParen;
            else if (tokenToAdd.TokenText == ";")
                tokenToAdd.TokenType = TokenType.StatementEnd;
            else
                tokenToAdd.TokenType = TokenType.StringType;

        }

		private bool CheckOperators(string text)
		{
			if (operators.Contains(text))
				return true;
			else
				return false;
		}

        private bool CheckComment(string text)
        {
            return text.StartsWith(MultiLineCommentStart) || text.StartsWith(SingleLineComment);
        }

        private bool CheckPreprocessorKeyword(string text)
        {
            return Preprocessor.Contains(text);
        }

        private bool CheckReservedKeyword(string text)
        {
            return ReservedKeywords.Contains(text);
        }

		const string delimeters = "&<>~!%^*()−-+=|\\/{}[]:;\"' \n\t\r?,";
		const string operators = "&&<<>>!~%^=*=−−=--=++=||/=";
        private string ReadWord(ref int index)
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
				if (newIndex + 1 >= inputContents.Length)
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
                    if (IsValidIndex(newIndex))
                    {
                        test = word[0];
						char nextCh =  inputContents[newIndex];
                        if (operators.IndexOf(test) >= 0)
							if (((test == '+' || test == '-' || test == '|' || test == '=' || test == '&') && nextCh == test) || nextCh == '=')
								newIndex++;
                    }
                }
                word = inputContents.Substring(index, newIndex - index);
            }
            
            index = newIndex;
            return word;
        }

        private void SkipWhitespace(ref int index)
        {
            while (IsValidIndex(index) && char.IsWhiteSpace(inputContents[index]))
            {
                if (inputContents[index] == '\n')
                    line++;
                index++;
            }
        }

        private bool IsValidIndex(int index)
        {
            return index < inputContents.Length;
        }

    }

    public class Token
    {
        public TokenType TokenType { get; set; }
        public string TokenText { get; set; }
        public int LineNumber { get; set; }
        public int CharNumber { get; set; }
        
        public Token()
        {
            
        }

        public override string ToString()
        {
            return TokenText;
        }

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
            return t1.TokenType == t2.TokenType && t1.TokenText == t2.TokenText;
        }

        public static bool operator !=(Token t1, Token t2)
        {
            if (object.Equals(t1, t2))
            {
                return false;
            }
            if (object.Equals(t1, null) || object.Equals(t2, null))
            {
                return true;
            }
            return t1.TokenType != t2.TokenType || t1.TokenText != t2.TokenText;
        }
    }


}
