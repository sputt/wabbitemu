namespace Revsoft.Wabbitcode.Services
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;

    using Revsoft.Wabbitcode;
    using Revsoft.Wabbitcode.Classes;
    using Revsoft.Wabbitcode.Properties;
    using Revsoft.Wabbitcode.Services;
    using Revsoft.Wabbitcode.Services.Parser;

    public class ParserService
    {
        private const char commentChar = ';';
        private const string commentString = "#comment";
        private const string defineString = "#define";
        private const string delimeters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";
        private const string endCommentString = "#endcomment";
        private const string endMacroString = "#endmacro";
        private const char endOfLineChar = '\\';
        private const string includeString = "#include";
        private const string macroString = "#macro";

        private string baseDir;
        private List<KeyValuePair<string, int>> lineLengthGroup;

        public List<List<Reference>> FindAllReferences(string refString)
        {
            var refsList = new List<List<Reference>>();
            if (ProjectService.IsInternal)
            {
                var files = DockingService.Documents;
                foreach (var file in files)
                {
                    var refs = FindAllReferencesInFile(((NewEditor)file).ToolTipText, refString);
                    if (refs.Count > 0)
                    {
                        refsList.Add(refs);
                    }
                }
            }
            else
            {
                var files = ProjectService.Project.GetProjectFiles();
                foreach (var file in files)
                {
                    var refs = FindAllReferencesInFile(Path.Combine(ProjectService.ProjectDirectory, file.FileFullPath), refString);
                    if (refs.Count > 0)
                    {
                        refsList.Add(refs);
                    }
                }
            }

            return refsList;
        }

        /// <summary>
        /// Finds all references to the given text.
        /// </summary>
        /// <param name="file">Fully rooted path to the file</param>
        /// <param name="refString">String to find references to</param>
        public List<Reference> FindAllReferencesInFile(string file, string refString)
        {
            var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
            int len = refString.Length;
            var refs = new List<Reference>();
            StreamReader reader = null;
            string[] lines;
            try
            {
                reader = new StreamReader(file);
                lines = reader.ReadToEnd().Split('\n');
            }
            catch (Exception)
            {
                return refs;
            }
            finally
            {
                if (reader != null)
                {
                    reader.Dispose();
                }
            }

            for (int i = 0; i < lines.Length; i++)
            {
                string line = lines[i];
                string originalLine = line;
                int commentIndex = line.IndexOf(commentChar);
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
                if ((refactorIndex == -1) || (refactorIndex != 0 && !delimeters.Contains(line[refactorIndex - 1]))
                    || (refactorIndex + len < line.Length && !delimeters.Contains(line[refactorIndex + len])))
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
                    if (refactorIndex > quotes[j])
                    {
                        if (j + 1 < quotes.Count && refactorIndex >= quotes[j + 1])
                        {
                            continue;
                        }

                        if (j % 2 == 0)
                        {
                            inQuote = true;
                        }

                        break;
                    }
                }

                if (inQuote)
                {
                    continue;
                }

                refs.Add(new Reference(file, i, refactorIndex, refString, originalLine));
            }

            return refs;
        }

        public int GetWord(string text, int offset)
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

        public ParserInformation ParseFile(int hashCode, string filename)
        {
            string lines = null;
            StreamReader reader = null;
            try
            {
                string mimeType = FileOperations.GetMimeType(filename);
                if (mimeType != "text/plain")
                {
                    System.Diagnostics.Debug.Print("Invalid MIME type {0}", mimeType);
                    return null;
                }

                reader = new StreamReader(filename);
                lines = reader.ReadToEnd();
                return ParseFile(hashCode, filename, lines);
            }
            catch (FileNotFoundException ex)
            {
                System.Windows.Forms.DialogResult result = System.Windows.Forms.MessageBox.Show(
                            ex.FileName + " not found, would you like to remove it from the project?",
                            "File not found",
                            System.Windows.Forms.MessageBoxButtons.YesNo,
                            System.Windows.Forms.MessageBoxIcon.None);
                if (result == System.Windows.Forms.DialogResult.Yes)
                {
                    ProjectService.DeleteFile(filename);
                }
                return null;
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error parsing file: " + filename, ex);
                return null;
            }
            finally
            {
                if (reader != null)
                {
                    reader.Close();
                }
            }
        }

        internal bool IsReservedKeyword(string keyword)
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

        internal ParserInformation ParseFile(int hashCode, string file, string lines)
        {
            int total = 0;
            lineLengthGroup = lines.Split('\n').Select(l => new KeyValuePair<string, int>(l, total += l.Length + 1)).ToList();
            if (string.IsNullOrEmpty(file) && hashCode == 0)
            {
                System.Diagnostics.Debug.WriteLine("No file name specified");
                return null;
            }

            if (string.IsNullOrEmpty(lines))
            {
                System.Diagnostics.Debug.WriteLine("Lines were null or empty");
                return null;
            }

            ParserInformation info = new ParserInformation(hashCode, file);
            var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
            int counter = 0, percent = 0, newPercent;
            while (counter < lines.Length && counter >= 0)
            {
                newPercent = counter * 100 / lines.Length;
                if (newPercent < percent)
                {
                    // should never get here
                    throw new Exception("Repeat!");
                }

                if (percent + 5 <= newPercent)
                {
                    percent = newPercent;
                    try
                    {
                        DockingService.MainForm.Invoke(() => DockingService.MainForm.SetProgress(percent));
                    }

                    // we've quit for some reason lets get out
                    catch (ObjectDisposedException)
                    {
                        return null;
                    }
                }

                // handle label other xx = 22 type define
                if (IsValidLabelChar(lines[counter]))
                {
                    string description = GetDescription(lines, counter);
                    int newCounter = GetLabel(lines, counter);
                    string labelName = lines.Substring(counter, newCounter - counter);
                    if (newCounter < lines.Length && lines[newCounter] == ':')
                    {
                        // its definitely a label
                        Label labelToAdd = new Label(new DocLocation(GetLine(counter), counter), labelName, description, info);
                        info.LabelsList.Add(labelToAdd);
                    }
                    else
                    {
                        int tempCounter = SkipWhitespace(lines, newCounter);
                        if (tempCounter == -1)
                        {
                            // it must be a label with no colon at the very end of a file
                            Label labelToAdd = new Label(new DocLocation(GetLine(counter), counter), labelName, description, info);
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
                                // we hit the end of the file and didn't find a value after the =
                                defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, String.Empty, description, info, 0);
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

                            // its a define
                            defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, contents, description, info, 0);
                            info.DefinesList.Add(defineToAdd);
                        }
                        else if (lines[tempCounter] == '(')
                        {
                            // must be a macro
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

                                // its an equate
                                secondWordStart = SkipWhitespace(lines, secondWordStart);
                                if (secondWordStart == -1)
                                {
                                    // we couldn't find second word start
                                    defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, String.Empty, description, info, 0);
                                    info.DefinesList.Add(defineToAdd);
                                    break;
                                }

                                int secondWordEnd = SkipToEOCL(lines, secondWordStart);
                                if (secondWordEnd == -1)
                                {
                                    secondWordEnd = lines.Length + 1;
                                }

                                string contents = lines.Substring(secondWordStart, secondWordEnd - secondWordStart - 1);
                                defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, contents, description, info, 0);
                                info.DefinesList.Add(defineToAdd);
                            }
                            else
                            {
                                // it must be a label with no colon
                                Label labelToAdd = new Label(new DocLocation(GetLine(counter), counter), labelName, description, info);
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

                // string substring = lines.Substring(counter).ToLower();
                if (string.Compare(lines, counter, commentString, 0, commentString.Length, true) == 0)
                {
                    counter = FindString(lines, counter, endCommentString) + endCommentString.Length;
                }

                // handle macros, defines, and includes
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
                    newCounter = SkipToEOCL(lines, counter);
                    if (counter == -1 || counter + 1 == newCounter)
                    {
                        // end of the road
                        defineToAdd = new Define(new DocLocation(GetLine(counter), counter), defineName, String.Empty, description, info, 0);
                        info.DefinesList.Add(defineToAdd);
                        continue;
                    }

                    if (newCounter == -1)
                    {
                        newCounter = lines.Length + 1;
                    }

                    string contents = lines.Substring(counter, newCounter - counter - 1).Trim();
                    defineToAdd = new Define(new DocLocation(GetLine(counter), counter), defineName, contents, description, info, 0);
                    info.DefinesList.Add(defineToAdd);
                    counter = newCounter;
                }
                else if (string.Compare(lines, counter, macroString, 0, macroString.Length, true) == 0)
                {
                    string description = GetDescription(lines, counter);
                    counter += macroString.Length;

                    // skip any whitespace
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
                        Macro macroToAdd = new Macro(new DocLocation(GetLine(counter), counter), macroName, args, contents, description, info);
                        info.MacrosList.Add(macroToAdd);
                        counter = newCounter + endMacroString.Length;
                    }

                }
                else if (string.Compare(lines, counter, includeString, 0, includeString.Length, true) == 0)
                {
                    string description = GetDescription(lines, counter);
                    counter += includeString.Length;

                    // we need to find the quotes
                    counter = SkipWhitespace(lines, counter);
                    if (counter == -1)
                    {
                        continue;
                    }

                    int newCounter;
                    if (lines[counter] == '\"')
                    {
                        newCounter = FindChar(lines, ++counter, '\"');
                    }
                    else
                    {
                        newCounter = SkipToEOCL(lines, counter);
                    }

                    // if the end of line char is next, then newCounter will be counter + 1
                    if (counter == -1 || newCounter == -1 || counter + 1 == newCounter)
                    {
                        counter = SkipToEOL(lines, counter);
                    }
                    else
                    {
                        string includeFile = lines.Substring(counter, newCounter - counter).Trim();
                        if (!string.IsNullOrEmpty(includeFile))
                        {
                            IncludeFile includeToAdd = new IncludeFile(new DocLocation(GetLine(counter), counter), includeFile, description, info);
                            if (!info.IncludeFilesList.Contains(includeToAdd))
                            {
                                info.IncludeFilesList.Add(includeToAdd);
                            }
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

            ProjectService.RemoveParseData(file);
            lock (ProjectService.ParseInfo)
            {
                ProjectService.ParseInfo.Add(info);
                foreach (var item in ProjectService.ParseInfo)
                {
                    item.IsIncluded = false;
                }
            }

            if (ProjectService.IsInternal)
            {
                if (!string.IsNullOrEmpty(file))
                {
                    baseDir = Path.GetDirectoryName(file);
                    FindIncludedFiles(file);
                }
            }
            else
            {
                baseDir = ProjectService.ProjectDirectory;
                var mainStep = ProjectService.CurrentBuildConfig.Steps.Find(item => !string.IsNullOrEmpty(item.InputFile));
                FindIncludedFiles(mainStep.InputFile);
            }

            try
            {
                DockingService.MainForm.Invoke(() => DockingService.MainForm.HideProgressBar());
            }

            // we've quit for some reason lets get out
            catch (ObjectDisposedException)
            {
                return null;
            }
            return info;
        }

        private int FindChar(string substring, int counter, char charToFind)
        {
            if (!IsValidIndex(substring, counter))
            {
                return -1;
            }
            while (IsValidIndex(substring, counter) && substring[counter] != charToFind)
            {
                if (!IsValidIndex(substring, counter) || /*!IsValidIndex(substring, counter + Environment.NewLine.Length) ||
                    substring.Substring(counter, Environment.NewLine.Length) == Environment.NewLine ||*/
                    substring[counter] == commentChar)
                {
                    return -1;
                }

                counter++;
            }

            return counter;
        }

        private void FindIncludedFiles(string file)
        {
            if (file.IndexOfAny(Path.GetInvalidPathChars()) != -1)
            {
                return;
            }

            if (!Path.IsPathRooted(file))
            {
                file = Path.Combine(baseDir, file);
            }

            ParserInformation[] array;
            lock (ProjectService.ParseInfo)
            {
                array = new ParserInformation[ProjectService.ParseInfo.Count];
                ProjectService.ParseInfo.CopyTo(array, 0);
            }

            ParserInformation fileInfo = array.SingleOrDefault(info => string.Equals(info.SourceFile, file, StringComparison.OrdinalIgnoreCase));
            if (!File.Exists(file) || fileInfo == null || !fileInfo.ParsingIncludes)
            {
                return;
            }

            fileInfo.IsIncluded = true;
            fileInfo.ParsingIncludes = true;
            foreach (IIncludeFile include in fileInfo.IncludeFilesList)
            {
                FindIncludedFiles(include.IncludedFile);
            }

            fileInfo.ParsingIncludes = false;
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

        private string GetDescription(string lines, int counter)
        {
            return "";
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

        private int GetLine(int counter)
        {
            int i;
            for (i = 1; i < lineLengthGroup.Count; i++)
            {
                if (lineLengthGroup[i - 1].Value > counter && counter <= lineLengthGroup[i].Value)
                {
                    return i - 1;
                }
            }

            return i;
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

                args.Add(substring.Substring(counter, newCounter - counter));
                counter = FindChar(substring, newCounter, ',');
                if (counter == -1)
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

        private bool IsValidIndex(string substring, int counter)
        {
            return counter > -1 && counter < substring.Length;
        }

        private bool IsValidLabelChar(char c)
        {
            return char.IsLetterOrDigit(c) || c == '_';
        }

        private int SkipToEOCL(string substring, int counter)
        {
            while (IsValidIndex(substring, counter) && substring[counter] != '\n' && substring[counter] != '\r' &&
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

        /*public int EvaluateContents(string contents)
        {
            List<IParserData> parserData = new List<IParserData>();
            string text = contents.ToLower();
            int value;
            if (int.TryParse(contents, out value))
                return value;
            lock (ProjectService.ParseInfo)
            {
                for (int i = 0; i < ProjectService.ParseInfo.Count; i++)
                {
                    var info = ProjectService.ParseInfo[i];
                    foreach (IParserData data in info.GeneratedList)
                        if (data.Name.ToLower() == text)
                        {
                            parserData.Add(data);
                            break;
                        }
                }
            }
            if (parserData.Count > 0)
            {
                foreach (IParserData data in parserData)
                {
                    if (data.GetType() == typeof(Label))
                        return 0x4000;                  //arbitrary number > 255. maybe someday i'll parse label values :/
                    if (data.GetType() == typeof(Define))
                        return ((IDefine) data).Value;
                }
                return 0;
            }
            else
                return 0;
        }*/
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

        private int SkipWhitespace(string substring, int counter)
        {
            while (IsValidIndex(substring, counter) &&
                   (substring[counter] != '\r' && substring[counter]  != '\n')
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
    }
}