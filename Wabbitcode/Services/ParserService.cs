using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Revsoft.Wabbitcode.Services.Project.Interface;
using Revsoft.Wabbitcode.Interface.Services;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services
{
	public class ParserService : IParserService
	{
		public IProject Project { get; private set; }

		ParserInformation parserData;

		#region IService

		public void InitService(params Object[] objects)
		{
            if (objects.Length != 1)
            {
                throw new ArgumentException("Invalid number of arguments");
            }
            if (!(objects[0] is IProject))
            {
                throw new ArgumentException("Argument is not of type IProject");
            }
            Project = (IProject) objects[0];
		}

		public void DestroyService()
		{
            Project = null;
		}

		#endregion

		//private string baseDir;
		//private void FindIncludedFiles(string file)
		//{
		//    if (file.IndexOfAny(Path.GetInvalidPathChars()) != -1)
		//        return;
		//    if (!Path.IsPathRooted(file))
		//        file = Path.Combine(baseDir, file);
		//    ParserInformation fileInfo = null;
		//    ParserInformation[] array;
		//    lock (Project.ParseInfo)
		//    {
		//        array = new ParserInformation[Project.ParseInfo.Count];
		//        Project.ParseInfo.CopyTo(array, 0);
		//    }
		//    foreach (ParserInformation info in array)
		//        if (info.SourceFile.ToLower() == file.ToLower())
		//        {
		//            info.IsIncluded = true;
		//            fileInfo = info;
		//        }
		//    if (!File.Exists(file) || fileInfo == null)
		//        return;
		//    foreach (IIncludeFile include in fileInfo.IncludeFilesList)
		//        FindIncludedFiles(include.IncludedFile);
		//}

		public const char commentChar = ';';
        public const char endOfLineChar = '\\';
		public const string ifString = "#if";
		public const string ifdefString = "#ifdef";
		public const string ifndefString = "#ifndef";
		public const string elseString = "#else";
		public const string endIfString = "#endif";
		public const string defineString = "#define";
		public const string macroString = "#macro";
		public const string endMacroString = "#endmacro";
		public const string includeString = "#include";
		public const string commentString = "#comment";
		public const string endCommentString = "#endcomment";
		public const string regionString = "#region";
		public const string endRegionString = "#endregion";

        /// <summary>
        /// Parses a file for useful information
        /// </summary>
        /// <param name="file">Text in the file to read</param>
        /// <param name="lines"></param>
        /// <param name="increment"></param>
        /// <param name="callback"></param>
        /// <returns></returns>
        public ParserInformation ParseFile(string file, string lines, float increment = .05f, Action<double> callback = null)
        {
            if (string.IsNullOrEmpty(file))
            {
                throw new FileNotFoundException("No file name specified");
            }
            if (string.IsNullOrEmpty(lines))
            {
                throw new Exception("Lines were null or empty");
            }
            ParserInformation info = new ParserInformation(file);
            int counter = 0;
            double percent = 0, newPercent;
            while (counter < lines.Length && counter >= 0)
            {
                newPercent = counter / lines.Length;
                if (newPercent < percent)
                {
                    //should never get here
                    throw new Exception("Repeat!");
                }
                if (percent + increment <= newPercent)
                {
                    percent = newPercent;
                    callback(percent);
                }
                //handle label other xx = 22 type define
                if (IsValidLabelChar(lines[counter]))
                {
                    string description = GetDescription(lines, counter);
                    int newCounter = GetLabel(lines, counter);
                    string labelName = lines.Substring(counter, newCounter - counter);
                    if (newCounter < lines.Length && lines[newCounter] == ':')
                    {
                        //its definitely a label
                        Label labelToAdd = new Label(counter, labelName, false, description, info);
                        info.LabelsList.Add(labelToAdd);
                    }
                    else
                    {
                        int tempCounter = SkipWhitespace(lines, newCounter);
                        if (tempCounter == -1)
                        {
                            //it must be a label with no colon at the very end of a file
                            Label labelToAdd = new Label(counter, labelName, true, description, info);
                            info.LabelsList.Add(labelToAdd);
                            break;
                        }
                        if (lines[tempCounter] == '=')
                        {
                            Define defineToAdd;
                            tempCounter++;
                            int temp = SkipWhitespace(lines, tempCounter);
                            if (temp == -1)
                            {
                                //we hit the end of the file and didn't find a value after the =
                                defineToAdd = new Define(counter, labelName, String.Empty, description, info);
                                info.DefinesList.Add(defineToAdd);
                                break;
                            }
                            counter = temp;
                            newCounter = SkipToEOCL(lines, tempCounter);
                            if (newCounter == -1)
                            {
                                newCounter = lines.Length + 1;
                            }
                            string contents = lines.Substring(counter, newCounter - counter - 1).Trim();
                            //its a define
                            defineToAdd = new Define(counter, labelName, contents, description, info, EvaluateContents(contents));
                            info.DefinesList.Add(defineToAdd);
                        }
                        else if (lines[tempCounter] == '(')
                        {
                            //must be a macro
                            counter = SkipToEOL(lines, counter);
                            continue;
                        }
                        else
                        {
                            string nextWord = null;
                            int secondWordStart = GetWord(lines, tempCounter);
                            if (secondWordStart != -1)
                            {
                                nextWord = lines.Substring(tempCounter, secondWordStart - tempCounter).ToLower();
                            }
                            if (secondWordStart != -1 && (nextWord == ".equ" || nextWord == "equ"))
                            {
                                Define defineToAdd;
                                //its an equate
                                secondWordStart = SkipWhitespace(lines, secondWordStart);
                                if (secondWordStart == -1)
                                {
                                    //we couldn't find second word start
                                    defineToAdd = new Define(counter, labelName, String.Empty, description, info, null);
                                    info.DefinesList.Add(defineToAdd);
                                    break;
                                }
                                int secondWordEnd = SkipToEOCL(lines, secondWordStart);
                                if (secondWordEnd == -1)
                                {
                                    secondWordEnd = lines.Length + 1;
                                }
                                string contents = lines.Substring(secondWordStart, secondWordEnd - secondWordStart - 1);
                                defineToAdd = new Define(counter, labelName, contents, description, info, EvaluateContents(contents));
                                info.DefinesList.Add(defineToAdd);
                            }
                            else
                            {
                                //it must be a label with no colon
                                Label labelToAdd = new Label(counter, labelName, true, description, info);
                                info.LabelsList.Add(labelToAdd);
                            }
                        }
                    }
                    counter = SkipToEOCL(lines, counter);
                    continue;
                }
                int tempVar = SkipWhitespace(lines, counter);
                if (tempVar == -1)
                {
                    counter = SkipToEOCL(lines, counter);
                    continue;
                }
                counter = tempVar;
                //string substring = lines.Substring(counter).ToLower();
                if (string.Compare(lines, counter, commentString, 0, commentString.Length, true) == 0)
                {
                    counter = FindString(lines, counter, endCommentString) + endCommentString.Length;
                }
                //handle macros, defines, and includes
                else if (string.Compare(lines, counter, defineString, 0, defineString.Length, true) == 0)
                {
                    Define defineToAdd;
                    string description = GetDescription(lines, counter);
                    counter += defineString.Length;
                    counter = SkipWhitespace(lines, counter);
                    int newCounter = GetLabel(lines, counter);
                    if (newCounter == -1)
                    {
                        break;
                    }
                    string defineName = lines.Substring(counter, newCounter - counter);
                    if (string.IsNullOrEmpty(defineName))
                    {
                        continue;
                    }
                    counter = SkipWhitespace(lines, newCounter);
                    if (counter == -1)
                    {
                        //end of the road
                        defineToAdd = new Define(counter, defineName, String.Empty, description, info);
                        info.DefinesList.Add(defineToAdd);
                        break;
                    }
                    newCounter = SkipToEOCL(lines, counter);
                    if (newCounter == -1)
                    {
                        newCounter = lines.Length + 1;
                    }

                    string contents = lines.Substring(counter, newCounter - counter - 1);
                    defineToAdd = new Define(counter, defineName, contents, description, info, EvaluateContents(contents));
                    info.DefinesList.Add(defineToAdd);
                    counter = newCounter;
                }
                else if (string.Compare(lines, counter, macroString, 0, macroString.Length, true) == 0)
                {
                    string description = GetDescription(lines, counter);
                    counter += macroString.Length;
                    //skip any whitespace
                    counter = SkipWhitespace(lines, counter);
                    int newCounter = GetLabel(lines, counter);
                    string macroName = lines.Substring(counter, newCounter - counter);
                    newCounter = FindChar(lines, newCounter, '(') + 1;
                    List<string> args;
                    if (counter == 0)
                    {
                        args = new List<string>();
                    }
                    else
                    {
                        args = GetMacroArgs(lines, newCounter);
                    }
                    counter = SkipToEOL(lines, counter);
                    newCounter = FindString(lines, counter, endMacroString);
                    if (newCounter != -1)
                    {
                        string contents = lines.Substring(counter, newCounter - counter);
                        Macro macroToAdd = new Macro(counter, macroName, args, contents, description, info);
                        info.MacrosList.Add(macroToAdd);
                        counter = newCounter + endMacroString.Length;
                    }

                }
                else if (string.Compare(lines, counter, includeString, 0, includeString.Length, true) == 0)
                {
                    string description = GetDescription(lines, counter);
                    counter += includeString.Length;
                    //we need to find the quotes
                    counter = FindChar(lines, counter, ' ') + 1;
                    counter = SkipWhitespace(lines, counter);
                    int newCounter;
                    if (lines[counter] == '\"')
                    {
                        newCounter = FindChar(lines, ++counter, '\"');
                    }
                    else
                    {
                        newCounter = SkipToEOCL(lines, counter);
                    }
                    if (counter == -1 || newCounter == -1)
                    {
                        counter = SkipToEOL(lines, counter);
                    }
                    else
                    {
                        string includeFile = lines.Substring(counter, newCounter - counter);
                        IncludeFile includeToAdd = new IncludeFile(counter, includeFile, description, info);
                        if (!info.IncludeFilesList.Contains(includeToAdd))
                        {
                            info.IncludeFilesList.Add(includeToAdd);
                        }
                        counter = SkipToEOCL(lines, newCounter);
                    }
                }
                else if (lines[counter] == commentChar)
                {
                    counter = SkipToEOL(lines, counter);
                }
                else
                {
                    counter = SkipToEOCL(lines, counter);
                }
            }
            //HideProgressDelegate hideProgress = DockingService.MainForm.HideProgressBar;
            //DockingService.MainForm.Invoke(hideProgress);
            return info;
        }

        private int? EvaluateContents(string contents)
        {
            List<IParserData> parserData = new List<IParserData>();
            string text = contents.ToLower();
            int value;
            if (int.TryParse(contents, out value))
            {
                return value;
            }
            return null;
        }

        private int SkipToEOL(string substring, int counter)
        {
            while (IsValidIndex(substring, counter) && substring[counter] != '\n' && substring[counter] != '\r')
            {
                counter++;
            }
            if (IsValidIndex(substring, counter) && (substring[counter] == '\n' || substring[counter] == '\r'))
            {
                counter++;
            }
            return !IsValidIndex(substring, counter) ? -1 : counter;
        }

        private int SkipToEOCL(string substring, int counter)
        {
            while (IsValidIndex(substring, counter)  && substring[counter] != '\n' && substring[counter] != '\r' &&
                substring[counter] != commentChar && substring[counter] != endOfLineChar)
            {
                counter++;
            }
            if (IsValidIndex(substring, counter) && (substring[counter] == endOfLineChar || 
                substring[counter] == '\n' || substring[counter] == '\r'))
            {
                counter++;
            }
            return !IsValidIndex(substring, counter) ? -1 : counter;
        }

        private List<string> GetMacroArgs(string substring, int counter)
        {
            List<string> args = new List<string>();
            int newCounter;
            while (IsValidIndex(substring, counter) && substring[counter] != ')')
            {
                counter = SkipWhitespace(substring, counter);
                newCounter = GetLabel(substring, counter);
                if (newCounter == -1)
                {
                    return args;
                }
                string argument = substring.Substring(counter, newCounter - counter);
                if (!string.IsNullOrEmpty(argument))
                {
                    args.Add(argument);
                }
                counter = FindChar(substring, newCounter, ',');
                newCounter = FindChar(substring, newCounter, ')');
                if (counter == -1)
                {
                    return args;
                }
                else if (newCounter < counter)
                {
                    return args;  
                }
                else
                {
                    counter++;
                }
            }
            return args;
        }

        const string delimeters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";
        private int GetWord(string text, int offset)
        {
            int newOffset = offset;
            char test = text[offset];
            while (offset > 0 && delimeters.IndexOf(test) == -1)
            {
                test = text[--offset];
            }
            if (offset > 0)
            {
                offset++;
            }
            test = text[newOffset];
            while (newOffset + 1 < text.Length && delimeters.IndexOf(test) == -1)
            {
                test = text[++newOffset];
            }
            if (newOffset < offset)
            {
                return -1;
            }
            return newOffset;
        }

        private int GetLabel(string substring, int counter)
        {
            if (!IsValidIndex(substring, counter))
            {
                return -1;
            }

            while (counter < substring.Length && IsValidLabelChar(substring[counter]))
            {
                if (!IsValidIndex(substring, counter))
                {
                    return -1;
                }
                counter++;
            }
            return counter;
        }

        private int FindChar(string substring, int counter, char charToFind)
        {
            if (!IsValidIndex(substring, counter))
                return -1;
            while (IsValidIndex(substring, counter) && substring[counter] != charToFind)
            {
                if (!IsValidIndex(substring, counter) || substring[counter] == commentChar)
                {
                    return -1;
                }
                counter++;
            }
            return counter;
        }

        private int FindString(string substring, int counter, string searchString)
        {
            if (!IsValidIndex(substring, counter))
            {
                return -1;
            }
            while (counter + searchString.Length < substring.Length && substring.Substring(counter, searchString.Length) != searchString)
            {
                if (!IsValidIndex(substring, counter))
                {
                    return -1;
                }
                if (substring[counter] == commentChar)
                {
                    SkipToEOL(substring, counter);
                }
                counter++;
            }
            if (counter + searchString.Length > substring.Length)
            {
                counter = -1;
            }
            return counter;
        }

        private int SkipWhitespace(string substring, int counter)
        {
            while (IsValidIndex(substring, counter) &&
                    (substring[counter] != '\r' && substring[counter] != '\n')
                        && char.IsWhiteSpace(substring[counter]))
            {
                counter++;
            }
            if (!IsValidIndex(substring, counter))
            {
                return -1;
            }
            return counter;
        }

        private bool IsValidIndex(string substring, int counter)
        {
            return counter > -1 && counter < substring.Length;
        }

        private bool IsValidLabelChar(char c)
        {
            return char.IsLetterOrDigit(c) || c == '_';
        }

        private string GetDescription(string lines, int counter)
        {
            return "";
        }

        private int SkipToNameEnd(string line, int index)
        {
            char[] extLabelSet = { '_', '[', ']', '!', '?', '.' };

            if (string.IsNullOrEmpty(line))
            {
                return -1;
            }
            int end = index;
            while (end < line.Length && (char.IsLetterOrDigit(line[end]) || extLabelSet.Contains(line[end])))
            {
                end++;
            }

            return end;
        }

        private bool IsEndOfCodeLine(string line, int index)
        {
            char charAtIndex = line[index];
            return charAtIndex == '\0' || charAtIndex == '\n' || charAtIndex == '\r' || charAtIndex == ';' || charAtIndex == '\\';
        }

        private bool IsReservedKeyword(string keyword)
        {
            return keyword == "ccf" || keyword == "cpdr" || keyword == "cpd" || keyword == "cpir" || keyword == "cpi" || keyword == "cpl" ||
                keyword == "daa" || keyword == "di" || keyword == "ei" || keyword == "exx" || keyword == "halt" || keyword == "indr" ||
                keyword == "ind" || keyword == "inir" || keyword == "ini" || keyword == "lddr" || keyword == "ldd" || keyword == "ldir" ||
                keyword == "ldi" || keyword == "neg" || keyword == "nop" || keyword == "otdr" || keyword == "otir" || keyword == "outd" ||
                keyword == "outi" || keyword == "reti" || keyword == "retn" || keyword == "rla" || keyword == "rlca" || keyword == "rld" ||
                keyword == "rra" || keyword == "rrca" || keyword == "scf" || keyword == "rst" || keyword == "ex" || keyword == "im" ||
                keyword == "djnz" || keyword == "jp" || keyword == "jr" || keyword == "ret" || keyword == "call" || keyword == "push" ||
                keyword == "pop" || keyword == "cp" || keyword == "xor" || keyword == "sub" || keyword == "add" || keyword == "adc" ||
                keyword == "sbc" || keyword == "dec" || keyword == "inc" || keyword == "rlc" || keyword == "rl" || keyword == "rr" ||
                keyword == "rrc" || keyword == "sla" || keyword == "sll" || keyword == "sra" || keyword == "srl" || keyword == "bit" ||
                keyword == "set" || keyword == "res" || keyword == "in" || keyword == "out" || keyword == "ld";

        }
	}
}
