using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using System.Linq;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	class CodeCompletionProvider : ICompletionDataProvider
	{
		readonly Editor _editor;
		readonly TextEditorControl _textEditor;
        readonly IParserService _parserService;

		public CodeCompletionProvider(Editor editor, TextEditorControl textTextEditor, IParserService parserService)
		{
			_editor = editor;
			_textEditor = textTextEditor;
            _parserService = parserService;
		}

		public ImageList ImageList
		{
			get { return _editor.imageList1; }
		}

		public string PreSelection
		{
			get { return null; }
		}

		public int DefaultIndex
		{
			get { return -1; }
		}

		public CompletionDataProviderKeyResult ProcessKey(char key)
		{
			if (char.IsLetterOrDigit(key) || key == '\"' || key == '.' || key == '(' || key == '_' || key == '$')
			{
				return CompletionDataProviderKeyResult.NormalKey;
			}
			// key triggers insertion of selected items
			return CompletionDataProviderKeyResult.InsertionKey;
		}

		/// <summary>
		/// Called when entry should be inserted. Forward to the insertion action of the completion data.
		/// </summary>
		public bool InsertAction(ICompletionData data, TextArea textArea, int insertionOffset, char key)
		{
			textArea.Caret.Position = textArea.Document.OffsetToPosition(insertionOffset);
			bool temp = data.InsertAction(textArea, key);
			if (key == '\n')
			{
				textArea.Caret.Line += 1;
				textArea.Caret.Column += textArea.Document.FormattingStrategy.IndentLine(textArea.Document, textArea.Caret.Line);
			}
			textArea.Refresh();
			return temp;
		}

		private string GetLine(int start)
		{
			if (string.IsNullOrEmpty(_textEditor.Text) || start < 0)
			{
				return string.Empty;
			}
			if (start == _textEditor.Text.Length)
			{
				start -= 2;
			}
			while (start >= 0 && _textEditor.Text[start] != '\n')
			{
				start--;
			}
			start++;
			int end = start;
			//comment effectively ends the line (for our purposes)
			while (end < _textEditor.Text.Length && _textEditor.Text[end] != '\n' && _textEditor.Text[end] != ';')
			{
				end++;
			}
			end--;
			if (end - start == -1)
			{
				return string.Empty;
			}
			string line = _textEditor.Text.Substring(start, end - start);
			return line;
		}

		private string GetOpcodeOrMacro(string line)
		{
			int start = 0;
			start = SkipWhitespace(line, start);
			int end = start + 1;
			while (end < line.Length && !char.IsWhiteSpace(line[end]))
			{
				end++;
			}
			if (end > line.Length)
			{
				return string.Empty;
			}
			return line.Substring(start, end - start).Trim();
		}

		private string GetFirstArg(string line, int offset, char charTyped)
		{
			int start = 0;
			start = SkipWhitespace(line, start);
			start += offset;
			int end = start;
			while (end < line.Length && line[end] != ',')
				end++;
			end--;
			return (line[end] != ',' && "(),+-*/\r".IndexOf(charTyped) == -1) || end - start < 1 ? "" : line.Substring(start + 1, end - start).Trim();
		}

		private int SkipWhitespace(string line, int start)
		{
			while (start < line.Length && char.IsWhiteSpace(line[start]))
				start++;
			return start;
		}

		private void AddParserData(List<ICompletionData> resultList)
		{
		    var data = _parserService.GetAllParserData().Where(s => (s is ILabel && !((ILabel) s).IsReusable) || s is IDefine);
		    resultList.AddRange(data.Select(parserData => new CodeCompletionData(parserData.Name, CodeCompletionType.Label, parserData.Description)));
		}

	    #region Predefined Data

		readonly ICompletionData[] _preprocessors =
		{
			new CodeCompletionData("define", CodeCompletionType.Preprocessor),
			new CodeCompletionData("ifdef", CodeCompletionType.Preprocessor),
			new CodeCompletionData("ifndef", CodeCompletionType.Preprocessor),
			new CodeCompletionData("if", CodeCompletionType.Preprocessor),
			new CodeCompletionData("endif", CodeCompletionType.Preprocessor),
			new CodeCompletionData("macro", CodeCompletionType.Preprocessor),
			new CodeCompletionData("endmacro", CodeCompletionType.Preprocessor),
			new CodeCompletionData("comment", CodeCompletionType.Preprocessor),
			new CodeCompletionData("endcomment", CodeCompletionType.Preprocessor),
			new CodeCompletionData("else", CodeCompletionType.Preprocessor),
			new CodeCompletionData("undefine", CodeCompletionType.Preprocessor),
			new CodeCompletionData("include", CodeCompletionType.Preprocessor),
			new CodeCompletionData("region", CodeCompletionType.Preprocessor),
			new CodeCompletionData("endregion", CodeCompletionType.Preprocessor)
		};

		readonly ICompletionData[] _directives =
		{
			new CodeCompletionData("db", CodeCompletionType.Directive),
			new CodeCompletionData("dw", CodeCompletionType.Directive),
			new CodeCompletionData("end", CodeCompletionType.Directive),
			new CodeCompletionData("or", CodeCompletionType.Directive),
			new CodeCompletionData("byte", CodeCompletionType.Directive),
			new CodeCompletionData("word", CodeCompletionType.Directive),
			new CodeCompletionData("fill", CodeCompletionType.Directive),
			new CodeCompletionData("block", CodeCompletionType.Directive),
			new CodeCompletionData("addinstr", CodeCompletionType.Directive),
			new CodeCompletionData("echo", CodeCompletionType.Directive),
			new CodeCompletionData("error", CodeCompletionType.Directive),
			new CodeCompletionData("list", CodeCompletionType.Directive),
			new CodeCompletionData("nolist", CodeCompletionType.Directive),
			new CodeCompletionData("equ", CodeCompletionType.Directive),
			new CodeCompletionData("option", CodeCompletionType.Directive)
		};

		readonly ICompletionData[] _portsList =
		{
			new CodeCompletionData("($00)", CodeCompletionType.Command),
			new CodeCompletionData("($01)", CodeCompletionType.Command),
			new CodeCompletionData("($02)", CodeCompletionType.Command),
			new CodeCompletionData("($03)", CodeCompletionType.Command),
			new CodeCompletionData("($04)", CodeCompletionType.Command),
			new CodeCompletionData("($05)", CodeCompletionType.Command),
			new CodeCompletionData("($06)", CodeCompletionType.Command),
			new CodeCompletionData("($07)", CodeCompletionType.Command),
			new CodeCompletionData("($08)", CodeCompletionType.Command),
			new CodeCompletionData("($09)", CodeCompletionType.Command),
			new CodeCompletionData("($0A)", CodeCompletionType.Command),
			new CodeCompletionData("($0D)", CodeCompletionType.Command),
			new CodeCompletionData("($10)", CodeCompletionType.Command),
			new CodeCompletionData("($11)", CodeCompletionType.Command),
			new CodeCompletionData("($14)", CodeCompletionType.Command),
			new CodeCompletionData("($16)", CodeCompletionType.Command),
			new CodeCompletionData("($18)", CodeCompletionType.Command),
			new CodeCompletionData("($19)", CodeCompletionType.Command),
			new CodeCompletionData("($1A)", CodeCompletionType.Command),
			new CodeCompletionData("($1B)", CodeCompletionType.Command),
			new CodeCompletionData("($1C)", CodeCompletionType.Command),
			new CodeCompletionData("($1D)", CodeCompletionType.Command),
			new CodeCompletionData("($1E)", CodeCompletionType.Command),
			new CodeCompletionData("($1F)", CodeCompletionType.Command),
			new CodeCompletionData("($20)", CodeCompletionType.Command),
			new CodeCompletionData("($21)", CodeCompletionType.Command),
			new CodeCompletionData("($22)", CodeCompletionType.Command),
			new CodeCompletionData("($23)", CodeCompletionType.Command),
			new CodeCompletionData("($27)", CodeCompletionType.Command),
			new CodeCompletionData("($28)", CodeCompletionType.Command),
			new CodeCompletionData("($29)", CodeCompletionType.Command),
			new CodeCompletionData("($2A)", CodeCompletionType.Command),
			new CodeCompletionData("($2B)", CodeCompletionType.Command),
			new CodeCompletionData("($2C)", CodeCompletionType.Command),
			new CodeCompletionData("($2E)", CodeCompletionType.Command),
			new CodeCompletionData("($2F)", CodeCompletionType.Command),
			new CodeCompletionData("($30)", CodeCompletionType.Command),
			new CodeCompletionData("($31)", CodeCompletionType.Command),
			new CodeCompletionData("($32)", CodeCompletionType.Command),
			new CodeCompletionData("($33)", CodeCompletionType.Command),
			new CodeCompletionData("($34)", CodeCompletionType.Command),
			new CodeCompletionData("($35)", CodeCompletionType.Command),
			new CodeCompletionData("($36)", CodeCompletionType.Command),
			new CodeCompletionData("($37)", CodeCompletionType.Command),
			new CodeCompletionData("($38)", CodeCompletionType.Command),
			new CodeCompletionData("($40)", CodeCompletionType.Command),
			new CodeCompletionData("($41)", CodeCompletionType.Command),
			new CodeCompletionData("($42)", CodeCompletionType.Command),
			new CodeCompletionData("($43)", CodeCompletionType.Command),
			new CodeCompletionData("($44)", CodeCompletionType.Command),
			new CodeCompletionData("($45)", CodeCompletionType.Command),
			new CodeCompletionData("($46)", CodeCompletionType.Command),
			new CodeCompletionData("($47)", CodeCompletionType.Command),
			new CodeCompletionData("($48)", CodeCompletionType.Command),
			new CodeCompletionData("($4D)", CodeCompletionType.Command),
			new CodeCompletionData("($55)", CodeCompletionType.Command),
			new CodeCompletionData("($56)", CodeCompletionType.Command),
			new CodeCompletionData("($57)", CodeCompletionType.Command),
			new CodeCompletionData("($5B)", CodeCompletionType.Command)
		};

		private void Add16BitRegs(List<ICompletionData> resultList)
		{
			resultList.Add(new CodeCompletionData("bc", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("de", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("hl", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("ix", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("iy", CodeCompletionType.Register));
		}

		private void Add8BitRegs(List<ICompletionData> resultList)
		{
			resultList.Add(new CodeCompletionData("a", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("b", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("d", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("h", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("ixl", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("iyl", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("c", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("e", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("l", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("ixh", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("iyh", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("(hl)", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("(ix+", CodeCompletionType.Register));
			resultList.Add(new CodeCompletionData("(iy+", CodeCompletionType.Register));
		}
		#endregion

		public ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped)
		{
			List<ICompletionData> resultList = new List<ICompletionData>();
			int startOffset = _textEditor.ActiveTextAreaControl.Caret.Offset;
			int lineNumber = _textEditor.Document.GetLineNumberForOffset(startOffset);
			List<FoldMarker> foldings = _textEditor.Document.FoldingManager.GetFoldingsContainsLineNumber(lineNumber);
			bool isInComment = false;
			var options = Settings.Default.CaseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
            foreach (FoldMarker folder in foldings)
            {
                isInComment = folder.InnerText.Contains("#endcomment", options);
            }

			string line = GetLine(startOffset);
			int start = _textEditor.Document.OffsetToPosition(startOffset).Column;
            if (line.Length == 0 || line.Length < start || isInComment || char.IsLetterOrDigit(line[0]))
            {
                return resultList.ToArray();
            }

			switch (charTyped)
			{
				case '#':
					return _preprocessors;
				case '.':
					return _directives;
				default:
				{
					string command = GetOpcodeOrMacro(line);
					string firstArg = GetFirstArg(line, command.Length, charTyped);
                    switch (command.ToLower())
                    {
                        case "ld":
                            if (string.IsNullOrEmpty(firstArg))
                            {
                                Add16BitRegs(resultList);
                                resultList.Add(new CodeCompletionData("sp", CodeCompletionType.Register));
                                Add8BitRegs(resultList);
                                return resultList.ToArray();
                            }
                            switch (firstArg.ToLower())
                            {
                                case "hl":
                                case "de":
                                case "bc":
                                case "iy":
                                case "ix":
                                    AddParserData(resultList);
                                    break;
                                case "sp":
                                    resultList.Add(new CodeCompletionData("hl", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("ix", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("iy", CodeCompletionType.Register));
                                    return resultList.ToArray();
                                case "a":
                                case "b":
                                case "c":
                                case "d":
                                case "e":
                                case "h":
                                case "l":
                                case "(hl)":
                                    if (firstArg == "a")
                                    {
                                        resultList.Add(new CodeCompletionData("i", CodeCompletionType.Register));
                                        resultList.Add(new CodeCompletionData("r", CodeCompletionType.Register));
                                    }
                                    Add8BitRegs(resultList);
                                    break;
                                case "i":
                                case "r":
                                    resultList.Add(new CodeCompletionData("a", CodeCompletionType.Register));
                                    break;
                            }
                            return resultList.ToArray();
                        case "in":
                        case "out":
                            int temp;
                            if (string.IsNullOrEmpty(firstArg))
                            {
                                resultList.Add(new CodeCompletionData("a", CodeCompletionType.Register));
                                resultList.Add(new CodeCompletionData("(C)", CodeCompletionType.Register));
                            }
                            else if (int.TryParse(firstArg, out temp))
                            {
                                resultList.Add(new CodeCompletionData("(C)", CodeCompletionType.Register));
                            }
                            else
                            {
                                return _portsList;
                            }
                            return resultList.ToArray();
                        case "bit":
                        case "set":
                        case "res":
                            if (string.IsNullOrEmpty(firstArg))
                            {
                                resultList.Add(new CodeCompletionData("0", CodeCompletionType.Command));
                                resultList.Add(new CodeCompletionData("1", CodeCompletionType.Command));
                                resultList.Add(new CodeCompletionData("2", CodeCompletionType.Command));
                                resultList.Add(new CodeCompletionData("3", CodeCompletionType.Command));
                                resultList.Add(new CodeCompletionData("4", CodeCompletionType.Command));
                                resultList.Add(new CodeCompletionData("5", CodeCompletionType.Command));
                                resultList.Add(new CodeCompletionData("6", CodeCompletionType.Command));
                                resultList.Add(new CodeCompletionData("7", CodeCompletionType.Command));
                            }
                            else
                            {
                                Add8BitRegs(resultList);
                            }
                            return resultList.ToArray();
                        case "add":
                        case "adc":
                        case "sbc":
                            if (string.IsNullOrEmpty(firstArg))
                            {
                                resultList.Add(new CodeCompletionData("a", CodeCompletionType.Register));
                                resultList.Add(new CodeCompletionData("hl", CodeCompletionType.Register));
                                if (command == "add")
                                {
                                    resultList.Add(new CodeCompletionData("ix", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("iy", CodeCompletionType.Register));
                                }
                            }
                            else
                            {
                                if (firstArg == "hl" || firstArg == "ix" || firstArg == "iy")
                                {
                                    resultList.Add(new CodeCompletionData("bc", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("de", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("hl", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("sp", CodeCompletionType.Register));
                                }
                                else
                                    Add8BitRegs(resultList);
                            }
                            return resultList.ToArray();
                        case "dec":
                        case "inc":
                        case "rlc":
                        case "rl":
                        case "rr":
                        case "rrc":
                        case "sla":
                        case "sll":
                        case "sra":
                        case "srl":
                        // commands that take a register or a number
                        case "cp":
                        case "or":
                        case "xor":
                            Add8BitRegs(resultList);
                            return resultList.ToArray();
                        case "sub":
                            if (string.IsNullOrEmpty(firstArg))
                            {
                                resultList.Add(new CodeCompletionData("a", CodeCompletionType.Register));
                            }
                            else
                            {
                                Add8BitRegs(resultList);
                            }
                            return resultList.ToArray();
                        // 16 bit only
                        case "push":
                        case "pop":
                            resultList.Add(new CodeCompletionData("af", CodeCompletionType.Register));
                            Add16BitRegs(resultList);
                            return resultList.ToArray();
                        // labels/equates and conditions
                        case "call":
                        case "jp":
                        case "jr":
                        case "ret":
                        case "djnz":
                            // possible to have conditions
                            if (command != "djnz" && string.IsNullOrEmpty(firstArg))
                            {
                                resultList.Add(new CodeCompletionData("z", CodeCompletionType.Register));
                                resultList.Add(new CodeCompletionData("nz", CodeCompletionType.Register));
                                resultList.Add(new CodeCompletionData("c", CodeCompletionType.Register));
                                resultList.Add(new CodeCompletionData("nc", CodeCompletionType.Register));
                                if (command != "jr")
                                {
                                    resultList.Add(new CodeCompletionData("p", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("m", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("po", CodeCompletionType.Register));
                                    resultList.Add(new CodeCompletionData("pe", CodeCompletionType.Register));
                                }
                            }
                            if (command == "ret")
                            {
                                return resultList.ToArray();
                            }

                            AddParserData(resultList);
                            return resultList.ToArray();
                        //special cases
                        case "im":
                            resultList.Add(new CodeCompletionData("0", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("1", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("2", CodeCompletionType.Command));
                            return resultList.ToArray();
                        case "ex":
                            resultList.Add(new CodeCompletionData("de,hl", CodeCompletionType.Register));
                            resultList.Add(new CodeCompletionData("af,af'", CodeCompletionType.Register));
                            resultList.Add(new CodeCompletionData("(sp),hl", CodeCompletionType.Register));
                            resultList.Add(new CodeCompletionData("(sp),ix", CodeCompletionType.Register));
                            resultList.Add(new CodeCompletionData("(sp),iy", CodeCompletionType.Register));
                            return resultList.ToArray();
                        case "rst":
                            resultList.Add(new CodeCompletionData("08h", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("10h", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("18h", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("20h", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("28h", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("30h", CodeCompletionType.Command));
                            resultList.Add(new CodeCompletionData("38h", CodeCompletionType.Command));
                            return resultList.ToArray();

                        //all the no argument commands
                        case "ccf":
                        case "cpdr":
                        case "cpd":
                        case "cpir":
                        case "cpi":
                        case "cpl":
                        case "daa":
                        case "di":
                        case "ei":
                        case "exx":
                        case "halt":
                        case "indr":
                        case "ind":
                        case "inir":
                        case "ini":
                        case "lddr":
                        case "ldd":
                        case "ldir":
                        case "ldi":
                        case "neg":
                        case "nop":
                        case "otdr":
                        case "otir":
                        case "outd":
                        case "outi":
                        case "reti":
                        case "retn":
                        case "rla":
                        case "rlca":
                        case "rld":
                        case "rra":
                        case "rrca":
                        case "scf":
                            return resultList.ToArray();
                        default:
                            {
                                var macros = _parserService.GetAllParserData().Where(s => s is IMacro);
                                bool foundMacro = macros.Any(macro => command.StartsWith(macro.Name));

                                if (!foundMacro)
                                {
                                    break;
                                }

                                AddParserData(resultList);
                                return resultList.ToArray();
                            }
                    }
				}
                break;
			}
			return null;//resultList.ToArray();
		}
	}

	enum CodeCompletionType
	{
		Preprocessor,
		Directive,
		Register,
		Label,
		Define,
		Command
	}


	class CodeCompletionData : DefaultCompletionData, ICompletionData
	{
		readonly string _member;
		readonly CodeCompletionType _type;
		string _description;

		public CodeCompletionData(string member, CodeCompletionType type)
			: base(member, null, (int) type)
		{
			_member = member;
			_type = type;
		}
		public CodeCompletionData(string member, CodeCompletionType type, string description)
			: base(member, description, (int) type)
		{
			_member = member;
			_type = type;
			_description = description;
		}

		public override string ToString()
		{
			return _member;
		}

		// DefaultCompletionData.Description is not virtual, but we can reimplement
		// the interface to get the same effect as overriding.
		string ICompletionData.Description
		{
			get
			{
				if (_description == null)
				{
					switch (_type)
					{
						case CodeCompletionType.Preprocessor:
							switch (_member)
							{
								case "define":
									_description = "Defines a macro or label.";
									break;
								case "ifdef":
									_description = "Optionally executes a block of code dependent on if a\nvalue is defined.";
									break;
								case "ifndef":
									_description = "Optionally executes a block of code dependent on if a\nvalue is not defined.";
									break;
								case "if":
									_description = "Optionally executes a block of code dependent on the\nvalue of a given expression.";
									break;
								case "endif":
									_description = "Ends an if statement";
									break;
								case "macro":
									_description = "Start of a macro.";
									break;
								case "endmacro":
									_description = "Ends a macro.";
									break;
								case "comment":
									_description = "Starts a block comment.";
									break;
								case "endcomment":
									_description = "Ends a block comment";
									break;
								case "else":
									_description = "Executes if the if statement was false.";
									break;
								case "undefine":
									_description = "Undefines a label or macro.";
									break;
								case "include":
									_description = "Includes the specified file in the assembly.";
									break;
							}
							break;
						case CodeCompletionType.Directive:
							switch (_member)
							{
								case "db":
									_description = "Allows a value assignment to the byte pointed\nto at the current location.";
									break;
								case "dw":
									_description = "Allows a value assignment to the word pointed\nto at the current location.";
									break;
								case "end":
									_description = "It useless.";
									break;
								case "org":
									_description = "Sets the program counter to the desired value.";
									break;
								case "byte":
									_description = "Allows a value assignment to the byte pointed\nto at the current location.";
									break;
								case "word":
									_description = "Allows a value assignment to the word pointed\nto at the current location.";
									break;
								case "fill":
									_description = "Fills a selected number of object bytes with a fixed value.";
									break;
								case "block":
									_description = "Advances the specified number of bytes without\nassigning values to the skipped over locations.";
									break;
								case "addinstr":
									_description = "Defines additional instructions for use in this assembly.";
									break;
								case "echo":
									_description = "Outputs information to the console.";
									break;
								case "error":
									_description = "Reports a fatal error.";
									break;
								case "list":
									_description = "Turns on writing to the list file.";
									break;
								case "nolist":
									_description = "Turns off writing to the list file.";
									break;
								case "equ":
									_description = "Used to assign values to labels. The labels can then be used in\nexpressions in place of the literal constant.";
									break;
								case "option":
									_description = "The option directive is used to specific how other parts\nof the assembler function. It is possible to specify multiple\noptions with a single directive.";
									break;
							}
							break;
						case CodeCompletionType.Command:
							switch (_member)
							{
								case "00h":
									_description = "Simulates taking all the batteries out of the calculator";
									break;
								case "08h":
									_description = "Execute system routine OP1ToOP2.";
									break;
								case "10h":
									_description = "Execute system routine FindSym.";
									break;
								case "18h":
									_description = "Execute system routine PushRealO1.";
									break;
								case "20h":
									_description = "Execute system routine Mov9ToOP1.";
									break;
								case "28h":
									_description = "Part of the b_call() macro.";
									break;
								case "30h":
									_description = "Execute system routine FPAdd.";
									break;
								case "38h":
									_description = "System interrupt routine.";
									break;
							}
							break;
					}
				}
				return _description;
			}
		}
	}

	class CodeCompletionKeyHandler
	{
		readonly Editor _mainForm;
		readonly TextEditorControl _editor;
        readonly IParserService _parserService;
		CodeCompletionWindow _codeCompletionWindow;

		private CodeCompletionKeyHandler(Editor mainForm, TextEditorControl editor, IParserService parserService)
		{
			_mainForm = mainForm;
			_editor = editor;
            _parserService = parserService;
		}

		public static void Attach(Editor mainForm, TextEditorControl editor, IParserService parserService)
		{
			CodeCompletionKeyHandler h = new CodeCompletionKeyHandler(mainForm, editor, parserService);

			editor.ActiveTextAreaControl.TextArea.KeyEventHandler += h.TextAreaKeyEventHandler;

			// When the editor is disposed, close the code completion window
			editor.Disposed += h.CloseCodeCompletionWindow;
		}

		/// <summary>
		/// Return true to handle the keypress, return false to let the text area handle the keypress
		/// </summary>
		bool TextAreaKeyEventHandler(char key)
		{
			if (_codeCompletionWindow != null)
			{
				// If completion window is open and wants to handle the key, don't let the text area
				// handle it
				if (_codeCompletionWindow.ProcessKeyEvent(key))
					return true;
			}

		    if ((_codeCompletionWindow != null || (!Settings.Default.EnableAutoTrigger || ",( .#\t".IndexOf(key) == -1)) && (key != ' ' || Control.ModifierKeys != Keys.Control))
		    {
		        return Control.ModifierKeys == Keys.Control;
		    }

		    ICompletionDataProvider completionDataProvider = new CodeCompletionProvider(_mainForm, _editor, _parserService);

		    if (Control.ModifierKeys == Keys.Control && _editor.ActiveTextAreaControl.Caret.Offset != 0)
		    {
		        //editor.ActiveTextAreaControl.Caret.Column -= 1;
		        int startOffset = _editor.ActiveTextAreaControl.Caret.Offset;
		        if (startOffset == _editor.Text.Length)
		        {
		            startOffset--;
		        }

		        while (startOffset >= 0 && ",( .#\t\n\r+-*/".IndexOf(_editor.Text[startOffset]) == -1)
		        {
		            startOffset--;
		        }

		        if (startOffset == -1)
		        {
		            return false;
		        }
		        key = _editor.Document.TextContent[startOffset];
		    }
		    _codeCompletionWindow = CodeCompletionWindow.ShowCompletionWindow(
		        _mainForm,					// The parent window for the completion window
		        _editor, 					// The text editor to show the window for
		        _mainForm.Text,		        // Filename - will be passed back to the provider
		        completionDataProvider,		// Provider to get the list of possible completions
		        key							// Key pressed - will be passed to the provider
		        );
		    if (_codeCompletionWindow != null)
		    {
		        // ShowCompletionWindow can return null when the provider returns an empty list
		        _codeCompletionWindow.Closed += CloseCodeCompletionWindow;
		        //codeCompletionWindow.
		    }
		    return Control.ModifierKeys == Keys.Control;
		}

		void CloseCodeCompletionWindow(object sender, EventArgs e)
		{
			if (_codeCompletionWindow == null)
			{
				return;
			}

			_codeCompletionWindow.Closed -= CloseCodeCompletionWindow;
			_codeCompletionWindow.Dispose();
			_codeCompletionWindow = null;
		}
	}
}