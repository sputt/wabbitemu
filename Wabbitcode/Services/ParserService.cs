using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Revsoft.Wabbitcode.Services
{
    [ServiceDependency(typeof(IFileReaderService))]
    public class ParserService : IParserService
    {
        #region Constants

        private const char CommentChar = ';';
        private const string CommentString = "#comment";
        private const string DefineString = "#define";
        private const string Delimeters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";
        private const string EndCommentString = "#endcomment";
        private const string EndMacroString = "#endmacro";
        private const char EndOfLineChar = '\\';
        private const string IncludeString = "#include";
        private const string MacroString = "#macro";

        #endregion

        #region Events

        public event ParserProgressHandler OnParserProgress;
        public event ParserFinishedHandler OnParserFinished;

        #endregion

        private readonly Dictionary<string, ParserInformation> _parserInfoDictionary = new Dictionary<string, ParserInformation>();
        private readonly IFileReaderService _fileReaderService;

        /// <summary>
        /// Finds all references to the given text.
        /// </summary>
        /// <param name="file">Fully rooted path to the file</param>
        /// <param name="refString">String to find references to</param>
        public List<Reference> FindAllReferencesInFile(string file, string refString)
        {
            var refs = new List<Reference>();
            string mimeType = FileOperations.GetMimeType(file);
            if (mimeType != "text/plain")
            {
                return refs;
            }

            var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
            int len = refString.Length;

            string[] lines = _fileReaderService.GetFileText(file).Split('\n');
            for (int i = 0; i < lines.Length; i++)
            {
                string line = lines[i];
                string originalLine = line;
                int commentIndex = line.IndexOf(CommentChar);
                if (commentIndex != -1)
                {
                    line = line.Remove(commentIndex);
                }

                if (line.Trim().StartsWith("#comment", options))
                {
                    while (!line.Trim().StartsWith("#endcomment", options))
                    {
                        line = lines[++i];
                    }

                    continue;
                }

                int refactorIndex = line.IndexOf(refString, options);
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

                refs.Add(new Reference(file, i, refactorIndex, refString, originalLine));
            }

            return refs;
        }

        public IEnumerable<IParserData> GetAllParserData()
        {
            return _parserInfoDictionary.Values.SelectMany(info => info);
        }

        public IEnumerable<IParserData> GetParserData(string referenceString, bool caseSensitive)
        {
            if (string.IsNullOrEmpty(referenceString))
            {
                yield break;
            }

            if (!caseSensitive)
            {
                referenceString = referenceString.ToUpper();
            }

            foreach (var info in _parserInfoDictionary.Values)
            {
                foreach (var data in info)
                {
                    string name = caseSensitive ? data.Name : data.Name.ToUpper();
                    if (name == referenceString)
                    {
                        yield return data;
                    }
                }
            }
        }

        public void ParseFile(int hashCode, string filename)
        {
            string mimeType = FileOperations.GetMimeType(filename);
            if (mimeType != "text/plain")
            {
                return;
            }

            string lines = _fileReaderService.GetFileText(filename);
            ParseFile(hashCode, filename, lines);
        }

        private static IEnumerable<string> TokenizeLine(string line)
        {
            Regex re = new Regex("([&<>~!%\\^\\*\\(\\)-\\+=\\|\\\\/\\{}\\[\\]:;\"'\\ \\n\\t\\r\\?,])", RegexOptions.Compiled);
            var tokenList = re.Split(line).Where(t => !string.IsNullOrEmpty(t));
            return tokenList;
        }

        public ParserInformation ParseFile(int hashCode, string file, string fileText)
        {
            if (string.IsNullOrEmpty(file))
            {
                System.Diagnostics.Debug.WriteLine("No file name specified");
                return null;
            }

            if (string.IsNullOrEmpty(fileText))
            {
                System.Diagnostics.Debug.WriteLine("Lines were null or empty");
                return null;
            }

            ParserInformation info = new ParserInformation(hashCode, file);
            int lineIndex;
            var tokens = TokenizeLine(fileText);
            var tokensEnumerator = tokens.GetEnumerator();
            string[] lines = fileText.Split('\n');
            int percent = 0;
            for (lineIndex = 0; lineIndex < lines.Length; lineIndex++)
            {
                int newPercent = lineIndex * 100 / lines.Length;

                if (percent + 5 <= newPercent)
                {
                    percent = newPercent;

                    if (OnParserProgress != null)
                    {
                        OnParserProgress(this, new ParserProgressEventArgs(file, percent));
                    }
                }

                bool hasNext;
                do
                {
                    hasNext = tokensEnumerator.MoveNext();
                    if (!hasNext)
                    {
                        break;
                    }

                    char firstChar = tokensEnumerator.Current.First();
                    if (firstChar == CommentChar)
                    {
                        SkipToEndOfLine(tokensEnumerator);
                        break;
                    }
                    // handle label other xx = 22 type define
                    if (TextUtils.IsValidLabelChar(firstChar))
                    {
                        HandleLabelOrDefine(lines, lineIndex, tokensEnumerator, info);
                    }
                    else
                    {
                        if (char.IsWhiteSpace(firstChar))
                        {
                            hasNext = SkipWhitespace(tokensEnumerator);
                            if (!hasNext)
                            {
                                break;
                            }
                        }

                        switch (tokensEnumerator.Current.ToLower())
                        {
                            case IncludeString:
                                HandleInclude(lines, lineIndex, tokensEnumerator, info);
                                break;
                            case DefineString:
                                HandleDefine(lines, lineIndex, tokensEnumerator, info);
                                break;
                            case CommentString:
                                lineIndex = HandleBlockComment(lines, lineIndex, tokensEnumerator);
                                break;
                            case MacroString:
                                lineIndex = HandleMacro(lines, lineIndex, tokensEnumerator, info);
                                break;
                        }
                    }

                    hasNext = SkipToEndOfCodeLine(tokensEnumerator);
                    if (tokensEnumerator.Current == CommentChar.ToString())
                    {
                        SkipToEndOfLine(tokensEnumerator);
                        break;
                    }
                } while (hasNext && IsEndOfCodeLine(tokensEnumerator) && !IsEndOfLine(tokensEnumerator));
            }

            lock (_parserInfoDictionary)
            {
                _parserInfoDictionary.Remove(file.ToLower());
                _parserInfoDictionary.Add(file.ToLower(), info);
                foreach (var item in _parserInfoDictionary)
                {
                    item.Value.IsIncluded = false;
                }
            }

            if (OnParserFinished != null)
            {
                OnParserFinished(this, new ParserEventArgs(file));
            }

            return info;
        }

        private static int HandleBlockComment(ICollection<string> lines, int lineIndex, IEnumerator<string> tokensEnumerator)
        {
            string contents;
            int newLineIndex = FindToken(lineIndex, tokensEnumerator, EndCommentString, out contents);
            return newLineIndex == -1 ? lines.Count : newLineIndex;
        }

        private int HandleMacro(string[] lines, int lineIndex, IEnumerator<string> tokensEnumerator, ParserInformation info)
        {
            if (!tokensEnumerator.MoveNext())
            {
                return lineIndex;
            }

            bool hasNext = SkipWhitespace(tokensEnumerator);
            if (!hasNext)
            {
                return lineIndex;
            }

            if (IsEndOfCodeLine(tokensEnumerator))
            {
                return lineIndex;
            }

            string macroName = tokensEnumerator.Current;
            List<string> args;
            if (tokensEnumerator.MoveNext())
            {
                hasNext = SkipWhitespace(tokensEnumerator);
                if (!hasNext)
                {
                    args = new List<string>();
                }
                else if (tokensEnumerator.Current == "(")
                {
                    // its really a macro
                    args = GetMacroArgs(tokensEnumerator);
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
            int newLineIndex = FindToken(lineIndex, tokensEnumerator, EndMacroString, out contents);
            if (newLineIndex == -1)
            {
                return lines.Length;
            }

            string description = GetDescription(lines, lineIndex);
            Macro macroToAdd = new Macro(new DocLocation(lineIndex, 0), macroName, args, contents, description, info);
            info.MacrosList.Add(macroToAdd);

            return newLineIndex;
        }

        private void HandleDefine(string[] lines, int lineIndex, IEnumerator<string> tokensEnumerator, ParserInformation info)
        {
            if (!tokensEnumerator.MoveNext())
            {
                return;
            }

            bool hasNext = SkipWhitespace(tokensEnumerator);
            if (!hasNext)
            {
                return;
            }

            if (IsEndOfCodeLine(tokensEnumerator))
            {
                return;
            }

            string defineName = tokensEnumerator.Current;
            string contents;
            string description;
            if (tokensEnumerator.MoveNext())
            {
                hasNext = SkipWhitespace(tokensEnumerator);
                if (!hasNext)
                {
                    contents = string.Empty;
                }
                else if (tokensEnumerator.Current == "(")
                {
                    // its really a macro
                    List<string> args = GetMacroArgs(tokensEnumerator);
                    description = GetDescription(lines, lineIndex);
                    SkipToEndOfCodeLine(tokensEnumerator, out contents);
                    Macro macroToAdd = new Macro(new DocLocation(lineIndex, 0), defineName, args, contents, description, info);
                    info.MacrosList.Add(macroToAdd);
                    return;
                }
                else
                {
                    SkipToEndOfCodeLine(tokensEnumerator, out contents);
                }
            }
            else
            {
                contents = string.Empty;
            }

            description = GetDescription(lines, lineIndex);
            Define defineToAdd = new Define(new DocLocation(lineIndex, 0), defineName, contents.Trim(), description, info, 0);
            info.DefinesList.Add(defineToAdd);

        }

        private static void HandleInclude(string[] lines, int lineIndex, IEnumerator<string> tokensEnumerator, ParserInformation info)
        {
            if (!tokensEnumerator.MoveNext())
            {
                return;
            }

            bool hasNext = SkipWhitespace(tokensEnumerator);
            if (!hasNext)
            {
                return;
            }

            bool startQuote = tokensEnumerator.Current == "\"";
            if (startQuote && !tokensEnumerator.MoveNext())
            {
                return;
            }

            string includeFile = string.Empty;
            if (startQuote)
            {
                while (tokensEnumerator.Current != "\"" && hasNext)
                {
                    includeFile += tokensEnumerator.Current;
                    hasNext = tokensEnumerator.MoveNext();
                }

                tokensEnumerator.MoveNext();
            }
            else
            {
                if (IsEndOfCodeLine(tokensEnumerator))
                {
                    return;
                }

                includeFile = tokensEnumerator.Current;
            }

            if (string.IsNullOrWhiteSpace(includeFile))
            {
                return;
            }

            string description = GetDescription(lines, lineIndex);
            IncludeFile includeToAdd = new IncludeFile(new DocLocation(lineIndex, 0), includeFile, description, info);
            if (!info.IncludeFilesList.Contains(includeToAdd))
            {
                info.IncludeFilesList.Add(includeToAdd);
            }
        }

        private static void HandleLabelOrDefine(string[] lines, int lineIndex, IEnumerator<string> tokensEnumerator, ParserInformation info)
        {
            string description = GetDescription(lines, lineIndex);
            string labelName = tokensEnumerator.Current;
            if (!tokensEnumerator.MoveNext() || tokensEnumerator.Current == ":")
            {
                // its definitely a label
                Label labelToAdd = new Label(new DocLocation(lineIndex, 0), labelName, description, info);
                info.LabelsList.Add(labelToAdd);
            }
            else
            {
                if (tokensEnumerator.Current == "(")
                {
                    return;
                }

                if (tokensEnumerator.MoveNext() && tokensEnumerator.Current == "=" ||
                    tokensEnumerator.Current.ToLower() == ".equ" || tokensEnumerator.Current.ToLower() == "equ")
                {
                    string contents;
                    if (!tokensEnumerator.MoveNext())
                    {
                        contents = string.Empty;
                    }
                    else
                    {
                        if (!SkipWhitespace(tokensEnumerator))
                        {
                            contents = string.Empty;
                        }
                        else
                        {
                            SkipToEndOfCodeLine(tokensEnumerator, out contents);
                        }
                    }

                    Define defineToAdd = new Define(new DocLocation(lineIndex, 0), labelName, contents.Trim(), description, info, 0);
                    info.DefinesList.Add(defineToAdd);
                }
                else
                {
                    // it must be a label with no colon
                    Label labelToAdd = new Label(new DocLocation(lineIndex, 0), labelName, description, info);
                    info.LabelsList.Add(labelToAdd);
                }
            }
        }

        private static int FindToken(int lineIndex, IEnumerator<string> tokensEnumerator, string tokenToFind, out string contents)
        {
            contents = string.Empty;
            while (true)
            {
                bool hasNext = tokensEnumerator.MoveNext();
                if (!hasNext)
                {
                    break;
                }

                if (tokensEnumerator.Current == CommentChar.ToString())
                {
                    hasNext = SkipToEndOfLine(tokensEnumerator);
                    if (!hasNext)
                    {
                        break;
                    }
                }

                if (tokensEnumerator.Current == tokenToFind)
                {
                    return lineIndex;
                }
                
                if (tokensEnumerator.Current == "\n")
                {
                    lineIndex++;
                }
            }

            return -1;
        }

        private static bool SkipToEndOfLine(IEnumerator<string> tokensEnumerator)
        {
            string contents;
            return SkipToEndOfLine(tokensEnumerator, out contents);
        }

        private static bool SkipToEndOfLine(IEnumerator<string> tokensEnumerator, out string contents)
        {
            contents = string.Empty;
            bool hasNext = true;
            while (hasNext && !IsEndOfLine(tokensEnumerator))
            {
                contents += tokensEnumerator.Current;
                hasNext = tokensEnumerator.MoveNext();
            }

            return hasNext;
        }

        private static bool SkipToEndOfCodeLine(IEnumerator<string> tokensEnumerator)
        {
            string contents;
            return SkipToEndOfCodeLine(tokensEnumerator, out contents);
        }

        private static bool SkipToEndOfCodeLine(IEnumerator<string> tokensEnumerator, out string contents)
        {
            contents = string.Empty;
            bool hasNext = SkipWhitespace(tokensEnumerator);
            if (!hasNext)
            {
                return false;
            }

            bool isInQuote = false;
            while (hasNext && (!IsEndOfCodeLine(tokensEnumerator) || (isInQuote && !IsEndOfLine(tokensEnumerator))))
            {
                contents += tokensEnumerator.Current;
                if (tokensEnumerator.Current == "\"")
                {
                    isInQuote = !isInQuote;
                }

                hasNext = tokensEnumerator.MoveNext();
            }

            return hasNext;
        }

        private static bool SkipWhitespace(IEnumerator<string> tokensEnumerator)
        {
            bool hasNext = true;
            while (hasNext && (tokensEnumerator.Current == " " || tokensEnumerator.Current == "\t" || 
                tokensEnumerator.Current == "\r"))
            {
                hasNext = tokensEnumerator.MoveNext();
            }

            return hasNext;
        }

        private static bool IsEndOfCodeLine(IEnumerator<string> tokensEnumerator)
        {
            return tokensEnumerator.Current == EndOfLineChar.ToString() || tokensEnumerator.Current == CommentChar.ToString() ||
                tokensEnumerator.Current == "\n";
        }

        private static bool IsEndOfLine(IEnumerator<string> tokensEnumerator)
        {
            return tokensEnumerator.Current == "\n";
        }

        private static string GetDescription(string[] lines, int lineIndex)
        {
            // we expect a description to be before whatever line we start on
            lineIndex--;
            string description = string.Empty;
            while (lineIndex >= 0)
            {
                string line = lines[lineIndex];
                char lineStartChar = line.FirstOrDefault();
                if (lineStartChar == CommentChar || line.Trim() == CommentString)
                {
                    description = line + "\n" + description;
                }
                else if (!TextUtils.IsValidLabelChar(lineStartChar))
                {
                    break;
                }

                lineIndex--;
            }

            return description;
        }

        private List<string> GetMacroArgs(IEnumerator<string> tokensEnumerator)
        {
            List<string> args = new List<string>();
            bool hasNext = tokensEnumerator.MoveNext();
            while (hasNext && tokensEnumerator.Current != ")")
            {
                if (tokensEnumerator.Current != ",")
                {
                    args.Add(tokensEnumerator.Current);
                }

                hasNext = tokensEnumerator.MoveNext();
                if (!hasNext)
                {
                    break;
                }

                if (tokensEnumerator.Current == ")")
                {
                    break;
                }

                if (tokensEnumerator.Current == ",")
                {
                    hasNext = tokensEnumerator.MoveNext();
                    if (!hasNext)
                    {
                        break;
                    }

                    hasNext = SkipWhitespace(tokensEnumerator);
                }
            }

            return args;
        }

        public ParserInformation GetParserInfo(string fileName)
        {
            lock (_parserInfoDictionary)
            {
                ParserInformation info;
                _parserInfoDictionary.TryGetValue(fileName.ToLower(), out info);
                return info;
            }
        }

        public ParserService(IFileReaderService fileReaderService)
        {
            _fileReaderService = fileReaderService;
        }

        public void DestroyService()
        {

        }

        public void InitService(params object[] objects)
        {

        }
    }
}