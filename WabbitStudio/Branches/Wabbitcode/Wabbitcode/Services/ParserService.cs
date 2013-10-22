using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Revsoft.Wabbitcode.Services
{
	public class ParserService : IParserService
	{
		#region Constants

		public const char CommentChar = ';';
		private const string CommentString = "#comment";
		private const string DefineString = "#define";
		private const string Delimeters = "&<>~!%^*()-+=|\\/{}[]:;\"' \n\t\r?,";
		private const string EndCommentString = "#endcomment";
		private const string EndMacroString = "#endmacro";
		public const char EndOfLineChar = '\\';
		private const string IncludeString = "#include";
		private const string MacroString = "#macro";

		#endregion

		#region Events

		public delegate void ParserProgress(object sender, ProgressEventArgs e);
		public event ParserProgress OnParserProgress;

		#endregion

		private readonly Dictionary<string, ParserInformation> _parserInfoDictionary = new Dictionary<string, ParserInformation>();

		private List<KeyValuePair<string, int>> _lineLengthGroup;

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
			foreach (var info in _parserInfoDictionary.Values)
			{
				foreach (var data in info)
				{
					yield return data;
				}
			}
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

		// TODO: remove project service from here
		public void ParseFile(int hashCode, string filename)
		{
			StreamReader reader = null;
			try
			{
				string mimeType = FileOperations.GetMimeType(filename);
				if (mimeType != "text/plain")
				{
					return;
				}

				reader = new StreamReader(filename);
				string lines = reader.ReadToEnd();
				ParseFile(hashCode, filename, lines);
			}
			catch (FileNotFoundException)
			{
				throw;
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error parsing file: " + filename, ex);
			}
			finally
			{
				if (reader != null)
				{
					reader.Close();
				}
			}
		}

		public ParserInformation ParseFile(int hashCode, string file, string lines)
		{
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

			int total = 0;
			_lineLengthGroup = lines.Split('\n').Select(l => new KeyValuePair<string, int>(l, total += l.Length + 1)).ToList();
			ParserInformation info = new ParserInformation(hashCode, file);
			int counter = 0, percent = 0;
			while (counter < lines.Length && counter >= 0)
			{
				int newPercent = counter * 100 / lines.Length;
				if (newPercent < percent)
				{
					// should never get here
					throw new Exception("Repeat!");
				}

				if (percent + 5 <= newPercent)
				{
					percent = newPercent;

					if (OnParserProgress != null)
					{
						OnParserProgress(this, new ProgressEventArgs(percent));
					}
				}

				// handle label other xx = 22 type define
				if (TextUtils.IsValidLabelChar(lines[counter]))
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
						int tempCounter = TextUtils.SkipWhitespace(lines, newCounter);
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
							int temp = TextUtils.SkipWhitespace(lines, tempCounter);
							if (temp == -1)
							{
								// we hit the end of the file and didn't find a value after the =
								defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, string.Empty, description, info, 0);
								info.DefinesList.Add(defineToAdd);
								break;
							}

							counter = temp;
							newCounter = TextUtils.SkipToEndOfCodeLine(lines, tempCounter, EndOfLineChar, CommentChar);
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
							counter = TextUtils.SkipToEndOfLine(lines, counter);
							continue;
						}
						else
						{
							string nextWord = TextUtils.GetWord(lines, tempCounter).ToLower();

							if (nextWord == ".equ" || nextWord == "equ")
							{
								Define defineToAdd;

								// its an equate
								tempCounter = TextUtils.SkipWhitespace(lines, tempCounter + nextWord.Length);
								if (tempCounter == -1)
								{
									// we couldn't find second word start
									defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, string.Empty, description, info, 0);
									info.DefinesList.Add(defineToAdd);
									break;
								}

								int secondWordEnd = TextUtils.SkipToEndOfCodeLine(lines, tempCounter, EndOfLineChar, CommentChar);
								if (secondWordEnd == -1)
								{
									secondWordEnd = lines.Length + 1;
								}

								string contents = lines.Substring(tempCounter, secondWordEnd - tempCounter - 1).Trim();
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

					counter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
					continue;
				}

				int tempVar = TextUtils.SkipWhitespace(lines, counter);
				if (tempVar == -1)
				{
					counter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
					continue;
				}

				counter = tempVar;

				// string substring = lines.Substring(counter).ToLower();
				if (string.Compare(lines, counter, CommentString, 0, CommentString.Length, true) == 0)
				{
					counter = FindString(lines, counter, EndCommentString) + EndCommentString.Length;
				}

				// handle macros, defines, and includes
				else if (string.Compare(lines, counter, DefineString, 0, DefineString.Length, true) == 0)
				{
					string contents;
					Define defineToAdd;
					string description = GetDescription(lines, counter);
					counter += DefineString.Length;
					counter = TextUtils.SkipWhitespace(lines, counter);
					int newCounter = GetDefineName(lines, counter);
					if (newCounter == -1)
					{
						break;
					}

					string defineName = lines.Substring(counter, newCounter - counter);
					if (string.IsNullOrEmpty(defineName))
					{
						continue;
					}

					if (TextUtils.IsValidIndex(newCounter, lines.Length) && lines[newCounter] == '(')
					{
						//skip paren
						counter = newCounter + 1;
						// it says define, but its really a macro
						List<string> args = GetMacroArgs(lines, counter);
						int endParen = FindChar(lines, counter, ')');
						newCounter = TextUtils.SkipToEndOfLine(lines, counter);
						// TODO: fix so that dumbass multiline defines work
						if (endParen > newCounter)
						{
							// missing end paren
							args = new List<string>();
						}
						else
						{
							counter = endParen + 1;
						}

						contents = lines.Substring(counter, newCounter - counter - 1).Trim();
						Macro macroToAdd = new Macro(new DocLocation(GetLine(counter), counter), defineName, args, contents, description, info);
						info.MacrosList.Add(macroToAdd);
						counter = newCounter;
						continue;
					}

					counter = TextUtils.SkipWhitespace(lines, newCounter);
					newCounter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
					if (counter == -1 || counter + 1 == newCounter)
					{
						// end of the road
						defineToAdd = new Define(new DocLocation(GetLine(counter), counter), defineName, string.Empty, description, info, 0);
						info.DefinesList.Add(defineToAdd);
						continue;
					}

					if (newCounter == -1)
					{
						newCounter = lines.Length + 1;
					}

					// its a simple substitution define
					contents = lines.Substring(counter, newCounter - counter - 1).Trim();
					defineToAdd = new Define(new DocLocation(GetLine(counter), counter), defineName, contents, description, info, 0);
					info.DefinesList.Add(defineToAdd);
					counter = newCounter;
				}
				else if (string.Compare(lines, counter, MacroString, 0, MacroString.Length, true) == 0)
				{
					string description = GetDescription(lines, counter);
					counter += MacroString.Length;

					// skip any whitespace
					counter = TextUtils.SkipWhitespace(lines, counter);
					int newCounter = GetDefineName(lines, counter);
					string macroName = lines.Substring(counter, newCounter - counter);
					newCounter = FindChar(lines, newCounter, '(') + 1;
					List<string> args = counter == 0 ? new List<string>() : GetMacroArgs(lines, newCounter);

					counter = TextUtils.SkipToEndOfLine(lines, counter);
					newCounter = FindString(lines, counter, EndMacroString);
					if (newCounter != -1)
					{
						string contents = lines.Substring(counter, newCounter - counter);
						Macro macroToAdd = new Macro(new DocLocation(GetLine(counter), counter), macroName, args, contents, description, info);
						info.MacrosList.Add(macroToAdd);
						counter = newCounter + EndMacroString.Length;
					}

				}
				else if (string.Compare(lines, counter, IncludeString, 0, IncludeString.Length, true) == 0)
				{
					string description = GetDescription(lines, counter);
					counter += IncludeString.Length;

					// we need to find the quotes
					counter = TextUtils.SkipWhitespace(lines, counter);
					if (counter == -1)
					{
						continue;
					}

					int newCounter = lines[counter] == '\"' ? FindChar(lines, ++counter, '\"') :
						TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);

					// if the end of line char is next, then newCounter will be counter + 1
					if (counter == -1 || newCounter == -1 || counter + 1 == newCounter)
					{
						counter = TextUtils.SkipToEndOfLine(lines, counter);
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
						if (lines[newCounter] == '\"')
						{
							newCounter++;
						}
						counter = TextUtils.SkipToEndOfCodeLine(lines, newCounter, EndOfLineChar, CommentChar);
					}
				}
				else if (lines[counter] == CommentChar)
				{
					counter = TextUtils.SkipToEndOfLine(lines, counter);
				}
				else
				{
					counter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
				}
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

			//if (ProjectService.IsInternal)
			//{
			//	if (!string.IsNullOrEmpty(file))
			//	{
			//		_baseDir = Path.GetDirectoryName(file);
			//		FindIncludedFiles(file);
			//	}
			//}
			//else
			//{
			//	_baseDir = ProjectService.ProjectDirectory;
			//	var mainStep = ProjectService.CurrentBuildConfig.Steps.Find(item => !string.IsNullOrEmpty(item.InputFile));
			//	FindIncludedFiles(mainStep.InputFile);
			//}

			//try
			//{
			//	DockingService.MainForm.Invoke(() => DockingService.MainForm.HideProgressBar());
			//}

			//// we've quit for some reason lets get out
			//catch (ObjectDisposedException)
			//{
			//	return null;
			//}
			return info;
		}

		//public ParserInformation ParseFileNew(int hashCode, string fileName, string fileText)
		//{
		//	if (string.IsNullOrEmpty(fileName) && hashCode == 0)
		//	{
		//		Debug.WriteLine("No file name specified");
		//		return null;
		//	}

		//	if (string.IsNullOrEmpty(fileText))
		//	{
		//		Debug.WriteLine("Lines were null or empty");
		//		return null;
		//	}

		//	ParserInformation info = new ParserInformation(hashCode, fileName);
		//	int percent = 0;
		//	string[] lines = fileText.Split('\n');
		//	for (int currentLineIndex = 0; currentLineIndex < lines.Length; currentLineIndex++)
		//	{
		//		string currentLine = lines[currentLineIndex];
		//		ParseLine(info, currentLine);
		//		int newPercent = currentLineIndex * 100 / lines.Length;
		//		if (newPercent < percent)
		//		{
		//			// should never get here
		//			throw new Exception("Repeat!");
		//		}

		//		if (percent + 5 <= newPercent)
		//		{
		//			percent = newPercent;

		//			if (OnParserProgress != null)
		//			{
		//				OnParserProgress(this, new ProgressEventArgs(percent));
		//			}
		//		}

		//		// handle label other xx = 22 type define
		//		if (IsValidLabelChar(currentLineIndex[0]))
		//		{
		//			string description = GetDescription(lines, counter);
		//			int newCounter = GetLabel(lines, counter);
		//			string labelName = lines.Substring(counter, newCounter - counter);
		//			if (newCounter < lines.Length && lines[newCounter] == ':')
		//			{
		//				// its definitely a label
		//				Label labelToAdd = new Label(new DocLocation(GetLine(counter), counter), labelName, description, info);
		//				info.LabelsList.Add(labelToAdd);
		//			}
		//			else
		//			{
		//				int tempCounter = TextUtils.SkipWhitespace(lines, newCounter);
		//				if (tempCounter == -1)
		//				{
		//					// it must be a label with no colon at the very end of a file
		//					Label labelToAdd = new Label(new DocLocation(GetLine(counter), counter), labelName, description, info);
		//					info.LabelsList.Add(labelToAdd);
		//					break;
		//				}

		//				if (lines[tempCounter] == '=')
		//				{
		//					Define defineToAdd;
		//					tempCounter++;
		//					int temp = TextUtils.SkipWhitespace(lines, tempCounter);
		//					if (temp == -1)
		//					{
		//						// we hit the end of the file and didn't find a value after the =
		//						defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, string.Empty, description, info, 0);
		//						info.DefinesList.Add(defineToAdd);
		//						break;
		//					}

		//					counter = temp;
		//					newCounter = TextUtils.SkipToEndOfCodeLine(lines, tempCounter, EndOfLineChar, CommentChar);
		//					if (newCounter == -1)
		//					{
		//						newCounter = lines.Length + 1;
		//					}

		//					string contents = lines.Substring(counter, newCounter - counter - 1).Trim();

		//					// its a define
		//					defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, contents, description, info, 0);
		//					info.DefinesList.Add(defineToAdd);
		//				}
		//				else if (lines[tempCounter] == '(')
		//				{
		//					// must be a macro
		//					counter = TextUtils.SkipToEndOfLine(lines, counter);
		//					continue;
		//				}
		//				else
		//				{
		//					string nextWord = TextUtils.GetWord(lines, tempCounter).ToLower();

		//					if (nextWord == ".equ" || nextWord == "equ")
		//					{
		//						Define defineToAdd;

		//						// its an equate
		//						tempCounter = TextUtils.SkipWhitespace(lines, tempCounter + nextWord.Length);
		//						if (tempCounter == -1)
		//						{
		//							// we couldn't find second word start
		//							defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, string.Empty, description, info, 0);
		//							info.DefinesList.Add(defineToAdd);
		//							break;
		//						}

		//						int secondWordEnd = TextUtils.SkipToEndOfCodeLine(lines, tempCounter, EndOfLineChar, CommentChar);
		//						if (secondWordEnd == -1)
		//						{
		//							secondWordEnd = lines.Length + 1;
		//						}

		//						string contents = lines.Substring(tempCounter, secondWordEnd - tempCounter - 1).Trim();
		//						defineToAdd = new Define(new DocLocation(GetLine(counter), counter), labelName, contents, description, info, 0);
		//						info.DefinesList.Add(defineToAdd);
		//					}
		//					else
		//					{
		//						// it must be a label with no colon
		//						Label labelToAdd = new Label(new DocLocation(GetLine(counter), counter), labelName, description, info);
		//						info.LabelsList.Add(labelToAdd);
		//					}
		//				}
		//			}

		//			counter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
		//			continue;
		//		}

		//		int tempVar = TextUtils.SkipWhitespace(lines, counter);
		//		if (tempVar == -1)
		//		{
		//			counter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
		//			continue;
		//		}

		//		counter = tempVar;

		//		// string substring = lines.Substring(counter).ToLower();
		//		if (string.Compare(lines, counter, CommentString, 0, CommentString.Length, true) == 0)
		//		{
		//			counter = FindString(lines, counter, EndCommentString) + EndCommentString.Length;
		//		}

		//		// handle macros, defines, and includes
		//		else if (string.Compare(lines, counter, DefineString, 0, DefineString.Length, true) == 0)
		//		{
		//			string contents;
		//			Define defineToAdd;
		//			string description = GetDescription(lines, counter);
		//			counter += DefineString.Length;
		//			counter = TextUtils.SkipWhitespace(lines, counter);
		//			int newCounter = GetDefineName(lines, counter);
		//			if (newCounter == -1)
		//			{
		//				break;
		//			}

		//			string defineName = lines.Substring(counter, newCounter - counter);
		//			if (string.IsNullOrEmpty(defineName))
		//			{
		//				continue;
		//			}

		//			if (TextUtils.IsValidIndex(newCounter, lines.Length) && lines[newCounter] == '(')
		//			{
		//				//skip paren
		//				counter = newCounter + 1;
		//				// it says define, but its really a macro
		//				List<string> args = GetMacroArgs(lines, counter);
		//				int endParen = FindChar(lines, counter, ')');
		//				newCounter = TextUtils.SkipToEndOfLine(lines, counter);
		//				// TODO: fix so that dumbass multiline defines work
		//				if (endParen > newCounter)
		//				{
		//					// missing end paren
		//					args = new List<string>();
		//				}
		//				else
		//				{
		//					counter = endParen + 1;
		//				}

		//				contents = lines.Substring(counter, newCounter - counter - 1).Trim();
		//				Macro macroToAdd = new Macro(new DocLocation(GetLine(counter), counter), defineName, args, contents, description, info);
		//				info.MacrosList.Add(macroToAdd);
		//				counter = newCounter;
		//				continue;
		//			}

		//			counter = TextUtils.SkipWhitespace(lines, newCounter);
		//			newCounter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
		//			if (counter == -1 || counter + 1 == newCounter)
		//			{
		//				// end of the road
		//				defineToAdd = new Define(new DocLocation(GetLine(counter), counter), defineName, string.Empty, description, info, 0);
		//				info.DefinesList.Add(defineToAdd);
		//				continue;
		//			}

		//			if (newCounter == -1)
		//			{
		//				newCounter = lines.Length + 1;
		//			}

		//			// its a simple substitution define
		//			contents = lines.Substring(counter, newCounter - counter - 1).Trim();
		//			defineToAdd = new Define(new DocLocation(GetLine(counter), counter), defineName, contents, description, info, 0);
		//			info.DefinesList.Add(defineToAdd);
		//			counter = newCounter;
		//		}
		//		else if (string.Compare(lines, counter, MacroString, 0, MacroString.Length, true) == 0)
		//		{
		//			string description = GetDescription(lines, counter);
		//			counter += MacroString.Length;

		//			// skip any whitespace
		//			counter = TextUtils.SkipWhitespace(lines, counter);
		//			int newCounter = GetDefineName(lines, counter);
		//			string macroName = lines.Substring(counter, newCounter - counter);
		//			newCounter = FindChar(lines, newCounter, '(') + 1;
		//			List<string> args = counter == 0 ? new List<string>() : GetMacroArgs(lines, newCounter);

		//			counter = TextUtils.SkipToEndOfLine(lines, counter);
		//			newCounter = FindString(lines, counter, EndMacroString);
		//			if (newCounter != -1)
		//			{
		//				string contents = lines.Substring(counter, newCounter - counter);
		//				Macro macroToAdd = new Macro(new DocLocation(GetLine(counter), counter), macroName, args, contents, description, info);
		//				info.MacrosList.Add(macroToAdd);
		//				counter = newCounter + EndMacroString.Length;
		//			}

		//		}
		//		else if (string.Compare(lines, counter, IncludeString, 0, IncludeString.Length, true) == 0)
		//		{
		//			string description = GetDescription(lines, counter);
		//			counter += IncludeString.Length;

		//			// we need to find the quotes
		//			counter = TextUtils.SkipWhitespace(lines, counter);
		//			if (counter == -1)
		//			{
		//				continue;
		//			}

		//			int newCounter = lines[counter] == '\"' ? FindChar(lines, ++counter, '\"') :
		//				TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);

		//			// if the end of line char is next, then newCounter will be counter + 1
		//			if (counter == -1 || newCounter == -1 || counter + 1 == newCounter)
		//			{
		//				counter = TextUtils.SkipToEndOfLine(lines, counter);
		//			}
		//			else
		//			{
		//				string includeFile = lines.Substring(counter, newCounter - counter).Trim();
		//				if (!string.IsNullOrEmpty(includeFile))
		//				{
		//					IncludeFile includeToAdd = new IncludeFile(new DocLocation(GetLine(counter), counter), includeFile, description, info);
		//					if (!info.IncludeFilesList.Contains(includeToAdd))
		//					{
		//						info.IncludeFilesList.Add(includeToAdd);
		//					}
		//				}
		//				if (lines[newCounter] == '\"')
		//				{
		//					newCounter++;
		//				}
		//				counter = TextUtils.SkipToEndOfCodeLine(lines, newCounter, EndOfLineChar, CommentChar);
		//			}
		//		}
		//		else if (lines[counter] == CommentChar)
		//		{
		//			counter = TextUtils.SkipToEndOfLine(lines, counter);
		//		}
		//		else
		//		{
		//			counter = TextUtils.SkipToEndOfCodeLine(lines, counter, EndOfLineChar, CommentChar);
		//		}
		//	}

		//	lock (_parserInfoDictionary)
		//	{
		//		_parserInfoDictionary.Remove(file);
		//		_parserInfoDictionary.Add(file, info);
		//		foreach (var item in _parserInfoDictionary)
		//		{
		//			item.Value.IsIncluded = false;
		//		}
		//	}

		//	//if (ProjectService.IsInternal)
		//	//{
		//	//	if (!string.IsNullOrEmpty(file))
		//	//	{
		//	//		_baseDir = Path.GetDirectoryName(file);
		//	//		FindIncludedFiles(file);
		//	//	}
		//	//}
		//	//else
		//	//{
		//	//	_baseDir = ProjectService.ProjectDirectory;
		//	//	var mainStep = ProjectService.CurrentBuildConfig.Steps.Find(item => !string.IsNullOrEmpty(item.InputFile));
		//	//	FindIncludedFiles(mainStep.InputFile);
		//	//}

		//	//try
		//	//{
		//	//	DockingService.MainForm.Invoke(() => DockingService.MainForm.HideProgressBar());
		//	//}

		//	//// we've quit for some reason lets get out
		//	//catch (ObjectDisposedException)
		//	//{
		//	//	return null;
		//	//}
		//	return info;
		//}

		//private void ParseLine(ParserInformation info, string currentLine)
		//{
		//	int counter = 0;
		//	do
		//	{
		//		if (currentLine[counter] == EndOfLineChar)
		//		{
		//			counter++;
		//		}
		//		counter = ParseLineSegment(info, currentLine, counter);
		//		counter = TextUtils.SkipWhitespace(currentLine, counter);
		//	} while (TextUtils.IsValidIndex(counter, currentLine.Length) && currentLine[counter] == EndOfLineChar);
		//}

		//private int ParseLineSegment(ParserInformation info, string currentLine, int counter)
		//{
		//	if (!TextUtils.IsValidIndex(counter, currentLine.Length))
		//	{
		//		return counter;
		//	}

		//	char ch = currentLine[counter];
		//	if (TextUtils.IsEndOfCodeLineChar(ch))
		//	{
		//		return counter;
		//	}
		//	if (IsValidLabelChar())
		//}

		private int FindChar(string substring, int counter, char charToFind)
		{
			if (!TextUtils.IsValidIndex(counter, substring.Length))
			{
				return -1;
			}
			while (TextUtils.IsValidIndex(counter, substring.Length) && substring[counter] != charToFind)
			{
				if (!TextUtils.IsValidIndex(counter, substring.Length) || /*!IsValidIndex(substring, counter + Environment.NewLine.Length) ||
					substring.Substring(counter, Environment.NewLine.Length) == Environment.NewLine ||*/
					substring[counter] == CommentChar)
				{
					return -1;
				}

				counter++;
			}

			return counter;
		}

		//private void FindIncludedFiles(string file)
		//{
		//	if (file.IndexOfAny(Path.GetInvalidPathChars()) != -1)
		//	{
		//		return;
		//	}

		//	if (!Path.IsPathRooted(file))
		//	{
		//		file = Path.Combine(_baseDir, file);
		//	}

		//	ParserInformation[] array;
		//	lock (ProjectService.ParserInfomInformation)
		//	{
		//		array = new ParserInformation[ProjectService.ParserInfomInformation.Count];
		//		ProjectService.ParserInfomInformation.CopyTo(array, 0);
		//	}

		//	ParserInformation fileInfo = array.SingleOrDefault(info => string.Equals(info.SourceFile, file, StringComparison.OrdinalIgnoreCase));
		//	if (!File.Exists(file) || fileInfo == null || !fileInfo.ParsingIncludes)
		//	{
		//		return;
		//	}

		//	fileInfo.IsIncluded = true;
		//	foreach (IIncludeFile include in fileInfo.IncludeFilesList)
		//	{
		//		FindIncludedFiles(include.IncludedFile);
		//	}

		//	fileInfo.ParsingIncludes = false;
		//}

		private static int FindString(string substring, int counter, string searchString)
		{
			int length = substring.Length;
			if (!TextUtils.IsValidIndex(counter, length))
			{
				return -1;
			}
			while (counter + searchString.Length < length && substring.Substring(counter, searchString.Length) != searchString)
			{
				if (!TextUtils.IsValidIndex(counter, length))
				{
					return -1;
				}

				if (substring[counter] == CommentChar)
				{
					TextUtils.SkipToEndOfLine(substring, counter);
				}

				if (substring[counter] == '\"')
				{
					counter++;
					while (TextUtils.IsValidIndex(counter, length) && substring[counter] != '\"')
					{
						if (substring[counter] == '\\' && TextUtils.IsValidIndex(counter + 1, length) &&
							substring[counter + 1] == '\"')
						{
							//skip escaped quote
							counter++;
						}
						counter++;
					}
				}

				counter++;
			}

			if (counter + searchString.Length > length)
			{
				counter = -1;
			}
			return counter;
		}

		private static string GetDescription(string lines, int counter)
		{
			bool foundComment = false;
			int lineStart = counter;
			int length = lines.Length;
			while (TextUtils.IsValidIndex(lineStart, length))
			{
				if (lineStart == 0)
				{
					break;
				}

				int oldLineStart = lineStart;
				lineStart = TextUtils.SkipToBeginningOfLine(lines, lineStart - 2);

				if (TextUtils.IsValidLabelChar(lines[lineStart]))
				{
					if (foundComment)
					{
						break;
					}
					// if we get here, we have found two labels in a row
					counter = lineStart;
					continue;
				}

				if (lines[lineStart] == CommentChar)
				{
					foundComment = true;
					
					continue;
				}

				// move back to the last good line
				lineStart = oldLineStart;
				break;
			}

			return !TextUtils.IsValidIndex(lineStart, length) || !foundComment ?
				string.Empty : lines.Substring(lineStart, counter - lineStart - 1);
		}

		private static int GetLabel(string substring, int counter)
		{
			int length = substring.Length;
			if (!TextUtils.IsValidIndex(counter, length))
			{
				return -1;
			}

			while (counter < length && TextUtils.IsValidLabelChar(substring[counter]))
			{
				if (!TextUtils.IsValidIndex(counter, length))
				{
					return -1;
				}

				counter++;
			}

			return counter;
		}

		private int GetDefineName(string substring, int counter)
		{
			int length = substring.Length;
			if (!TextUtils.IsValidIndex(counter, length))
			{
				return -1;
			}

			while (counter < length && TextUtils.IsValidDefineChar(substring[counter]))
			{
				if (!TextUtils.IsValidIndex(counter, length))
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
			for (i = 1; i < _lineLengthGroup.Count; i++)
			{
				if (_lineLengthGroup[i - 1].Value > counter && counter <= _lineLengthGroup[i].Value)
				{
					return i - 1;
				}
			}

			return i;
		}

		private List<string> GetMacroArgs(string substring, int counter)
		{
			List<string> args = new List<string>();
			int endCounter = FindEndParen(substring, counter);
			while (TextUtils.IsValidIndex(counter, substring.Length) && substring[counter] != ')')
			{
				counter = TextUtils.SkipWhitespace(substring, counter);
				int newCounter = GetLabel(substring, counter);
				if (newCounter == -1)
				{
					return args;
				}

				args.Add(substring.Substring(counter, newCounter - counter));
				counter = FindChar(substring, newCounter, ',');
				if (counter == -1 || counter > endCounter)
				{
					return args;
				}
				counter++;
			}

			return args;
		}

		private static int FindEndParen(string substring, int counter)
		{
			int parenLevel = 0;
			while (TextUtils.IsValidIndex(counter, substring.Length) && !(substring[counter] == ')' && parenLevel == 0))
			{
				if (substring[counter] == '(')
				{
					parenLevel++;
				}
				else if (substring[counter] == ')' && parenLevel != 0)
				{
					parenLevel--;
				}
				counter++;
			}
			return counter;
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

		public ParserInformation GetParserInfo(string fileName)
		{
			lock (_parserInfoDictionary)
			{
				ParserInformation info;
				_parserInfoDictionary.TryGetValue(fileName.ToLower(), out info);
				return info;
			}
		}

		public void DestroyService()
		{

		}

		public void InitService(params object[] objects)
		{

		}
	}

	public class ProgressEventArgs
	{
		public int PercentComplete { get; private set; }

		public ProgressEventArgs(int percentComplete)
		{
			PercentComplete = percentComplete;
		}
	}
}