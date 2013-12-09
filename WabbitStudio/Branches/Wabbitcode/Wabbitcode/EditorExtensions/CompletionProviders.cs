using System.Linq;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Gui.CompletionWindow;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    internal class PreprocessorCompletionProvider : CompletionProviderBase
    {
        public PreprocessorCompletionProvider(WabbitcodeTextEditor textEditor)
            : base(textEditor)
        {
        }

        public override ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped)
        {
            return StaticCompletionData.Preprocessors;
        }
    }

    internal class DirectiveCompletionProvider : CompletionProviderBase
    {
        private readonly IParserService _parserService;

        public DirectiveCompletionProvider(WabbitcodeTextEditor textEditor)
            : base(textEditor)
        {
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
        }

        public override ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped)
        {
            var dotCommands = _parserService.GetAllParserData().Where(s => (s is IMacro || s is IDefine)
                        && s.Name.StartsWith("."));
            return StaticCompletionData.Directives.Concat(dotCommands.Select(m =>
                            new CodeCompletionData(m.Name, CodeCompletionType.Define, m.Description)))
                            .ToArray();
        }
    }

    internal class CtrlSpaceCompletionProvider : CompletionProviderBase
    {
        private readonly IParserService _parserService;

        private ICompletionData[] GetAllCompletionData
        {
            get
            {
                return _parserService.GetAllParserData().Where(p =>
                {
                    var label = p as Label;
                    return label == null || !label.IsReusable;
                })
                    .Select(s => new CodeCompletionData(s.Name, CodeCompletionType.Command))
                    .Concat(StaticCompletionData.Preprocessors)
                    .Concat(StaticCompletionData.Directives)
                    .Concat(StaticCompletionData.Commands)
                    .ToArray();
            }
        }

        public CtrlSpaceCompletionProvider(WabbitcodeTextEditor textEditor)
            : base(textEditor)
        {
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
        }

        public override ICompletionData[] GenerateCompletionData(string fileName, TextArea textArea, char charTyped)
        {
            string word = TextEditor.GetWordBeforeCaret();

            var z80Data = HandleZ80Command(word);
            if (z80Data != null)
            {
                return z80Data;
            }

            return GetAllCompletionData;
        }

        private ICompletionData[] HandleZ80Command(string word)
        {
            switch (word)
            {
                case "adc":
                case "add":
                case "and":
                case "bit":
                case "call":
                case "ccf":
                case "cp":
                case "cpd":
                case "cpdr":
                case "cpi":
                case "cpir":
                case "cpl":
                case "daa":
                case "dec":
                case "di":
                case "djnz":
                case "ei":
                case "ex":
                case "exx":
                case "halt":
                case "im":
                case "in":
                case "inc":
                case "ind":
                case "indr":
                case "ini":
                case "inir":
                case "jp":
                case "jr":
                case "ld":
                case "ldd":
                case "lddr":
                case "ldi":
                case "ldir":
                case "neg":
                case "nop":
                case "or":
                case "out":
                case "outd":
                case "otdr":
                case "outi":
                case "otir":
                case "pop":
                case "push":
                case "res":
                case "ret":
                case "reti":
                case "retn":
                case "rla":
                case "rl":
                case "rlca":
                case "rlc":
                case "rld":
                case "rra":
                case "rr":
                case "rrca":
                case "rrc":
                case "rrd":
                case "rst":
                case "sbc":
                case "scf":
                case "set":
                case "sla":
                case "sra":
                case "sll":
                case "srl":
                case "sub":
                case "xor":
                    TextEditor.ShowInsightWindow(new Z80InsightProvider());
                    return new ICompletionData[0];
            }
            return null;
        }
    }

    internal static class StaticCompletionData
    {
        public static readonly ICompletionData[] Directives =
		{
			new CodeCompletionData(".db", CodeCompletionType.Directive),
			new CodeCompletionData(".dw", CodeCompletionType.Directive),
			new CodeCompletionData(".end", CodeCompletionType.Directive),
			new CodeCompletionData(".or", CodeCompletionType.Directive),
			new CodeCompletionData(".byte", CodeCompletionType.Directive),
			new CodeCompletionData(".word", CodeCompletionType.Directive),
			new CodeCompletionData(".fill", CodeCompletionType.Directive),
			new CodeCompletionData(".block", CodeCompletionType.Directive),
			new CodeCompletionData(".addinstr", CodeCompletionType.Directive),
			new CodeCompletionData(".echo", CodeCompletionType.Directive),
			new CodeCompletionData(".error", CodeCompletionType.Directive),
			new CodeCompletionData(".list", CodeCompletionType.Directive),
			new CodeCompletionData(".nolist", CodeCompletionType.Directive),
			new CodeCompletionData(".equ", CodeCompletionType.Directive),
			new CodeCompletionData(".option", CodeCompletionType.Directive)
		};

        public static readonly ICompletionData[] Preprocessors =
        {
            new CodeCompletionData("#define", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#ifdef", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#ifndef", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#if", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#endif", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#macro", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#endmacro", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#comment", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#endcomment", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#else", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#undefine", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#include", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#region", CodeCompletionType.Preprocessor),
            new CodeCompletionData("#endregion", CodeCompletionType.Preprocessor)
        };

        public static readonly ICompletionData[] Commands =
        {
            new CodeCompletionData("adc", CodeCompletionType.Command),
            new CodeCompletionData("add", CodeCompletionType.Command),
            new CodeCompletionData("and", CodeCompletionType.Command),
            new CodeCompletionData("bit", CodeCompletionType.Command),
            new CodeCompletionData("call", CodeCompletionType.Command),
            new CodeCompletionData("ccf", CodeCompletionType.Command),
            new CodeCompletionData("cp", CodeCompletionType.Command),
            new CodeCompletionData("cpd", CodeCompletionType.Command),
            new CodeCompletionData("cpdr", CodeCompletionType.Command),
            new CodeCompletionData("cpi", CodeCompletionType.Command),
            new CodeCompletionData("cpir", CodeCompletionType.Command),
            new CodeCompletionData("cpl", CodeCompletionType.Command),
            new CodeCompletionData("daa", CodeCompletionType.Command),
            new CodeCompletionData("dec", CodeCompletionType.Command),
            new CodeCompletionData("di", CodeCompletionType.Command),
            new CodeCompletionData("djnz", CodeCompletionType.Command),
            new CodeCompletionData("ei", CodeCompletionType.Command),
            new CodeCompletionData("ex", CodeCompletionType.Command),
            new CodeCompletionData("exx", CodeCompletionType.Command),
            new CodeCompletionData("halt", CodeCompletionType.Command),
            new CodeCompletionData("im", CodeCompletionType.Command),
            new CodeCompletionData("in", CodeCompletionType.Command),
            new CodeCompletionData("inc", CodeCompletionType.Command),
            new CodeCompletionData("ind", CodeCompletionType.Command),
            new CodeCompletionData("indr", CodeCompletionType.Command),
            new CodeCompletionData("ini", CodeCompletionType.Command),
            new CodeCompletionData("inir", CodeCompletionType.Command),
            new CodeCompletionData("jp", CodeCompletionType.Command),
            new CodeCompletionData("jr", CodeCompletionType.Command),
            new CodeCompletionData("ld", CodeCompletionType.Command),
            new CodeCompletionData("ldd", CodeCompletionType.Command),
            new CodeCompletionData("lddr", CodeCompletionType.Command),
            new CodeCompletionData("ldi", CodeCompletionType.Command),
            new CodeCompletionData("ldir", CodeCompletionType.Command),
            new CodeCompletionData("neg", CodeCompletionType.Command),
            new CodeCompletionData("nop", CodeCompletionType.Command),
            new CodeCompletionData("or", CodeCompletionType.Command),
            new CodeCompletionData("out", CodeCompletionType.Command),
            new CodeCompletionData("outd", CodeCompletionType.Command),
            new CodeCompletionData("otdr", CodeCompletionType.Command),
            new CodeCompletionData("outi", CodeCompletionType.Command),
            new CodeCompletionData("otir", CodeCompletionType.Command),
            new CodeCompletionData("pop", CodeCompletionType.Command),
            new CodeCompletionData("push", CodeCompletionType.Command),
            new CodeCompletionData("res", CodeCompletionType.Command),
            new CodeCompletionData("ret", CodeCompletionType.Command),
            new CodeCompletionData("reti", CodeCompletionType.Command),
            new CodeCompletionData("retn", CodeCompletionType.Command),
            new CodeCompletionData("rla", CodeCompletionType.Command),
            new CodeCompletionData("rl", CodeCompletionType.Command),
            new CodeCompletionData("rlca", CodeCompletionType.Command),
            new CodeCompletionData("rlc", CodeCompletionType.Command),
            new CodeCompletionData("rld", CodeCompletionType.Command),
            new CodeCompletionData("rra", CodeCompletionType.Command),
            new CodeCompletionData("rr", CodeCompletionType.Command),
            new CodeCompletionData("rrca", CodeCompletionType.Command),
            new CodeCompletionData("rrc", CodeCompletionType.Command),
            new CodeCompletionData("rrd", CodeCompletionType.Command),
            new CodeCompletionData("rst", CodeCompletionType.Command),
            new CodeCompletionData("sbc", CodeCompletionType.Command),
            new CodeCompletionData("scf", CodeCompletionType.Command),
            new CodeCompletionData("set", CodeCompletionType.Command),
            new CodeCompletionData("sla", CodeCompletionType.Command),
            new CodeCompletionData("sra", CodeCompletionType.Command),
            new CodeCompletionData("sll", CodeCompletionType.Command),
            new CodeCompletionData("srl", CodeCompletionType.Command),
            new CodeCompletionData("sub", CodeCompletionType.Command),
            new CodeCompletionData("xor", CodeCompletionType.Command)
        };
    }
}