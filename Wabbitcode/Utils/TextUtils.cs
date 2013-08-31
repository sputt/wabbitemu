using System;

namespace Revsoft.Wabbitcode.Utils
{
	public static class TextUtils
	{
		public const string DefaultDelimiters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";

		public static bool IsValidIndex(int counter, int length)
		{
			return counter > -1 && counter < length;
		}

		public static int SkipToEndOfCodeLine(string substring, int counter, char endOfLineChar, char commentChar)
		{
			int length = substring.Length;
			while (IsValidIndex(counter, length) && substring[counter] != '\n' && substring[counter] != '\r' &&
				   substring[counter] != commentChar && substring[counter] != endOfLineChar)
			{
				if (substring[counter] == '\"')
				{
					counter++;
					while (IsValidIndex(counter, length) && substring[counter] != '\"')
					{
						counter++;
					}
				}
				counter++;
			}

			while (IsValidIndex(counter, length) && (substring[counter] == endOfLineChar ||
					substring[counter] == '\n' || substring[counter] == '\r'))
			{
				counter++;
			}

			return !IsValidIndex(counter, length) ? -1 : counter;
		}

		public static int SkipToEndOfLine(string substring, int counter)
		{
			int length = substring.Length;
			while (IsValidIndex(counter, length) && substring[counter] != '\n' && substring[counter] != '\r')
			{
				counter++;
			}

			if (IsValidIndex(counter, length) && (substring[counter] == '\n' || substring[counter] == '\r'))
			{
				counter++;
			}

			return !IsValidIndex(counter, length) ? -1 : counter;
		}

		public static int SkipWhitespace(string substring, int counter)
		{
			int length = substring.Length;
			while (IsValidIndex(counter, length) &&
				   (substring[counter] != '\r' && substring[counter] != '\n')
				   && char.IsWhiteSpace(substring[counter]))
			{
				counter++;
			}
			if (!IsValidIndex(counter, length))
			{
				return -1;
			}
			return counter;
		}

		public static string GetWord(string text, int offset, string delimiters = null)
		{
			delimiters = delimiters ?? DefaultDelimiters;
			if (offset >= text.Length)
			{
				return String.Empty;
			}
			int newOffset = offset;
			char test = text[offset];
			while (offset > 0 && delimiters.IndexOf(test) == -1)
			{
				test = text[--offset];
			}

			if (offset > 0)
			{
				offset++;
			}

			test = text[newOffset];
			while (newOffset + 1 < text.Length && delimiters.IndexOf(test) == -1)
			{
				test = text[++newOffset];
			}

			return newOffset < offset ? string.Empty : text.Substring(offset, newOffset - offset);
		}

		public static int SkipToBeginningOfLine(string lines, int counter)
		{
			while (IsValidIndex(counter, lines.Length) && lines[counter] != '\n')
			{
				counter--;
			}
			// move past the \n
			counter++;

			return counter;
		}

		public static bool IsEndOfCodeLineChar(char ch)
		{
			return (ch == '\0' || ch == '\n' || ch == '\r' || ch == ';' || ch == '\\');
		}

		public static bool IsValidLabelChar(char c)
		{
			return char.IsLetterOrDigit(c) || c == '_';
		}

		public static bool IsValidDefineChar(char c)
		{
			return char.IsLetterOrDigit(c) || c == '_' || c == '.';
		}
	}
}
