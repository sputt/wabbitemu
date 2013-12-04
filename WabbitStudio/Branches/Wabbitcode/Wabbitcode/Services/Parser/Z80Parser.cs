using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services.Parser
{
    public class Z80Parser : IParser
    {
        #region Constants

        private const char CommentChar = ';';
        private const string CommentString = "comment";
        private const string DefineString = "define";
        private const string EndCommentString = "endcomment";
        private const string EndMacroString = "endmacro";
        private const char EndOfLineChar = '\\';
        private const string IncludeString = "include";
        private const string MacroString = "macro";
        private const string Delimeters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";

        #endregion

        private string[] _lines;
        private int _lineIndex;
        private IEnumerator<string> _tokensEnumerator;
        private ParserInformation _parserInfo;

        public event ParserProgressHandler OnParserProgress;

        private static IEnumerable<string> TokenizeLine(string line)
        {
            Regex re = new Regex("([&<>~!%#\\^\\*\\(\\)-\\+=\\|\\\\/\\{}\\[\\]:;\"'\\ \\n\\t\\r\\?,])", RegexOptions.Compiled);
            var tokenList = re.Split(line).Where(t => !String.IsNullOrEmpty(t));
            return tokenList;
        }

        public void ParseText(ParserInformation parserInfo, string fileText)
        {
            var tokens = TokenizeLine(fileText);
            _tokensEnumerator = tokens.GetEnumerator();
            _lines = fileText.Split('\n');
            _parserInfo = parserInfo;
            int percent = 0;
            for (_lineIndex = 0; _lineIndex < _lines.Length; _lineIndex++)
            {
                int newPercent = _lineIndex * 100 / _lines.Length;

                if (percent + 5 <= newPercent)
                {
                    percent = newPercent;

                    if (OnParserProgress != null)
                    {
                        OnParserProgress(this, new ParserProgressEventArgs(parserInfo.SourceFile, percent));
                    }
                }

                bool hasNext;
                do
                {
                    hasNext = _tokensEnumerator.MoveNext();
                    if (!hasNext)
                    {
                        break;
                    }

                    char firstChar = _tokensEnumerator.Current.First();
                    if (firstChar == CommentChar)
                    {
                        SkipToEndOfLine();
                        break;
                    }
                    // handle label other xx = 22 type define
                    if (IsValidLabelChar(firstChar))
                    {
                        HandleLabelOrDefine();
                    }
                    else
                    {
                        if (char.IsWhiteSpace(firstChar))
                        {
                            hasNext = SkipWhitespace();
                            if (!hasNext)
                            {
                                break;
                            }
                        }

                        if (_tokensEnumerator.Current == "#")
                        {
                            if (_tokensEnumerator.MoveNext()) 
                            { 
                                switch (_tokensEnumerator.Current.ToLower())
                                {
                                    case IncludeString:
                                        HandleInclude();
                                        break;
                                    case DefineString:
                                        HandleDefine();
                                        break;
                                    case CommentString:
                                        _lineIndex = HandleBlockComment();
                                        break;
                                    case MacroString:
                                        _lineIndex = HandleMacro();
                                        break;
                                }
                            }
                        }
                    }

                    hasNext = SkipToEndOfCodeLine();
                    if (_tokensEnumerator.Current == CommentChar.ToString())
                    {
                        SkipToEndOfLine();
                        break;
                    }
                } while (hasNext && IsEndOfCodeLine() && !IsEndOfLine());
            }
        }

        public List<Reference> FindReferences(string fileName, string fileText, string textToFind, bool caseSensitive)
        {
            List<Reference> refs = new List<Reference>();
            var options = caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
            int len = textToFind.Length;

            string[] lines = fileText.Split('\n');
            for (int i = 0; i < lines.Length; i++)
            {
                string line = lines[i];
                string originalLine = line;
                int commentIndex = line.IndexOf(';');
                if (commentIndex != -1)
                {
                    line = line.Remove(commentIndex);
                }

                if (line.Trim().StartsWith("#" + CommentString, options))
                {
                    while (!line.Trim().StartsWith("#" + EndCommentString, options))
                    {
                        line = lines[++i];
                    }

                    continue;
                }

                int refactorIndex = line.IndexOf(textToFind, options);
                if ((refactorIndex == -1) || (refactorIndex != 0 && !Delimeters.Contains(line[refactorIndex - 1]))
                    || (refactorIndex + len < line.Length && !Delimeters.Contains(line[refactorIndex + len])))
                {
                    continue;
                }

                List<int> quotes = new List<int>();
                int quoteIndex = 0;
                while (line.IndexOf('\"', quoteIndex) != -1)
                {
                    quoteIndex = line.IndexOf('\"', quoteIndex);
                    quotes.Add(quoteIndex++);
                }

                bool inQuote = false;
                for (int j = 0; j < quotes.Count; j++)
                {
                    if ((refactorIndex <= quotes[j]) ||
                        (j + 1 < quotes.Count && refactorIndex >= quotes[j + 1]))
                    {
                        continue;
                    }

                    if (j % 2 == 0)
                    {
                        inQuote = true;
                    }

                    break;
                }

                if (inQuote)
                {
                    continue;
                }

                refs.Add(new Reference(fileName, i, refactorIndex, textToFind, originalLine));
            }

            return refs;
        }

        private int HandleBlockComment()
        {
            string contents;
            int newLineIndex = FindToken(EndCommentString, out contents);
            return newLineIndex == -1 ? _lines.Length : newLineIndex;
        }

        private int HandleMacro()
        {
            if (!_tokensEnumerator.MoveNext())
            {
                return _lineIndex;
            }

            bool hasNext = SkipWhitespace();
            if (!hasNext)
            {
                return _lineIndex;
            }

            if (IsEndOfCodeLine())
            {
                return _lineIndex;
            }

            int startLine = _lineIndex;
            string macroName = _tokensEnumerator.Current;
            List<string> args;
            if (_tokensEnumerator.MoveNext())
            {
                hasNext = SkipWhitespace();
                if (!hasNext)
                {
                    args = new List<string>();
                }
                else if (_tokensEnumerator.Current == "(")
                {
                    // its really a macro
                    args = GetMacroArgs();
                }
                else
                {
                    args = new List<string>();
                }
            }
            else
            {
                args = new List<string>();
            }

            string contents;
            int newLineIndex = FindToken(EndMacroString, out contents);
            if (newLineIndex == -1)
            {
                return _lines.Length;
            }

            string description = GetDescription(startLine);
            Macro macroToAdd = new Macro(new DocLocation(startLine, 0), macroName, args, contents, description, _parserInfo);
            _parserInfo.MacrosList.Add(macroToAdd);

            return newLineIndex;
        }

        private void HandleDefine()
        {
            if (!_tokensEnumerator.MoveNext())
            {
                return;
            }

            bool hasNext = SkipWhitespace();
            if (!hasNext)
            {
                return;
            }

            if (IsEndOfCodeLine())
            {
                return;
            }

            string defineName = _tokensEnumerator.Current;
            if (defineName.Any(c => !IsValidNameChar(c)))
            {
                SkipToEndOfLine();
                return;
            }

            string contents;
            string description;
            if (_tokensEnumerator.MoveNext())
            {
                if (_tokensEnumerator.Current == "(")
                {
                    // its really a macro
                    List<string> args = GetMacroArgs();
                    description = GetDescription();
                    SkipToEndOfCodeLine(out contents);
                    Macro macroToAdd = new Macro(new DocLocation(_lineIndex, 0), defineName, args, contents, description, _parserInfo);
                    _parserInfo.MacrosList.Add(macroToAdd);
                    return;
                }

                hasNext = SkipWhitespace();
                if (!hasNext)
                {
                    contents = string.Empty;
                }
                else
                {
                    SkipToEndOfCodeLine(out contents);
                }
            }
            else
            {
                contents = string.Empty;
            }

            description = GetDescription();
            Define defineToAdd = new Define(new DocLocation(_lineIndex, 0), defineName, contents.Trim(), description, _parserInfo, 0);
            _parserInfo.DefinesList.Add(defineToAdd);

        }

        private void HandleInclude()
        {
            if (!_tokensEnumerator.MoveNext())
            {
                return;
            }

            bool hasNext = SkipWhitespace();
            if (!hasNext)
            {
                return;
            }

            bool startQuote = _tokensEnumerator.Current == "\"";
            if (startQuote && !_tokensEnumerator.MoveNext())
            {
                return;
            }

            string includeFile = string.Empty;
            if (startQuote)
            {
                while (_tokensEnumerator.Current != "\"" && hasNext)
                {
                    includeFile += _tokensEnumerator.Current;
                    hasNext = _tokensEnumerator.MoveNext();
                }

                _tokensEnumerator.MoveNext();
            }
            else
            {
                if (IsEndOfCodeLine())
                {
                    return;
                }

                includeFile = _tokensEnumerator.Current;
            }

            if (string.IsNullOrWhiteSpace(includeFile))
            {
                return;
            }

            string description = GetDescription();
            IncludeFile includeToAdd = new IncludeFile(new DocLocation(_lineIndex, 0), includeFile, description, _parserInfo);
            if (!_parserInfo.IncludeFilesList.Contains(includeToAdd))
            {
                _parserInfo.IncludeFilesList.Add(includeToAdd);
            }
        }

        private void HandleLabelOrDefine()
        {
            string description = GetDescription();
            string labelName = _tokensEnumerator.Current;
            if (labelName.Any(c => !IsValidNameChar(c)))
            {
                SkipToEndOfLine();
                return;
            }

            if (!_tokensEnumerator.MoveNext() || _tokensEnumerator.Current == ":")
            {
                // its definitely a label
                Label labelToAdd = new Label(new DocLocation(_lineIndex, 0), labelName, description, _parserInfo);
                _parserInfo.LabelsList.Add(labelToAdd);
            }
            else
            {
                if (_tokensEnumerator.Current == "(")
                {
                    return;
                }

                if (_tokensEnumerator.MoveNext() && _tokensEnumerator.Current == "=" ||
                    _tokensEnumerator.Current.ToLower() == ".equ" || _tokensEnumerator.Current.ToLower() == "equ")
                {
                    string contents;
                    if (!_tokensEnumerator.MoveNext())
                    {
                        contents = string.Empty;
                    }
                    else
                    {
                        if (!SkipWhitespace())
                        {
                            contents = string.Empty;
                        }
                        else
                        {
                            SkipToEndOfCodeLine(out contents);
                        }
                    }

                    Define defineToAdd = new Define(new DocLocation(_lineIndex, 0), labelName, contents.Trim(), description, _parserInfo, 0);
                    _parserInfo.DefinesList.Add(defineToAdd);
                }
                else
                {
                    // it must be a label with no colon
                    Label labelToAdd = new Label(new DocLocation(_lineIndex, 0), labelName, description, _parserInfo);
                    _parserInfo.LabelsList.Add(labelToAdd);
                }
            }
        }

        private int FindToken(string tokenToFind, out string contents)
        {
            contents = string.Empty;
            while (true)
            {
                bool hasNext = _tokensEnumerator.MoveNext();
                if (!hasNext)
                {
                    break;
                }

                if (_tokensEnumerator.Current == CommentChar.ToString())
                {
                    string innerContents;
                    hasNext = SkipToEndOfLine(out innerContents);
                    contents += innerContents;
                    if (!hasNext)
                    {
                        break;
                    }
                }

                if (_tokensEnumerator.Current == tokenToFind)
                {
                    return _lineIndex;
                }

                if (_tokensEnumerator.Current == "\n")
                {
                    _lineIndex++;
                }

                contents += _tokensEnumerator.Current;
            }

            return -1;
        }

        private void SkipToEndOfLine()
        {
            string contents;
            SkipToEndOfLine(out contents);
        }

        private bool SkipToEndOfLine(out string contents)
        {
            contents = string.Empty;
            bool hasNext = true;
            while (hasNext && !IsEndOfLine())
            {
                contents += _tokensEnumerator.Current;
                hasNext = _tokensEnumerator.MoveNext();
            }

            return hasNext;
        }

        private bool SkipToEndOfCodeLine()
        {
            string contents;
            return SkipToEndOfCodeLine(out contents);
        }

        private bool SkipToEndOfCodeLine(out string contents)
        {
            contents = string.Empty;
            bool hasNext = SkipWhitespace();
            if (!hasNext)
            {
                return false;
            }

            bool isInQuote = false;
            while (hasNext && (!IsEndOfCodeLine() || (isInQuote && !IsEndOfLine())))
            {
                contents += _tokensEnumerator.Current;
                if (_tokensEnumerator.Current == "\"")
                {
                    isInQuote = !isInQuote;
                }

                hasNext = _tokensEnumerator.MoveNext();
            }

            return hasNext;
        }

        private bool SkipWhitespace()
        {
            bool hasNext = true;
            while (hasNext && (_tokensEnumerator.Current == " " || _tokensEnumerator.Current == "\t" ||
                _tokensEnumerator.Current == "\r"))
            {
                hasNext = _tokensEnumerator.MoveNext();
            }

            return hasNext;
        }

        private bool IsEndOfCodeLine()
        {
            return _tokensEnumerator.Current == EndOfLineChar.ToString() || _tokensEnumerator.Current == CommentChar.ToString() ||
                _tokensEnumerator.Current == "\n";
        }

        private bool IsEndOfLine()
        {
            return _tokensEnumerator.Current == "\n";
        }

        private string GetDescription()
        {
            return GetDescription(_lineIndex);
        }

        private string GetDescription(int lineIndex)
        {
            // we expect a description to be before whatever line we start on
            lineIndex--;
            string description = string.Empty;
            while (lineIndex >= 0)
            {
                string line = _lines[lineIndex];
                char lineStartChar = line.FirstOrDefault();
                if (lineStartChar == CommentChar || line.Trim() == CommentString)
                {
                    description = line + "\n" + description;
                }
                else if (!IsValidLabelChar(lineStartChar))
                {
                    break;
                }

                lineIndex--;
            }

            return description;
        }

        private List<string> GetMacroArgs()
        {
            List<string> args = new List<string>();
            bool hasNext = _tokensEnumerator.MoveNext();
            while (hasNext && _tokensEnumerator.Current != ")")
            {
                if (_tokensEnumerator.Current != ",")
                {
                    args.Add(_tokensEnumerator.Current);
                }

                hasNext = _tokensEnumerator.MoveNext();
                if (!hasNext)
                {
                    break;
                }

                if (_tokensEnumerator.Current == ")")
                {
                    break;
                }

                if (_tokensEnumerator.Current != ",")
                {
                    continue;
                }

                hasNext = _tokensEnumerator.MoveNext();
                if (!hasNext)
                {
                    break;
                }

                hasNext = SkipWhitespace();
            }

            return args;
        }

        private static bool IsValidLabelChar(char c)
        {
            return char.IsLetterOrDigit(c) || c == '_';
        }

        private static bool IsValidNameChar(char ch)
        {
            return IsValidLabelChar(ch) || ch == '_' || ch == '[' ||
                ch == ']' || ch == '!' || ch == '?' || ch == '.';
        }
    }
}