using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace WabbitC
{
	public class PreprocessorParser
	{
		readonly List<Token> fileContents;
		List<Token> parsedContent;
		static List<PreprocessorDefine> defines = new List<PreprocessorDefine>();
		//List<PreprocessorDefine> Defines { get { return defines; } }

		public static PreprocessorDefine DefineValue(Token token)
		{
			foreach (PreprocessorDefine define in defines)
			{
				if (define.Name == token)
					return define;
			}
			return null;
		}

		public PreprocessorParser(List<Token> fileContents)
		{
			this.fileContents = fileContents;
		}

		public List<Token> Parse()
		{
			parsedContent = new List<Token>();
			for (int i = 0; i < fileContents.Count; i++)
			{
				Token token = fileContents[i];
				switch (token.Type)
				{
					case TokenType.Preprocessor:
						switch (token.Text)
						{
							case "#ifdef":
								HandleIfDef(ref i, true);
								break;
							case "#ifndef":
								HandleIfDef(ref i, false);
								break;
							case "#if":
							case "#elsif":
								HandleIf(ref i);
								break;
							case "#undef":
								HandleUndefine(ref i);
								break;
							case "#define":
								HandleDefine(ref i);
								break;
							case "#include":
								HandleInclude(ref i);
								break;
							case "#region":
								HandleRegion(ref i);
								break;
							case "#error":
								HandleErrorMessage(ref i);
								break;
							case "#warning":
								HandleWarningMessage(ref i);
								break;
						}
						break;
					case TokenType.CommentType:
						break;
					default:
						parsedContent.Add(token);
						break;

				}
			}

			return parsedContent;
		}

		private void HandleRegion(ref int i)
		{

		}

		private List<Token> GetPreprocessorLine(ref int i)
		{
			int currentLine;
			List<Token> line = new List<Token>();
			do
			{
				currentLine = fileContents[i].LineNumber;
				while (++i < fileContents.Count && fileContents[i].LineNumber == currentLine)
					if (fileContents[i].Type != TokenType.CommentType)
						line.Add(fileContents[i]);
			} while (i < fileContents.Count && fileContents[i].Text == "\\");
			i--;
			return line;
		}

		private void HandleWarningMessage(ref int i)
		{
			List<Token> line = GetPreprocessorLine(ref i);
			string message = null;
			MessageSystem.Instance.ThrowNewWarning(message);
		}

		private void HandleErrorMessage(ref int i)
		{
			List<Token> line = GetPreprocessorLine(ref i);
			string message = null;
			MessageSystem.Instance.ThrowNewError(message);
		}

		private void HandleInclude(ref int i)
		{
			StringBuilder sb = new StringBuilder();
			string includeFile;
			if (fileContents[++i].Text == "<")
			{
				//#include <>
				i++;		//skip first <
				while (fileContents[i].Text != ">")
					sb.Append(fileContents[i++].Text);
				includeFile = ResolveInclude(sb.ToString(), IncludeDirs.SystemIncludes);
			}
			else
			{
				//#include ""
				sb.Append(fileContents[i].Text.Replace("\"", String.Empty));
				includeFile = ResolveInclude(sb.ToString(), IncludeDirs.LocalIncludes);
			}
			if (!string.IsNullOrEmpty(includeFile))
			{
				string includeFileContents = Compiler.TryOpenFile(includeFile);
				if (!string.IsNullOrEmpty(includeFileContents))
				{
					PreprocessorParser newParser = new PreprocessorParser(Tokenizer.Tokenize(includeFileContents));
					List<Token> newPreprocess = newParser.Parse();
					parsedContent.AddRange(newPreprocess);
				}
			}
		}

		private string ResolveInclude(string partialPath, List<string> includeDirs)
		{
			foreach (string includeDir in includeDirs)
			{
				string file = Path.Combine(includeDir, partialPath);
				if (File.Exists(file))
					return file;
			}
			return null;
		}

		private void HandleDefine(ref int i)
		{
			List<Token> line = GetPreprocessorLine(ref i);
			if (line.Count < 2)
			{
				MessageSystem.Instance.ThrowNewError("Not enough arguments for #define");
				return;
			}
			List<Token> tokensAfter = new List<Token>();
				for (int j = 1; j < line.Count; j++)
					tokensAfter.Add(line[j]);
				if (line[1].Type == TokenType.OpenParen)
			{
				//its a macro
				int j, paren;
				List<Token> args = new List<Token>();
				for (j = 1, paren = 0; j < tokensAfter.Count; j++)
				{
					Token curToken = tokensAfter[j];
					if (curToken.Type == TokenType.OpenParen)
						paren++;
					if (curToken.Type == TokenType.CloseParen)
					{
						if (paren == 0)
						{
							j++;
							break;
						}
						else
						{
							paren--;
						}
					}
					if (curToken.Text != ",")
						args.Add(curToken);
				}
				List<Token> values = new List<Token>();
				for (; j < tokensAfter.Count; j++)
				{
					Token curToken = tokensAfter[j];
					values.Add(curToken);
				}
				if (values.Count == 0)
				{
					MessageSystem.Instance.ThrowNewError("#define has no value to insert");
					return;
				}
				MacroDefine macro = new MacroDefine(line[0], args, values);
				defines.Add(macro);
			}
			else
			{
				//its a replacement
				Expression expression = new Expression(tokensAfter);
				//var replacement = new ReplacementDefine(line[0], expression.Eval()[0]);
				//defines.Add(replacement);
			}
		}

		private void HandleUndefine(ref int i)
		{
			PreprocessorDefine define = DefineValue(fileContents[++i]);
			if (define != null)
				defines.Remove(define);
		}

		private void HandleIf(ref int i)
		{
			List<Token> line = GetPreprocessorLine(ref i);
			Expression exp = new Expression(line);
			exp.Eval();
		}

		private void HandleIfDef(ref int i, bool IsPositive)
		{
			List<Token> line = GetPreprocessorLine(ref i);
			Expression exp = new Expression(line);
			var value = new Token();// exp.Eval()[0];
			int nesting = 0;
			List<Token> conditionalTokens = new List<Token>();
			i++;
			if ((value.Type != TokenType.UndefinedType && IsPositive) || (value.Type != TokenType.UndefinedType && !IsPositive))
			{
				for (; i < fileContents.Count; i++)
				{
					if (fileContents[i].Text == "#if" || fileContents[i].Text == "#ifdef")
						nesting++;
					if (fileContents[i].Text == "#elif" || fileContents[i].Text == "#endif" || fileContents[i].Text == "#else")
					{
						if (nesting > 0)
						{
							if (fileContents[i].Text == "#endif")
							{
								nesting--;
							}
						}
						else
						{
							break;
						}
					}
					conditionalTokens.Add(fileContents[i]);
				}
				i++;
				PreprocessorParser parser = new PreprocessorParser(conditionalTokens);
				List<Token> temp = parser.Parse();
				parsedContent.AddRange(temp);
			}
			else
			{
				bool inCorrectRange = false;
				for (; i < fileContents.Count; i++)
				{
					Token token = fileContents[i];
					if (token.Text == "#if" || token.Text == "#ifdef")
						nesting++;
					if (token.Text == "#endif")
					{
						if (nesting > 0)
							nesting--;
						else
							break;
					}
					if (token.Text == "#elif" || token.Text == "#else")
					{
						if (nesting == 0)
						{
							if (token.Text == "#else")
							{
								inCorrectRange = true;
							}
							else
							{

							}
						}
					}
					if (inCorrectRange)
						conditionalTokens.Add(fileContents[i]);
				}
				i++;
				PreprocessorParser parser = new PreprocessorParser(conditionalTokens);
				List<Token> temp = parser.Parse();
				parsedContent.AddRange(temp);
			}
		}
	}
}
