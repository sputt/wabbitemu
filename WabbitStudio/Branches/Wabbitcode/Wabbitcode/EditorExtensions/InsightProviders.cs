using System;
using System.Collections.Generic;
using System.Linq;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Gui.InsightWindow;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    internal class MacroInsightProvider : InsightProviderBase
    {
        public override void SetupDataProvider(string fileName, TextArea textArea)
        {
            base.SetupDataProvider(fileName, textArea);

            string word = TextEditor.GetWordBeforeCaret();

            IParserService parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
            var parserData = parserService.GetParserData(word, Settings.Default.CaseSensitive).ToList();
            Data.AddRange(parserData.Select(d =>
            {
                IParserData data = d;
                while (data != null)
                {
                    string description = data.Description;
                    IMacro macro = data as IMacro;
                    if (macro != null)
                    {
                        string argDesc = macro.Arguments.Aggregate(macro.Name + "(", (current, arg) => current + (arg + ", "));
                        if (argDesc.Length > 2)
                        {
                            argDesc = argDesc.Remove(argDesc.Length - 2);
                        }
                        argDesc += ")";
                        return argDesc + "\n" + description;
                    }

                    if (!string.IsNullOrEmpty(data.Description))
                    {
                        return description;
                    }

                    if (d is IDefine)
                    {
                        data = parserService.TryResolveDefine(d as IDefine, Settings.Default.CaseSensitive);
                    }
                    else
                    {
                        break;
                    }
                }

                return null;
            })
            .Where(d => d != null));
        }
    }

    internal class PreprocessorInsightProvider : InsightProviderBase
    {
        public override void SetupDataProvider(string fileName, TextArea textArea)
        {
            base.SetupDataProvider(fileName, textArea);
            string word = TextEditor.GetWordBeforeCaret();
            switch (word)
            {
                case "#comment":
                    Data.Add("#comment\r\nStarts a block comment");
                    break;
                case "#define":
                    Data.Add("#define name\r\nAdds a new define called name with a value of 1");
                    Data.Add("#define name value\r\nAdds a new define called name with value");
                    Data.Add("#define name(arg1, arg2) contents\r\nAdds a new macro called name with args that expands to contents. See also #macro");
                    break;
                case "#defcont":
                    Data.Add("#defcont contents\r\nDeprecated.\r\nUsed to continue a define adding the specified contents");
                    break;
                case "#else":
                    Data.Add("#else\r\nEvaluates code contained in this block if previous if evaluates to false");
                    break;
                case "#endcomment":
                    Data.Add("#endcomment\r\nEnds a block comment");
                    break;
                case "#elif":
                    Data.Add("#elif condition\r\nEvaluates code contained in this block if condition evaluates to true");
                    break;
                case "#endmacro":
                    Data.Add("#endmacro\r\nEnds a macro block");
                    break;
                case "#endregion":
                    Data.Add("#endregion\r\nEnds a region block");
                    break;
                case "#endif":
                    Data.Add("#endif\r\nEnds an if\\elif\\else block");
                    break;
                case "#if":
                    Data.Add("#if condition\r\nEvaluates the code contained within the block if condition is true");
                    break;
                case "#ifdef":
                    Data.Add("#ifdef define\r\nEvaluates the code contained within the block if define is defined");
                    break;
                case "#import":
                    Data.Add("#import \"filename\"\r\nImports binary data directly into the assembled program");
                    break;
                case "#include":
                    Data.Add("#include \"filename\"\r\nIncludes the specified file at this location in the current document");
                    break;
                case "#macro":
                    Data.Add("#macro name(arg1, arg2)\n contents\n#endmacro\r\nDefines a new macro with the given name and arguments." +
                             " The macro is expected to start on the expected to start on the next line and ends when #endmacro is" +
                             "encounterd");
                    break;
                case "#undef":
                case "#undefine":
                    Data.Add("#undefine name\r\nRemoves the value of the define of the given name");
                    break;
                case "#region":
                    Data.Add("#region\r\nStarts a user defined region");
                    Data.Add("#region name\r\nStarts a user defined region of given name");
                    break;
            }
        }
    }

    internal class Z80InsightProvider : InsightProviderBase
    {
        public override void SetupDataProvider(string fileName, TextArea textArea)
        {
            base.SetupDataProvider(fileName, textArea);

            string word = TextEditor.GetWordBeforeCaret();
            GetArgumentsForCommand(word);
        }

        private void GetArgumentsForCommand(string word)
        {
            switch (word)
            {
                case "adc":
                    Data.Add("adc a, r\r\nAdds A to r and carry flag");
                    Data.Add("adc a, #\r\nAdds A to # and carry flag");
                    Data.Add("adc hl, rr\r\nAdds HL to rr and carry flag");
                    break;
                case "add":
                    Data.Add("add a, r\r\nAdds A to r");
                    Data.Add("add a, #\r\nAdds A to #");
                    Data.Add("add hl, rr\r\nAdds HL to rr");
                    break;
                case "and":
                    Data.Add("and r\r\nBitwise ANDs A with r");
                    Data.Add("and #\r\nBitwise ANDs A with #");
                    break;
                case "bit":
                    Data.Add("bit b, r\r\nBitwise ANDs r with 1<<b, discards result");
                    break;
                case "call":
                    Data.Add("call ##\r\nCalls ##");
                    Data.Add("call f, ##\r\nCalls ## if condition f");
                    break;
                case "ccf":
                    Data.Add("ccf\r\nInverts carry flag");
                    break;
                case "cp":
                    Data.Add("cp r\r\nCompares A to r");
                    Data.Add("cp #\r\nCompares A to #");
                    break;
                case "cpd":
                    Data.Add("cpd\r\nCP (HL)\\DEC HL\\DEC BC,PO if BC=0");
                    break;
                case "cpdr":
                    Data.Add("cpdr\r\nCPD until Z or PO");
                    break;
                case "cpi":
                    Data.Add("cpi\r\nCP (HL)\\INC HL\\DEC BC,PO if BC=0");
                    break;
                case "cpir":
                    Data.Add("cpir\r\nCPI until Z or PO");
                    break;
                case "cpl":
                    Data.Add("cpl\r\nInverts all bits of A");
                    break;
                case "daa":
                    Data.Add("daa\r\nAdjusts A after doing BCD calculations");
                    break;
                case "dec":
                    Data.Add("dec r\r\nDecreases r by 1");
                    Data.Add("dec rr\r\nDecreases rr by 1");
                    break;
                case "di":
                    Data.Add("di\r\nDisables interrupts");
                    break;
                case "djnz":
                    Data.Add("djnz #\r\nDecreases B and jumps if NZ");
                    break;
                case "ei":
                    Data.Add("ei\r\nEnables interrupts");
                    break;
                case "ex":
                    Data.Add("ex (sp), hl\r\nSwaps (SP) and HL");
                    Data.Add("ex af, af'\r\nSwaps AF and AF'");
                    Data.Add("ex de, hl\r\nSwaps DE and HL");
                    break;
                case "exx":
                    Data.Add("exx\r\nSwaps BC,DE,HL and BC',DE',HL'");
                    break;
                case "halt":
                    Data.Add("halt\r\nStops and waits for interrupt");
                    break;
                case "im":
                    Data.Add("im 0\r\nSets interrupt mode 0");
                    Data.Add("im 1\r\nSets interrupt mode 1");
                    Data.Add("im 2\r\nSets interrupt mode 2");
                    break;
                case "in":
                    Data.Add("in a, (#)\r\nPuts input from port # into A");
                    Data.Add("in r, (c)\r\nPuts input from port C into r");
                    Data.Add("in (c), *\r\nPuts input from port C nowhere");
                    break;
                case "inc":
                    Data.Add("inc r\r\nIncreases r by 1");
                    Data.Add("inc rr\r\nIncreases rr by 1");
                    break;
                case "ind":
                    Data.Add("ind\r\nIN (HL),(C)\\DEC HL\\DEC B");
                    break;
                case "indr":
                    Data.Add("indr\r\nIND until Z");
                    break;
                case "ini":
                    Data.Add("ini\r\nIN (HL),(C)\\INC HL\\DEC B");
                    break;
                case "inir":
                    Data.Add("inir\r\nINI until Z");
                    break;
                case "jp":
                    Data.Add("jp (hl)\r\nJumps to HL");
                    Data.Add("jp ##\r\nJumps to ##");
                    Data.Add("jp z, ##\r\nJumps to ## if condition zero flag set");
                    Data.Add("jp nz, ##\r\nJumps to ## if condition zero flag reset");
                    Data.Add("jp c, ##\r\nJumps to ## if condition carry flag set");
                    Data.Add("jp nc, ##\r\nJumps to ## if condition carry flag reset");
                    Data.Add("jp m, ##\r\nJumps to ## if condition sign flag set");
                    Data.Add("jp p, ##\r\nJumps to ## if condition p flag reset");
                    Data.Add("jp pe, ##\r\nJumps to ## if condition p/v flag set");
                    Data.Add("jp po, ##\r\nJumps to ## if condition p/v flag reset");
                    break;
                case "jr":
                    Data.Add("jr #\r\nJumps # (signed) bytes");
                    Data.Add("jr z, #\r\nJumps # (signed) bytes if zero flag set");
                    Data.Add("jr nz, #\r\nJumps # (signed) bytes if zero flag reset");
                    Data.Add("jr c, #\r\nJumps # (signed) bytes if carry flag set");
                    Data.Add("jr nc, #\r\nJumps # (signed) bytes if carry flag reset");
                    break;
                case "ld":
                    Data.Add("ld (bc), a\r\nLoads A into (BC)");
                    Data.Add("ld (de), a\r\nLoads A into (DE)");
                    Data.Add("ld (##), a\r\nLoads A into (##)");
                    Data.Add("ld (##), bc\r\nLoads BC into (##)");
                    Data.Add("ld (##), de\r\nLoads DE into (##)");
                    Data.Add("ld (##), hl\r\nLoads HL into (##)");
                    Data.Add("ld (##), sp\r\nLoads SP into (##)");
                    Data.Add("ld a, (bc)\r\nLoads (BC) into A");
                    Data.Add("ld a, (de)\r\nLoads (DE) into A");
                    Data.Add("ld a, (##)\r\nLoads (##) into A");
                    Data.Add("ld a, i\r\nLoads I into A");
                    Data.Add("ld a, r\r\nLoads R into A");
                    Data.Add("ld bc, (##)\r\nLoads (##) into BC");
                    Data.Add("ld de, (##)\r\nLoads (##) into DE");
                    Data.Add("ld hl, (##)\r\nLoads (##) into HL");
                    Data.Add("ld i, a\r\nLoads A into I");
                    Data.Add("ld r, a\r\nLoads A into R");
                    Data.Add("ld sp, (##)\r\nLoads (##) into SP");
                    Data.Add("ld sp, hl\r\nLoads HL into SP");
                    Data.Add("ld r1, r2\r\nLoads r2 into r1");
                    Data.Add("ld r, #\r\nLoads # into r");
                    Data.Add("ld rr, ##\r\nLoads ## into rr");
                    break;
                case "ldd":
                    Data.Add("ldd\r\nLD (DE),(HL)\\DEC HL\\DEC DE\\DEC BC,PO if BC=0");
                    break;
                case "lddr":
                    Data.Add("lddr\r\nLDD until PO");
                    break;
                case "ldi":
                    Data.Add("ldi\r\nLD (DE),(HL)\\INC HL\\INC DE\\DEC BC,PO if BC=0");
                    break;
                case "ldir":
                    Data.Add("ldir\r\nLDI until PO");
                    break;
                case "neg":
                    Data.Add("neg\r\nNegates A");
                    break;
                case "nop":
                    Data.Add("nop\r\nDoes nothing");
                    break;
                case "or":
                    Data.Add("or r\r\nBitwise ORs A with r");
                    Data.Add("or #\r\nBitwise ORs A with #");
                    break;
                case "out":
                    Data.Add("out (#), A\r\nOutputs A to port #");
                    Data.Add("out (C), r\r\nOutputs r to port C");
                    Data.Add("out (C), 0*\r\nOutputs 0 to port C");
                    break;
                case "outd":
                    Data.Add("outd\r\nOUT (C),(HL)\\DEC HL\\DEC B");
                    break;
                case "otdr":
                    Data.Add("otdr\r\nOUTD until Z");
                    break;
                case "outi":
                    Data.Add("outi\r\nOUT (C),(HL)\\INC HL\\DEC B");
                    break;
                case "otir":
                    Data.Add("otir\r\nOUTI until Z");
                    break;
                case "pop":
                    Data.Add("pop rr\r\nPops rr off the stack");
                    break;
                case "push":
                    Data.Add("push rr\r\nPushes rr onto the stack");
                    break;
                case "res":
                    Data.Add("res b,r\r\nResets bth bit of r");
                    break;
                case "ret":
                    Data.Add("ret\r\nReturns");
                    Data.Add("ret f\r\nReturns if condition f");
                    break;
                case "reti":
                    Data.Add("reti\r\nReturn from interrupt");
                    break;
                case "retn":
                    Data.Add("retn\r\nReturn from non-maskable interrupt");
                    break;
                case "rla":
                    Data.Add("rla\r\nRotates A left with carry");
                    break;
                case "rl":
                    Data.Add("rl r\r\nRotates r left with carry");
                    break;
                case "rlca":
                    Data.Add("rlca\r\nRotates A left with wrap");
                    break;
                case "rlc":
                    Data.Add("rlc r\r\nRotates r left with wrap");
                    break;
                case "rld":
                    Data.Add("rld\r\nRL (HL) with nibbles,LSN of A acts as carry");
                    break;
                case "rra":
                    Data.Add("rra\r\nRotates A right with carry");
                    break;
                case "rr":
                    Data.Add("rr r\r\nRotates r right with carry");
                    break;
                case "rrca":
                    Data.Add("rrca\r\nRotates A right with wrap");
                    break;
                case "rrc":
                    Data.Add("rrc r\r\nRotates r right with wrap");
                    break;
                case "rrd":
                    Data.Add("rrd\r\nRR (HL) with nibbles,LSN of A acts as carry");
                    break;
                case "rst":
                    Data.Add("rst 00h\r\nCalls $0000");
                    Data.Add("rst 08h\r\nCalls $0008");
                    Data.Add("rst 10h\r\nCalls $0010");
                    Data.Add("rst 18h\r\nCalls $0018");
                    Data.Add("rst 20h\r\nCalls $0020");
                    Data.Add("rst 28h\r\nCalls $0028");
                    Data.Add("rst 30h\r\nCalls $0030");
                    Data.Add("rst 38h\r\nCalls $0038");
                    break;
                case "sbc":
                    Data.Add("sbc r\r\nSubtracts r with carry from A ");
                    Data.Add("sbc #\r\nSubtracts # with carry from A");
                    Data.Add("sbc hl, rr\r\nSubtracts rr with carry from HL");
                    break;
                case "scf":
                    Data.Add("scf\r\nSets carry flag");
                    break;
                case "set":
                    Data.Add("set b, r\r\nSets bth bit of r");
                    break;
                case "sla":
                    Data.Add("sla r\r\nShifts r left, bottom bit 0");
                    break;
                case "sra":
                    Data.Add("sra r\r\nShifts r right, top bit = previous top bit");
                    break;
                case "sll":
                    Data.Add("sll r\r\nShifts r left, bottom bit 1");
                    break;
                case "srl":
                    Data.Add("srl r\r\nShifts r right, top bit 0");
                    break;
                case "sub":
                    Data.Add("sub r\r\nSubtracts r from A");
                    Data.Add("sub #\r\nSubtracts # from A");
                    break;
                case "xor":
                    Data.Add("xor r\r\nBitwise XORs A with r");
                    Data.Add("xor #\r\nBitwise XORs A with #");
                    break;
            }
        }
    }

    internal abstract class InsightProviderBase : IInsightDataProvider
    {
        private int _initialOffset;
        protected WabbitcodeTextEditor TextEditor;
        private TextArea _textArea;
        protected List<string> Data;

        public int InsightDataCount
        {
            get
            {
                return Data.Count;
            }
        }

        public int DefaultIndex
        {
            get
            {
                return 0;
            }
        }

        public virtual void SetupDataProvider(string fileName, TextArea textArea)
        {
            _textArea = textArea;
            TextEditor = textArea.MotherTextEditorControl as WabbitcodeTextEditor;
            _initialOffset = textArea.Caret.Offset;
            Data = new List<string>();
        }

        public virtual bool CaretOffsetChanged()
        {
            var segment = _textArea.Document.GetLineSegmentForOffset(_initialOffset);
            int endOffset = segment.Offset + segment.Length;
            bool closeDataProvider = _textArea.Caret.Offset <= _initialOffset || _textArea.Caret.Offset > endOffset;
            int brackets = 0;
            if (closeDataProvider)
            {
                return true;
            }

            bool insideChar = false;
            bool insideString = false;
            for (int offset = _initialOffset; offset < Math.Min(_textArea.Caret.Offset,
                _textArea.Document.TextLength); ++offset)
            {
                char ch = _textArea.Document.GetCharAt(offset);
                switch (ch)
                {
                    case '\'':
                        insideChar = !insideChar;
                        break;
                    case '(':
                        if (!(insideChar || insideString))
                        {
                            ++brackets;
                        }
                        break;
                    case ')':
                        if (!(insideChar || insideString))
                        {
                            --brackets;
                        }

                        if (brackets <= 0)
                        {
                            return true;
                        }
                        break;
                    case '"':
                        insideString = !insideString;
                        break;
                    case ';':
                        return !insideString;
                    case '\n':
                        return false;
                }
            }

            return false;
        }

        public string GetInsightData(int number)
        {
            return Data[number];
        }
    }
}
