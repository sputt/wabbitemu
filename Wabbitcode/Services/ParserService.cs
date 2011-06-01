using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public static class ParserService
	{
        internal static void IsSafeRefactor(string line, int index)
        {

        }

        static string currentLine;
        static int currentIndex;
        internal static List<ParsedLineSec> ParseLine(string line)
        {
            currentLine = line;
            List<ParsedLineSec> lineSections = new List<ParsedLineSec>();
            int index = 0;
            ParsedLineSec section;
            do
            {
                if (currentLine[index] == '\\')
                    index++;
                section = ParseLineSec();
                lineSections.Add(section);
                index = SkipWhitespace(currentLine, index);
            } while (section != null && !section.Error && index >= 0 && index < line.Length);
            return lineSections;
        }

        private static ParsedLineSec ParseLineSec()
        {
            ParsedLineSec sec = new ParsedLineSec(currentLine);
            if (IsEndOfCodeLine(currentIndex))
                return sec;
            char firstChar = currentLine[currentIndex];
            if (char.IsLetter(firstChar) || firstChar == '_')
            {
                sec.Label = currentLine.Substring(currentIndex, SkipToNameEnd(currentLine, currentIndex) - currentIndex);
                if (currentLine[SkipWhitespace(currentLine, currentIndex)] == '(')
                {
                    //its a macro with no indent
                    sec.Command = sec.Label;
                    sec.Label = null;
                }

            }
            else if (char.IsWhiteSpace(firstChar))
            {

            }
            return sec;
        }

        

        internal static void RemoveParseData(string fullPath)
        {
            ParserInformation replaceMe = ProjectService.GetParseInfo(fullPath);
            if (replaceMe != null)
                ProjectService.ParseInfo.Remove(replaceMe);
        }

        private static string baseDir;
        private static void FindIncludedFiles(string file)
        {
            if (file.IndexOfAny(Path.GetInvalidPathChars()) != -1)
                return;
            if (!Path.IsPathRooted(file))
                file = Path.Combine(baseDir, file);
            ParserInformation fileInfo = null;
            ParserInformation[] array;
            lock (ProjectService.ParseInfo)
            {
                array = new ParserInformation[ProjectService.ParseInfo.Count];
                ProjectService.ParseInfo.CopyTo(array, 0);
            }
            foreach (ParserInformation info in array)
                if (info.SourceFile.ToLower() == file.ToLower())
                {
                    info.IsIncluded = true;
                    fileInfo = info;
                }
            if (!File.Exists(file) || fileInfo == null)
                return;
            foreach (IIncludeFile include in fileInfo.IncludeFilesList)
                FindIncludedFiles(include.IncludedFile);
        }

		const char commentChar = ';';
		const string defineString = "#define";
		const string macroString = "#macro";
		const string endMacroString = "#endmacro";
		const string includeString = "#include";
		const string commentString = "#comment";
		const string endCommentString = "#endcomment";
		public static ParserInformation ParseFile(string file)
		{
			string lines = null;
			StreamReader reader = null;
#if !DEBUG
			try
			{
#endif
				reader = new StreamReader(file);
				lines = reader.ReadToEnd();
                //NewParser.NewParser.ParseFile(file);
                reader.Close();
				return ParseFile(file, lines);
#if !DEBUG
			}
			catch (FileNotFoundException ex)
			{
				DialogResult result = MessageBox.Show(ex.FileName + " not found, would you like to remove it from the project?",
					"File not found", MessageBoxButtons.YesNo, MessageBoxIcon.None);
				if (result == DialogResult.Yes)
					ProjectService.DeleteFile(file);
				return null;
			}
			catch (Exception ex)
			{
				MessageBox.Show("File: " + file + "\n" + ex.ToString());
				return null;
			}
			finally
			{
				if (reader != null)
					reader.Close();
			}
#endif
		}

		delegate void HideProgressDelegate();
		delegate void ProgressDelegate(int percent);
		internal static ParserInformation ParseFile(string file, string lines)
		{
			int line = 0;
            if (string.IsNullOrEmpty(file))
            {
                System.Diagnostics.Debug.WriteLine("No file name specified");
                return null;
            }
            if (string.IsNullOrEmpty(lines))
            {
                System.Diagnostics.Debug.WriteLine("Lines were null or empty");
                return null;
            }
			ParserInformation info = new ParserInformation(file);
			int counter = 0, percent = 0, newPercent;
			ProgressDelegate progressDelegate = new ProgressDelegate(DockingService.MainForm.SetProgress);
			while (counter < lines.Length && counter >= 0)
			{
				newPercent = counter * 100 / lines.Length;
                if (newPercent < percent)
                    throw new Exception("Repeat!");
				if (percent + 5 <= newPercent){
					percent = newPercent;
					try
					{
						DockingService.MainForm.Invoke(progressDelegate, percent);
					}
					//we've quit for some reason lets get out
					catch (ObjectDisposedException) { return null; }
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
						Label labelToAdd = new Label(new DocLocation(line, counter), labelName, description, info);
						info.LabelsList.Add(labelToAdd);
					}
					else
					{
						int tempCounter = SkipWhitespace(lines, newCounter);
                        if (tempCounter == -1)
                            break;
						if (lines[tempCounter] == '=')
						{
							tempCounter++;
							int temp = SkipWhitespace(lines, tempCounter);
							if (temp == -1)
								continue;
							else
								counter = temp;
							newCounter = SkipToEOCL(lines, counter);
							string contents = lines.Substring(counter, newCounter - counter).Trim();
							//its a define
							var defineToAdd = new Equate(new DocLocation(line, counter), labelName, contents, description, info);
							info.LabelsList.Add(defineToAdd);
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
                            if (secondWordStart > -1)
                                nextWord = lines.Substring(tempCounter, secondWordStart - tempCounter).ToLower();
                            if (secondWordStart > -1 && (nextWord == ".equ" || nextWord == "equ"))
                            {
                                //its an equate
                                secondWordStart = SkipWhitespace(lines, secondWordStart);
                                int secondWordEnd = SkipToEOCL(lines, secondWordStart);
                                string contents = lines.Substring(secondWordStart, secondWordEnd - secondWordStart);
								Define defineToAdd = new Define(new DocLocation(line, counter), labelName, contents, description, info, 0x4000);//EvaluateContents(contents));
                                info.DefinesList.Add(defineToAdd);
                            }
                            else
                            {
                                //it must be a label with no colon
								Label labelToAdd = new Label(new DocLocation(line, counter), labelName, description, info);
                                info.LabelsList.Add(labelToAdd);
                            }
						}
					}
					counter = SkipToEOL(lines, counter);
					continue;
				}
				counter = SkipWhitespace(lines, counter);
                if (counter < 0)
                    break;
				//string substring = lines.Substring(counter).ToLower();
				if (string.Compare(lines, counter, commentString, 0, commentString.Length, true) == 0)
				{
					counter = FindString(lines, counter, endCommentString) + endCommentString.Length;
				}
				//handle macros, defines, and includes
                else if (string.Compare(lines, counter, defineString, 0, defineString.Length, true) == 0)
				{
					string description = GetDescription(lines, counter);
					counter += defineString.Length;
					counter = SkipWhitespace(lines, counter);
					int newCounter = GetLabel(lines, counter);
					string defineName = lines.Substring(counter, newCounter - counter);
					counter = SkipWhitespace(lines, newCounter);
					newCounter = SkipToEOCL(lines, counter);
					string contents = lines.Substring(counter, newCounter - counter);
					Define defineToAdd = new Define(new DocLocation(line, counter), defineName, contents, description, info, 0x4000);//EvaluateContents(contents));
					info.DefinesList.Add(defineToAdd);
					counter = SkipWhitespace(lines, newCounter);
					counter = SkipToEOL(lines, counter);
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
						args = new List<string>();
					else
						args = GetMacroArgs(lines, newCounter);
					counter = SkipToEOL(lines, counter);
					newCounter = FindString(lines, counter, endMacroString);
                    if (newCounter != -1)
                    {
                        string contents = lines.Substring(counter, newCounter - counter);
                        Macro macroToAdd = new Macro(new DocLocation(line, counter), macroName, args, contents, description, info);
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
                        newCounter = FindChar(lines, ++counter, '\"');

                    else
                        newCounter = SkipToEOCL(lines, counter);
					if (counter == -1 || newCounter == -1)
						counter = SkipToEOL(lines, counter);
					else
					{
						string includeFile = lines.Substring(counter, newCounter - counter);
						IncludeFile includeToAdd = new IncludeFile(new DocLocation(line, counter), includeFile, description, info);
						info.IncludeFilesList.Add(includeToAdd);
                        counter = SkipToEOL(lines, newCounter);
					}
				}
				else
				{
					counter = SkipToEOL(lines, counter);
				}
				line++;
			}
            RemoveParseData(file);
            lock (ProjectService.ParseInfo) {
			    ProjectService.ParseInfo.Add(info);
            }
            if (ProjectService.IsInternal)
            {
                baseDir = Path.GetDirectoryName(file);
                FindIncludedFiles(file);
            }
            else
            {
                baseDir = ProjectService.ProjectDirectory;
                FindIncludedFiles(baseDir);
            }
			try
			{
				HideProgressDelegate hideProgress = DockingService.MainForm.HideProgressBar;
				DockingService.MainForm.Invoke(hideProgress);
			}
			//we've quit for some reason lets get out
			catch (ObjectDisposedException) { return null; }
			return info;
		}

        /*public static int EvaluateContents(string contents)
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

		private static int SkipToEOL(string substring, int counter)
		{
			while (IsValidIndex(substring, counter + Environment.NewLine.Length) &&
				(substring[counter] != '\n' || substring[counter] == commentChar))
				counter++;
			counter++;			//skip newline
			return !IsValidIndex(substring, counter) ? -1 : counter;
		}

		private static int SkipToEOCL(string substring, int counter)
		{
			while (IsValidIndex(substring, counter) && IsValidIndex(substring, counter + Environment.NewLine.Length) &&
				substring.Substring(counter, Environment.NewLine.Length) != Environment.NewLine && substring[counter] != commentChar)
				counter++;
			return !IsValidIndex(substring, counter) ? -1 : counter;
		}

		private static List<string> GetMacroArgs(string substring, int counter)
		{
			List<string> args = new List<string>();
			int newCounter;
			while (IsValidIndex(substring, counter) && substring[counter] != ')')
			{
				counter = SkipWhitespace(substring, counter);
				newCounter = GetLabel(substring, counter);
				if (newCounter == -1)
					return args;
				args.Add(substring.Substring(counter, newCounter - counter));
				counter = FindChar(substring, newCounter, ',');
				if (counter == -1)
					return args;
				else
					counter++;
			}
			return args;
		}

        const string delimeters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";
        public static int GetWord(string text, int offset)
        {
            int newOffset = offset;
            char test = text[offset];
            while (offset > 0 && delimeters.IndexOf(test) == -1)
                test = text[--offset];
            if (offset > 0)
                offset++;
            test = text[newOffset];
            while (newOffset + 1 < text.Length && delimeters.IndexOf(test) == -1)
                test = text[++newOffset];
            if (newOffset < offset)
                return -1;
            return newOffset;
        }

		private static int GetLabel(string substring, int counter)
		{
			if (!IsValidIndex(substring, counter))
				return -1;

			while (counter < substring.Length && IsValidLabelChar(substring[counter]))
			{
				if (!IsValidIndex(substring, counter))
					return -1;
				counter++;
			}
			return counter;
		}

		private static int FindChar(string substring, int counter, char charToFind)
		{
			if (!IsValidIndex(substring, counter))
				return -1;
			while (IsValidIndex(substring, counter) && substring[counter] != charToFind)
			{
				if (!IsValidIndex(substring, counter) || /*!IsValidIndex(substring, counter + Environment.NewLine.Length) ||
					substring.Substring(counter, Environment.NewLine.Length) == Environment.NewLine ||*/
					substring[counter] == commentChar)
					return -1;
				counter++;
			}
			return counter;
		}

		private static int FindString(string substring, int counter, string searchString)
		{
			if (!IsValidIndex(substring, counter))
				return -1;
			while (counter+searchString.Length < substring.Length && substring.Substring(counter, searchString.Length) != searchString)
			{
				if (!IsValidIndex(substring, counter))
					return -1;
				if (substring[counter] == commentChar)
					SkipToEOL(substring, counter);
				counter++;
			}
            if (counter + searchString.Length > substring.Length)
                counter = -1;
			return counter;
		}

		private static int SkipWhitespace(string substring, int counter)
		{
			while (IsValidIndex(substring, counter) && 
					(substring[counter] != '\r' && substring[counter]  != '\n' ) 
						&& char.IsWhiteSpace(substring[counter]))
				counter++;
			if (!IsValidIndex(substring, counter))
				return -1;
			return counter;
		}

		private static bool IsValidIndex(string substring, int counter)
		{
			return counter > -1 && counter < substring.Length ;
		}

		private static bool IsValidLabelChar(char c)
		{
			return char.IsLetterOrDigit(c) || c == '_';
		}

		private static string GetDescription(string lines, int counter)
		{
			return "";
		}

        private static int SkipToNameEnd(string line, int index)
        {
            char[] ext_label_set = { '_', '[', ']', '!', '?', '.' };

            if (string.IsNullOrEmpty(line))
                return -1;
            int end = index;
            while (end < line.Length && (char.IsLetterOrDigit(line[end]) || ext_label_set.Contains(line[end])))
                end++;

            return end;
        }

        private static bool IsEndOfCodeLine(int index)
        {
            char charAtIndex = currentLine[index];
            return charAtIndex == '\0' || charAtIndex == '\n' || charAtIndex == '\r' || charAtIndex == ';' || charAtIndex == '\\';
        }

        internal static bool IsReservedKeyword(string keyword)
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
		/*haha this looks so quaint now ;)
         * public void getAllProjectLabels(ArrayList files)
		{
			string file = "";
			try
			{
				FileStream stream;
				StreamReader reader;
				//int counter = 0;
				for (int i = 0; i < files.Count; i++)
				{
					file = files[i].ToString();
					if (!File.Exists(file))
						continue;
					stream = new FileStream(file, FileMode.Open);
					reader = new StreamReader(stream);
					string[] lines = reader.ReadToEnd().Split('\n');
					ArrayList temp = getAllLabels(lines, true, file);
					//if (((ArrayList)temp[1]).Count != 0)
					projectLabels.Add(temp);
					//counter++;
					stream.Flush();
					stream.Close();
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show("Error getting project labesls from file: " + file + "\n" + ex.ToString());
			}
		}

		public ArrayList getAllLabels(string[] lines, bool includeEquates, string fileName)
		{
			ArrayList includeFile = new ArrayList { new ArrayList(), new ArrayList(), new ArrayList() };
			ArrayList labels = (ArrayList)includeFile[0];
			ArrayList properties = (ArrayList)includeFile[1];
			ArrayList description = (ArrayList)includeFile[2];
			int location = 0;
			int counter = 0;
			string text;
			string endText = "|label|";
			foreach (string line in lines)
			{
				int lineLength = line.Length;
				if (lineLength <= 0)
					continue;
				char firstChar = line[0];
				if (char.IsWhiteSpace(line[0]) || "\r \t;.".IndexOf(firstChar) != -1 && (!line.Contains("#define") || !line.Contains("#macro")))
				{
					location += lineLength + 1;
					counter++;
					continue;
				}
				//Find first word
				char[] whiteSpaceNext = { ' ', '\t', '\n', ':', '\r' };
				int select = line.IndexOfAny(whiteSpaceNext);
				//Check if this is a label or an equate
				int label = line.IndexOf(':');
				//Check make sure the first char is not whitespace, check that its either a label or an equate (if we need to find them)
				if (label == select && label != -1 || includeEquates && ((select != -1 && line[0] != '#') || line.StartsWith("#define") || line.StartsWith("#macro")))
				{
					//if were including equates and theres no colon, its an equate, lets set it to that
					if (includeEquates && label == -1)
					{
						if (line.Contains("=") || line.ToLower().Contains("equ") || line.ToLower().Contains("#define") || line.ToLower().Contains("#macro"))
						{
							label = select;
							endText = "|equate|";
						}
						else
						{
							location += lineLength + 1;
							counter++;
							continue;
						}
					}
					if (label == -1)
						label = 1;
					//if its case sensitive we need to put it into lowercse
					if (line.Contains("#define") || line.Contains("#macro"))
					{
						label = line.IndexOf("#define") + 8;
						if (label == 7)
							label = line.IndexOf("#macro") + 7;
						if (label > line.Length || line.IndexOfAny(whiteSpaceNext, label) == -1)
						{
							location += lineLength + 1;
							counter++;
							continue;
						}
						char[] defineSpace = { ' ', '\t', '\n', ':', '\r', '(' };
						text = line.Substring(label, line.IndexOfAny(defineSpace, label) - label);
						if ((line.IndexOf('(', label) != -1 && line.Contains("#define") || line.Contains("#macro")))
							endText = "|macro|";
					}
					else
						text = line.Substring(0, label);
					if (Settings.Default.caseSensitive)
						labels.Add(text);
					else
						labels.Add(text.ToLower());
					properties.Add(fileName + endText + location);
					int back = 1;
					text = "";
					if (counter - back > 0)
					{
						while (lines[counter - back].StartsWith(";"))
						{
							text = lines[counter - back] + text;
							back++;
							if (counter - back < 0)
								break;
						}
					}
					description.Add(text.Replace('\r', '\n'));
					//reset the ending
					endText = "|label|";
				}
				location += lineLength + 1;
				counter++;
			}
			return includeFile;
		}*/
    }
}
