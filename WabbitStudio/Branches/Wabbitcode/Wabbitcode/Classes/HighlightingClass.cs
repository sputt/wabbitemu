using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;

using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.Classes
{
    public static class HighlightingClass
    {
        public static void MakeHighlightingFile()
        {
            string lineColor = Settings.Default.lineColor.ToHexString();
            string commentsColor = Settings.Default.commentColor.ToHexString();

            string blockCommentsColor = Settings.Default.blockCommentColor.ToHexString();
            string stringsColor = Settings.Default.stringColor.ToHexString();
            string directivesColor = Settings.Default.directiveColor.ToHexString();
            string preProcessorColor = Settings.Default.preProcessorColor.ToHexString();
            string opcodesColor = Settings.Default.opcodeColor.ToHexString();
            string registersColor = Settings.Default.registerColor.ToHexString();
            string conditionsColor = Settings.Default.conditionColor.ToHexString();
            string labelsColor = Settings.Default.labelColor.ToHexString();
            if (!Settings.Default.commentsEnabled)
            {
                commentsColor = "#000000";
            }

            if (!Settings.Default.blockCommentEnabled)
            {
                blockCommentsColor = "#000000";
            }

            if (!Settings.Default.stringsEnabled)
            {
                stringsColor = "#000000";
            }

            if (!Settings.Default.directivesEnabled)
            {
                directivesColor = "#000000";
            }

            if (!Settings.Default.preProcessorEnabled)
            {
                preProcessorColor = "#000000";
            }

            if (!Settings.Default.opcodesEnabled)
            {
                opcodesColor = "#000000";
            }

            if (!Settings.Default.registersEnabled)
            {
                registersColor = "#000000";
            }

            if (!Settings.Default.conditionsEnabled)
            {
                conditionsColor = "#000000";
            }

            if (!Settings.Default.labelsEnabled)
            {
                labelsColor = "#000000";
            }

            try
            {
                string[] punctuation =
                {
                    "?", ",", "(", ")", "[", "]", "{", "}", "+", "-",
                    "/", "%", "*", "<", ">", "^", "=", "!", "|", "&"
                };
                string[] registers =
                {
                    "a", "b", "c", "d", "e", "f", "h", "l",  "i", "af", "bc", "de", "hl", "ix",
                    "iy", "ixl", "ixh", "iyl", "iyh", "hx", "lx", "ly", "hy", "pc", "sp"
                };
                string[] preprocessor =
                {
                    "#include", "#define", "#ifdef", "#ifndef", "#if", "#else", "#endif",
                    "#defcont", "#undefine", "#macro", "#endmacro", "#region", "#endregion"
                };
                string[] directives =
                {
                    ".db", ".dw", ".end", ".org", ".byte", ".word", ".fill", ".block", ".addinstr",
                    ".echo", ".error", ".list", ".nolist", ".equ", ".option"
                };
                string[] opcodes =
                {
                    "adc", "add", "and", "bit", "call", "ccf", "cpdr", "cpd", "cpir",
                    "cpi", "cpl", "cp", "daa", "dec", "di", "djnz", "ei", "exx", "ex", "halt", "im", "inc",
                    "indr", "ind", "inir", "ini", "in", "jp", "jr", "lddr", "ldd", "ldir", "ldi", "ld", "neg",
                    "nop", "or", "otdr", "otir", "outd", "outi", "out", "pop", "push", "res", "reti", "retn",
                    "ret", "rla", "rlca", "rlc", "rld", "rl", "rra", "rrca", "rrc", "rrd", "rr", "rst", "sbc",
                    "scf", "set", "sla", "sll", "sra", "srl", "sub", "xor"
                };

                XmlTextWriter writer = new XmlTextWriter(Application.UserAppDataPath + "\\Z80Asm.xshd", Encoding.UTF8);
                writer.Formatting = Formatting.Indented;
                writer.WriteStartDocument();
                writer.WriteComment("Z80 Asm syntax highlighting file");
                writer.WriteStartElement("SyntaxDefinition");
                writer.WriteAttributeString("name", "Z80 Assembly");
                writer.WriteAttributeString("extensions", ".asm;.inc;.z80");
                writer.WriteStartElement("Environment");
                writer.WriteStartElement("Custom");
                writer.WriteAttributeString("name", "TypeReference");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#04ABAB");
                writer.WriteEndElement();

                writer.WriteStartElement("Custom");
                writer.WriteAttributeString("name", "UnknownEntity");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#AB0404");
                writer.WriteEndElement();

                writer.WriteStartElement("Default");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "Black");
                writer.WriteAttributeString("bgcolor", "White");
                writer.WriteEndElement();

                writer.WriteStartElement("Selection");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "White");
                writer.WriteAttributeString("bgcolor", "#316AC5");
                writer.WriteEndElement();

                writer.WriteStartElement("VRuler");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#F1EFE2");
                writer.WriteAttributeString("bgcolor", "White");
                writer.WriteEndElement();

                writer.WriteStartElement("InvalidLines");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "Red");
                writer.WriteEndElement();

                writer.WriteStartElement("CaretMarker");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#FFFFCC");
                writer.WriteEndElement();

                writer.WriteStartElement("LineNumbers");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#ACA899");
                writer.WriteAttributeString("bgcolor", "White");
                writer.WriteEndElement();

                writer.WriteStartElement("FoldLine");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "Gray");
                writer.WriteAttributeString("bgcolor", "Black");
                writer.WriteEndElement();

                writer.WriteStartElement("SelectedFoldLine");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "Black");
                writer.WriteEndElement();

                writer.WriteStartElement("EOLMarkers");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#F1EFE2");
                writer.WriteAttributeString("bgcolor", "White");
                writer.WriteEndElement();

                writer.WriteStartElement("SpaceMarkers");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#F1EFE2");
                writer.WriteAttributeString("bgcolor", "White");
                writer.WriteEndElement();

                writer.WriteStartElement("TabMarkers");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "#F1EFE2");
                writer.WriteAttributeString("bgcolor", "White");
                writer.WriteEndElement();

                writer.WriteStartElement("FirstWord");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", labelsColor);
                writer.WriteEndElement();

                writer.WriteEndElement();           // </environment>

                writer.WriteStartElement("Properties");
                writer.WriteStartElement("Property");
                writer.WriteAttributeString("name", "LineComment");
                writer.WriteAttributeString("value", ";");
                writer.WriteEndElement();
                writer.WriteEndElement();

                writer.WriteStartElement("Digits");
                writer.WriteAttributeString("name", "Digits");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("color", "DarkBlue");
                writer.WriteEndElement();

                writer.WriteStartElement("RuleSets");
                writer.WriteStartElement("RuleSet");
                writer.WriteAttributeString("ignorecase", "true");
                writer.WriteAttributeString("escapecharacter", "\\");

                writer.WriteElementString("Delimiters", "&<>~!%^*()-+=|\\/{}[]:;\"' ,	?");

                writer.WriteStartElement("Span");
                writer.WriteAttributeString("name", "DocLineComment");
                writer.WriteAttributeString("rule", "DocCommentSet");
                writer.WriteAttributeString("stopateol", "true");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", commentsColor);
                writer.WriteStartElement("Begin");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "Gray");
                writer.WriteString("///@!/@");
                writer.WriteEndElement();
                writer.WriteEndElement();

                writer.WriteStartElement("Span");
                writer.WriteAttributeString("name", "LineComment");
                writer.WriteAttributeString("stopateol", "true");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", commentsColor);
                writer.WriteElementString("Begin", ";");
                writer.WriteEndElement();

                writer.WriteStartElement("Span");
                writer.WriteAttributeString("name", "BlockComment");
                writer.WriteAttributeString("stopateol", "false");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", commentsColor);
                writer.WriteElementString("Begin", "#comment");
                writer.WriteElementString("End", "#endcomment");
                writer.WriteEndElement();

                writer.WriteStartElement("Span");
                writer.WriteAttributeString("name", "String");
                writer.WriteAttributeString("escapecharacter", "\\");
                writer.WriteAttributeString("stopateol", "true");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", stringsColor);
                writer.WriteElementString("Begin", "\"");
                writer.WriteElementString("End", "\"");
                writer.WriteEndElement();

                writer.WriteStartElement("Span");
                writer.WriteAttributeString("name", "Char");
                writer.WriteAttributeString("escapecharacter", "\\");
                writer.WriteAttributeString("stopateol", "true");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", stringsColor);
                writer.WriteElementString("Begin", "'");
                writer.WriteElementString("End", "'");
                writer.WriteEndElement();

                writer.WriteStartElement("MarkPrevious");
                writer.WriteAttributeString("bold", "true");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "MidnightBlue");
                writer.WriteString("(");
                writer.WriteEndElement();

                writer.WriteStartElement("MarkPrevious");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", labelsColor);
                writer.WriteAttributeString("markmarker", "true");
                writer.WriteString(":");
                writer.WriteEndElement();

                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "Punctuation");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "Black");

                foreach (string punc in punctuation)
                {
                    writer.WriteStartElement("Key");
                    writer.WriteAttributeString("word", punc);
                    writer.WriteEndElement();
                }

                writer.WriteEndElement();           // </KeyWords>

                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "Registers");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", registersColor);
                foreach (string register in registers)
                {
                    writer.WriteStartElement("Key");
                    writer.WriteAttributeString("word", register);
                    writer.WriteEndElement();
                }

                writer.WriteEndElement();

                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "Preprocessor");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", preProcessorColor);
                foreach (string pre in preprocessor)
                {
                    writer.WriteStartElement("Key");
                    writer.WriteAttributeString("word", pre);
                    writer.WriteEndElement();
                }

                writer.WriteEndElement();

                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "Directives");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", directivesColor);
                foreach (string directive in directives)
                {
                    writer.WriteStartElement("Key");
                    writer.WriteAttributeString("word", directive);
                    writer.WriteEndElement();
                }

                writer.WriteEndElement();

                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "Opcodes");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", opcodesColor);
                foreach (string opcode in opcodes)
                {
                    writer.WriteStartElement("Key");
                    writer.WriteAttributeString("word", opcode);
                    writer.WriteEndElement();
                }

                writer.WriteEndElement();

                string[] conditions = { "z", "nz", "c", "nc", "p", "m", "v", "nv", "pe", "po" };
                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "Conditions");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", conditionsColor);
                foreach (string condition in conditions)
                {
                    writer.WriteStartElement("Key");
                    writer.WriteAttributeString("word", condition);
                    writer.WriteEndElement();
                }

                writer.WriteEndElement();
                writer.WriteEndElement();           // </ruleset>

                writer.WriteStartElement("RuleSet");
                writer.WriteAttributeString("name", "DocCommentSet");
                writer.WriteAttributeString("ignorecase", "false");
                writer.WriteElementString("Delimiters", "&<>~!@%^*()-+=|\\#/{}[]:;\"' ,	.?");
                writer.WriteStartElement("Span");
                writer.WriteAttributeString("name", "XmlTag");
                writer.WriteAttributeString("stopateol", "true");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "SystemColors.WindowText");
                writer.WriteElementString("Begin", "<");
                writer.WriteElementString("End", ">");
                writer.WriteEndElement();

                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "ErrorWords");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "SystemColors.WindowText");
                writer.WriteStartElement("Key");
                writer.WriteAttributeString("word", "TODO");
                writer.WriteEndElement();
                writer.WriteStartElement("Key");
                writer.WriteAttributeString("word", "FIXME");
                writer.WriteEndElement();
                writer.WriteEndElement();

                writer.WriteStartElement("KeyWords");
                writer.WriteAttributeString("name", "WarningWords");
                writer.WriteAttributeString("bold", "false");
                writer.WriteAttributeString("italic", "false");
                writer.WriteAttributeString("color", "SystemColors.WindowText");
                writer.WriteStartElement("Key");
                writer.WriteAttributeString("word", "HACK");
                writer.WriteEndElement();
                writer.WriteStartElement("Key");
                writer.WriteAttributeString("word", "UNDONE");
                writer.WriteEndElement();
                writer.WriteEndElement();

                writer.WriteEndElement();           // </RuleSet>
                writer.WriteEndElement();           // </RuleSets>
                writer.WriteEndElement();           // </SyntaxDefinition>
                writer.Flush();
                writer.Close();
            }
            catch (Exception ex)
            {
                Services.DockingService.ShowError("Error creating highlighting file!", ex);
            }

            // Create new provider with the temp directory
            FileSyntaxModeProvider fsmProvider = new FileSyntaxModeProvider(Application.UserAppDataPath);

            // Attach to the text editor
            HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider);
            if (string.IsNullOrEmpty(Settings.Default.externalHighlight))
            {
                return;
            }

            FileSyntaxModeProvider fsmProvider2 = new FileSyntaxModeProvider(Path.GetDirectoryName(Settings.Default.externalHighlight));
            HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider2);
        }
    }
}