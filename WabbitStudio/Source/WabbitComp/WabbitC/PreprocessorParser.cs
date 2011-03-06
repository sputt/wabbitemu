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
				if (token.Type == TokenType.Preprocessor)
				{
					switch (token.Text)
					{
						case "#if":
						case "#ifdef":
						case "#ifndef":
						case "#elsif":
							HandleIf(ref i);
							break;
						case "#undefine":
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
				}
				else
				{
					parsedContent.Add(token);
				}
			}

			return parsedContent;
		}

		private void HandleRegion(ref int i)
		{

		}

		private List<Token> GetPreprocessorLine(ref int i)
		{
			int currentLine = fileContents[i].LineNumber;
			List<Token> line = new List<Token>();
			while (fileContents[++i].LineNumber == currentLine)
				line.Add(fileContents[i]);
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
			}
			else
			{
				//#include ""
				sb.Append(fileContents[i].Text.Replace("\"", ""));
				includeFile = ResolveInclude(sb.ToString(), IncludeDirs.LocalIncludes);
			}
		}

		private string ResolveInclude(string partialPath, List<string> includeDirs)
		{
			return null;
		}

		private void HandleDefine(ref int i)
		{
			throw new NotImplementedException();
		}

		private void HandleUndefine(ref int i)
		{
			throw new NotImplementedException();
		}

		private void HandleIf(ref int i)
		{
			List<Token> line = GetPreprocessorLine(ref i);
			Expression exp = new Expression(line);
			exp.Eval();
		}
	}
}
