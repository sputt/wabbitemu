using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.Wabbitcode.Properties;
using System.Linq;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	class CodeCompletionProvider : ICompletionDataProvider
	{
		readonly TextEditorControl _textEditor;
        readonly IParserService _parserService;

		public CodeCompletionProvider(TextEditorControl textTextEditor, IParserService parserService)
		{
			_textEditor = textTextEditor;
            _parserService = parserService;
		}

		public ImageList ImageList
		{
			get { return null; }
		}

		public string PreSelection
		{
		    get
		    {
		        Caret caret = _textEditor.ActiveTextAreaControl.Caret;
		        var segment = _textEditor.Document.GetLineSegment(caret.Line);
		        var word = segment.GetWord(caret.Column - 1);
		        return word == null ? string.Empty : word.Word;
		    }
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
			textArea.Refresh();
			return temp;
		}

	    private void AddParserData(List<ICompletionData> resultList)
		{
		    var data = _parserService.GetAllParserData().Where(s => (s is ILabel && !((ILabel) s).IsReusable) || s is IDefine);
		    resultList.AddRange(data.Select(parserData => new CodeCompletionData(parserData.Name, CodeCompletionType.Label, parserData.Description)));
		}

	    #region Predefined Data

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
		    string command = string.Empty;
		    string firstArg = string.Empty;
		    int col = 0;
		    var lineSegment = textArea.Document.GetLineSegment(textArea.Caret.Line);
		    while (col < textArea.Caret.Column)
		    {
		        var word = lineSegment.GetWord(col);

		        col += word.Length;
		    }

			switch (charTyped)
			{
				default:
				{
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
                            /*else
                            {
                                return _portsList;
                            }*/
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
                                resultList = new List<ICompletionData>(macros.Select(m => 
                                    new CodeCompletionData(m.Name, CodeCompletionType.Define, m.Description)));
                                return resultList.ToArray();
                            }
                    }
				}
			}
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
			    if (_description != null)
			    {
			        return _description;
			    }

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
			    return _description;
			}
		}
	}

    internal class Z80CodeCompletionBinding : ICodeCompletionBinding
    {
        public bool HandleKeyPress(WabbitcodeTextEditor editor, char ch)
        {
            if (!Settings.Default.EnableAutoTrigger)
            {
                return false;
            }
            switch (ch)
            {
                case ' ':
                    string word = editor.GetWordBeforeCaret();
                    switch (word.FirstOrDefault())
                    {
                        case '#':
                            editor.ShowInsightWindow(new PreprocessorInsightProvider());
                            break;
                        default:
                            editor.ShowInsightWindow(new Z80InsightProvider());
                            break;
                    }
                    break;
                case ',':
                case '(':
                    editor.ShowInsightWindow(new MacroInsightProvider());
                    break;
                case '#':
                    editor.ShowCompletionWindow(new PreprocessorCompletionProvider(editor), ch);
                    break;
                case '.':
                    editor.ShowCompletionWindow(new DirectiveCompletionProvider(editor), ch);
                    break;
            }

            return false;
        }

        public bool CtrlSpace(WabbitcodeTextEditor editor)
        {
            editor.ShowCompletionWindow(new CtrlSpaceCompletionProvider(editor), '\0');
            return true;
        }
    }
}