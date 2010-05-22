using System.Drawing;
using System.IO;
using System.Windows.Forms;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;
using System;

namespace Revsoft.Wabbitcode.Classes
{
    class HighlightingClass
    {
        public void makeHighlightingFile()
        {
            //HACK:this is the biggest/worst hack ever...fixed a little. Still pretty bad
            //the new way doesnt work?!? WHY THE FUCK NOT!?

            #region syntaxFilenew

            //        string lineColor = ColorToHexString(Properties.Settings.Default.lineColor);
            //        string commentsColor = ColorToHexString(Properties.Settings.Default.commentColor);
            //        string blockCommentsColor = ColorToHexString(Properties.Settings.Default.blockCommentColor);
            //        string stringsColor = ColorToHexString(Properties.Settings.Default.stringColor);
            //        string directivesColor = ColorToHexString(Properties.Settings.Default.directiveColor);
            //        string preProcessorColor = ColorToHexString(Properties.Settings.Default.preProcessorColor);
            //        string opcodesColor = ColorToHexString(Properties.Settings.Default.commandColor);
            //        string registersColor = ColorToHexString(Properties.Settings.Default.registerColor);
            //        string conditionsColor = ColorToHexString(Properties.Settings.Default.conditionColor);
            //        string labelsColor = ColorToHexString(Properties.Settings.Default.labelColor);
            //        if (!Properties.Settings.Default.commentsEnabled)
            //            commentsColor = "000000";
            //        if (!Properties.Settings.Default.blockCommentEnabled)
            //            blockCommentsColor = "000000";
            //        if (!Properties.Settings.Default.stringsEnabled)
            //            stringsColor = "000000";
            //        if (!Properties.Settings.Default.directivesEnabled)
            //            directivesColor = "000000";
            //        if (!Properties.Settings.Default.preProcessorEnabled)
            //            preProcessorColor = "000000";
            //        if (!Properties.Settings.Default.commandEnabled)
            //            opcodesColor = "000000";
            //        if (!Properties.Settings.Default.registersEnabled)
            //            registersColor = "000000";
            //        if (!Properties.Settings.Default.conditionsEnabled)
            //            conditionsColor = "000000";
            //        if (!Properties.Settings.Default.labelsEnabled)
            //            labelsColor = "000000";

            //        string file = "<?xml version=\"1.0\" encoding=\"utf-8\"?>" +
            //            "\n<SyntaxDefinition name=\"Z80 Assembly\" extensions=\".asm;.inc;.z80\">" +
            //            "\n\t<Environment>" +
            //            "\n<Custom name=\"TypeReference\" bold=\"false\" italic=\"false\" color=\"#04ABAB\" />" +
            //            "\n\t\t<Custom name=\"UnknownEntity\" bold=\"false\" italic=\"false\" color=\"#AB0404\" />" +
            //            "\n\t\t<Default bold=\"false\" italic=\"false\" color=\"Black\" bgcolor=\"White\" /> " +
            //            "\n\t\t<Selection bold=\"false\" italic=\"false\" color=\"White\" bgcolor=\"#316AC5\" />" +
            //            "\n\t\t<VRuler bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
            //            "\n\t\t<InvalidLines bold=\"false\" italic=\"false\" color=\"Red\" />" +
            //            "\n\t\t<CaretMarker bold=\"false\" italic=\"false\" color=\"#" + lineColor + "\" />" +
            //            "\n\t\t<LineNumbers bold=\"false\" italic=\"false\" color=\"#ACA899\" bgcolor=\"White\" />" +
            //            "\n\t\t<FoldLine bold=\"false\" italic=\"false\" color=\"Gray\" bgcolor=\"Black\" />" +
            //            "\n\t\t<FoldMarker bold=\"false\" italic=\"false\" color=\"Gray\" bgcolor=\"White\" />" +
            //            "\n\t\t<SelectedFoldLine bold=\"false\" italic=\"false\" color=\"Black\" />" +
            //            "\n\t\t<EOLMarkers bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
            //            "\n\t\t<SpaceMarkers bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
            //            "\n\t\t<TabMarkers bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
            //            "\n\t</Environment>" +
            //            "\n\t<Properties>" +
            //            "\n\t\t<Property name=\"LineComment\" value=\";\" />" +
            //            "\n\t</Properties>" +
            //            "\n\t<Digits name=\"Digits\" bold=\"false\" italic=\"false\" color=\"DarkBlue\" />" +
            //            "\n\t<RuleSets>" +
            //            "\n\t<RuleSet ignorecase=\"true\" escapecharacter=\"\\\">" +
            //            "\n\t<Delimiters>&amp;&lt;&gt;~!%^*()-+=|\\/{}[]:;\"' ,	?</Delimiters>" +
            //            //comments
            //            "\n\t<Span name=\"DocLineComment\" rule=\"DocCommentSet\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"Green\">" +
            //            "\n\t\t<Begin bold=\"false\" italic=\"false\" color=\"Gray\">///@!/@</Begin>" +
            //            "\n\t</Span>" +
            //            "\n\t<Span name=\"LineComment\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"#" + commentsColor + "\">" +
            //            "\n\t\t<Begin>;</Begin>" +
            //            "\n\t</Span>" +
            //            //block comments
            //            "\n\t<Span name=\"BlockComment\" stopateol=\"false\" bold=\"false\" italic=\"false\" color=\"#" + blockCommentsColor + "\">" +
            //            "\n\t\t<Begin>#comment</Begin>" +
            //            "\n\t\t<End>#endcomment</End>" +
            //            "\n\t</Span>" +
            //            //strings
            //            "\n\t<Span name=\"String\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"#" + stringsColor + "\">" +
            //            "\n\t\t<Begin>\"</Begin>" +
            //            "\n\t\t<End>\"</End>" +
            //            "\n\t</Span>" +
            //            "\n\t<Span name=\"Char\" escapecharacter=\"\\\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"#" + stringsColor + "\">" +
            //            "\n\t\t<Begin>'</Begin>" +
            //            "\n\t\t<End>'</End>" +
            //            "\n\t</Span>" +
            //            "\n\t<MarkPrevious bold=\"true\" italic=\"false\" color=\"MidnightBlue\">(</MarkPrevious>" +
            //            "\n\t<MarkPrevious bold=\"false\" italic=\"false\" color=\"#" + labelsColor + "\" markmarker=\"true\">:</MarkPrevious>" +
            //            //punctuation
            //            "\n\t<KeyWords name=\"Punctuation\" bold=\"false\" italic=\"false\" color=\"Black\">";
            //        string[] punctuation = { "?", ",", ";", "(", ")", "[", "]", "{", "}", "+", "-", "/", "&", "&lt;",
            //                                   "&gt;", "^", "=", "~", "!", "|", "&amp;" };
            //        foreach (string keyword in punctuation)
            //        {
            //            file += "\n\t\t<Key word=\"" + keyword + "\" />";
            //        }
            //        file += "\n\t</KeyWords>\n\t<KeyWords name=\"Registers\" bold=\"false\" italic=\"false\" color=\"#" + registersColor + "\">";
            //        #region punctuation
            //        //"\n\t\t<Key word=\"?\" />" +
            //        //        "\n\t\t<Key word=\",\" />" +
            //        //        "\n\t\t<Key word=\";\" />" +
            //        //        "\n\t\t<Key word=\"(\" />" +
            //        //        "\n\t\t<Key word=\")\" />" +
            //        //        "\n\t\t<Key word=\"[\" />" +
            //        //        "\n\t\t<Key word=\"]\" />" +
            //        //        "\n\t\t<Key word=\"{\" />" +
            //        //        "\n\t\t<Key word=\"}\" />" +
            //        //        "\n\t\t<Key word=\"+\" />" +
            //        //        "\n\t\t<Key word=\"-\" />" +
            //        //        "\n\t\t<Key word=\"/\" />" +
            //        //        "\n\t\t<Key word=\"%\" />" +
            //        //        "\n\t\t<Key word=\"*\" />" +
            //        //        "\n\t\t<Key word=\"&lt;\" />" +
            //        //        "\n\t\t<Key word=\"&gt;\" />" +
            //        //        "\n\t\t<Key word=\"^\" />" +
            //        //        "\n\t\t<Key word=\"=\" />" +
            //        //        "\n\t\t<Key word=\"~\" />" +
            //        //        "\n\t\t<Key word=\"!\" />" +
            //        //        "\n\t\t<Key word=\"|\" />" +
            //        //        "\n\t\t<Key word=\"&amp;\" />" +
            //        #endregion
            //        string[] registers = { "a", "b", "c", "d", "e", "f", "h", "l", "i", "af", "bc", "de", "hl", "ix",
            //                                 "iy", "ixl", "ixh", "iyl", "iyh", "hx", "hy", "lx", "ly", "r", "pc" };
            //        foreach(string keyword in registers)
            //        {
            //            file += "\n\t\t<Key word=\"" + keyword + "\" />";
            //        }
            //        file += "\n\t</KeyWords>\n\t<KeyWords name=\"Preprocessor\" bold=\"false\" italic=\"false\" color=\"#" + preProcessorColor + "\">";
            //        #region regs
            //            //"\n\t\t<Key word=\"a\" />" +
            //            //"\n\t\t<Key word=\"b\" />" +
            //            //"\n\t\t<Key word=\"c\" />" +
            //            //"\n\t\t<Key word=\"d\" />" +
            //            //"\n\t\t<Key word=\"e\" />" +
            //            //"\n\t\t<Key word=\"f\" />" +
            //            //"\n\t\t<Key word=\"h\" />" +
            //            //"\n\t\t<Key word=\"l\" />" +
            //            //"\n\t\t<Key word=\"i\" />" +
            //            //"\n\t\t<Key word=\"af\" />" +
            //            //"\n\t\t<Key word=\"bc\" />" +
            //            //"\n\t\t<Key word=\"de\" />" +
            //            //"\n\t\t<Key word=\"hl\" />" +
            //            //"\n\t\t<Key word=\"ix\" />" +
            //            //"\n\t\t<Key word=\"iy\" />" +
            //            //"\n\t\t<Key word=\"ixl\" />" +
            //            //"\n\t\t<Key word=\"ixh\" />" +
            //            //"\n\t\t<Key word=\"iyl\" />" +
            //            //"\n\t\t<Key word=\"iyh\" />" +
            //            //"\n\t\t<Key word=\"hx\" />" +
            //            //"\n\t\t<Key word=\"lx\" />" +
            //            //"\n\t\t<Key word=\"ly\" />" +
            //            //"\n\t\t<Key word=\"hy\" />" +
            //            //"\n\t\t<Key word=\"pc\" />" + 
            //#endregion
            //        string[] preprocessor = {"#define", "#ifdef", "#ifndef", "#if", "#endif", "#macro", "#endmacro", "#comment",
            //                                    "#endcomment", "#else", "#undefine", "#include"};
            //        foreach(string keyword in preprocessor)
            //        {
            //            file += "\n\t\t<Key word=\"" + keyword + "\" />";
            //        }
            //        file += "\n\t</KeyWords>\n\t<KeyWords name=\"Directives\" bold=\"false\" italic=\"false\" color=\"#" + directivesColor + "\">";
            //        #region preprocessor
            //                    //"\n\t\t<Key word=\"#include\" />" +
            //            //"\n\t\t<Key word=\"#define\" />" +
            //            //"\n\t\t<Key word=\"#ifdef\" />" +
            //            //"\n\t\t<Key word=\"#ifndef\" />" +
            //            //"\n\t\t<Key word=\"#if\" />" +
            //            //"\n\t\t<Key word=\"#else\" />" +
            //            //"\n\t\t<Key word=\"#endif\" />" +
            //            //"\n\t\t<Key word=\"#defcont\" />" +
            //            //"\n\t\t<Key word=\"#undefine\" />" +
            //            //"\n\t\t<Key word=\"#macro\" />" +
            //            //"\n\t\t<Key word=\"#endmacro\" />" + 
            //#endregion
            //        string[] directives = {".db", ".dw", ".end", ".org", ".byte", ".word", ".fill", ".block", ".addinstr",
            //                                ".echo", ".error", ".list", ".nolist", ".equ", ".option"};
            //        foreach(string keyword in directives)
            //        {
            //            file += "\n\t\t<Key word=\"" + keyword + "\" />";
            //        }
            //        file += "\n\t</KeyWords>\n\t<KeyWords name=\"Opcodes\" bold=\"false\" italic=\"false\" color=\"#" + opcodesColor + "\">";

            //        #region directives
            //                    //"\n\t\t<Key word=\".org\" />" +
            //            //"\n\t\t<Key word=\".db\" />" +
            //            //"\n\t\t<Key word=\".dw\" />" +
            //            //"\n\t\t<Key word=\".echo\" />" +
            //            //"\n\t\t<Key word=\".list\" />" +
            //            //"\n\t\t<Key word=\".nolist\" />" +
            //            //"\n\t\t<Key word=\".end\" />" +
            //            //"\n\t\t<Key word=\".fill\" />" +
            //            //"\n\t\t<Key word=\".block\" />" +
            //            //"\n\t\t<Key word=\".addinstr\" />" +
            //            //"\n\t\t<Key word=\".word\" />" +
            //            //"\n\t\t<Key word=\".byte\" />" +
            //            //"\n\t\t<Key word=\".show\" />" +
            //            //"\n\t\t<Key word=\".error\" />" +
            //            //"\n\t\t<Key word=\".seek\" />" + 
            //#endregion
            //        string[] opcodes = {"adc", "add", "and", "bit", "call", "ccf", "cpdr", "cpd", "cpir",
            //                "cpi", "cpl", "cp", "daa", "dec", "di", "djnz", "ei", "exx", "ex", "halt", "im", "inc",
            //                "indr", "ind", "inir", "ini", "in", "jp", "jr", "lddr", "ldd", "ldir", "ldi", "ld", "neg",
            //                "nop", "or", "otdr", "otir", "outd", "outi", "out", "pop", "push", "res", "reti", "retn",
            //                "ret", "rla", "rlca", "rlc", "rld", "rl", "rra", "rrca", "rrc", "rrd", "rr", "rst", "sbc",
            //                "scf", "set", "sla", "sll", "sra", "srl", "sub", "xor"};
            //        foreach(string keyword in opcodes)
            //        {
            //            file += "\n\t\t<Key word=\"" + keyword + "\" />";
            //        }
            //        file += "\n\t</KeyWords>\n\t<KeyWords name=\"Conditions\" bold=\"false\" italic=\"false\" color=\"#" + conditionsColor + "\">";
            //        #region opcodes
            //                    //"\n\t\t<Key word=\"adc\" />" +
            //            //"\n\t\t<Key word=\"add\" />" +
            //            //"\n\t\t<Key word=\"and\" />" +
            //            //"\n\t\t<Key word=\"bit\" />" +
            //            //"\n\t\t<Key word=\"call\" />" +
            //            //"\n\t\t<Key word=\"ccf\" />" +
            //            //"\n\t\t<Key word=\"cpdr\" />" +
            //            //"\n\t\t<Key word=\"cpd\" />" +
            //            //"\n\t\t<Key word=\"cpir\" />" +
            //            //"\n\t\t<Key word=\"cpi\" />" +
            //            //"\n\t\t<Key word=\"cpl\" />" +
            //            //"\n\t\t<Key word=\"cp\" />" +
            //            //"\n\t\t<Key word=\"daa\" />" +
            //            //"\n\t\t<Key word=\"dec\" />" +
            //            //"\n\t\t<Key word=\"di\" />" +
            //            //"\n\t\t<Key word=\"djnz\" />" +
            //            //"\n\t\t<Key word=\"ei\" />" +
            //            //"\n\t\t<Key word=\"exx\" />" +
            //            //"\n\t\t<Key word=\"ex\" />" +
            //            //"\n\t\t<Key word=\"halt\" />" +
            //            //"\n\t\t<Key word=\"im\" />" +
            //            //"\n\t\t<Key word=\"inc\" />" +
            //            //"\n\t\t<Key word=\"indr\" />" +
            //            //"\n\t\t<Key word=\"ind\" />" +
            //            //"\n\t\t<Key word=\"inir\" />" +
            //            //"\n\t\t<Key word=\"ini\" />" +
            //            //"\n\t\t<Key word=\"in\" />" +
            //            //"\n\t\t<Key word=\"jp\" />" +
            //            //"\n\t\t<Key word=\"jr\" />" +
            //            //"\n\t\t<Key word=\"lddr\" />" +
            //            //"\n\t\t<Key word=\"ldd\" />" +
            //            //"\n\t\t<Key word=\"ldir\" />" +
            //            //"\n\t\t<Key word=\"ldi\" />" +
            //            //"\n\t\t<Key word=\"ld\" />" +
            //            //"\n\t\t<Key word=\"neg\" />" +
            //            //"\n\t\t<Key word=\"nop\" />" +
            //            //"\n\t\t<Key word=\"or\" />" +
            //            //"\n\t\t<Key word=\"otdr\" />" +
            //            //"\n\t\t<Key word=\"otir\" />" +
            //            //"\n\t\t<Key word=\"outd\" />" +
            //            //"\n\t\t<Key word=\"outi\" />" +
            //            //"\n\t\t<Key word=\"out\" />" +
            //            //"\n\t\t<Key word=\"pop\" />" +
            //            //"\n\t\t<Key word=\"push\" />" +
            //            //"\n\t\t<Key word=\"res\" />" +
            //            //"\n\t\t<Key word=\"reti\" />" +
            //            //"\n\t\t<Key word=\"retn\" />" +
            //            //"\n\t\t<Key word=\"ret\" />" +
            //            //"\n\t\t<Key word=\"rla\" />" +
            //            //"\n\t\t<Key word=\"rlca\" />" +
            //            //"\n\t\t<Key word=\"rlc\" />" +
            //            //"\n\t\t<Key word=\"rld\" />" +
            //            //"\n\t\t<Key word=\"rl\" />" +
            //            //"\n\t\t<Key word=\"rra\" />" +
            //            //"\n\t\t<Key word=\"rrca\" />" +
            //            //"\n\t\t<Key word=\"rrc\" />" +
            //            //"\n\t\t<Key word=\"rrd\" />" +
            //            //"\n\t\t<Key word=\"rr\" />" +
            //            //"\n\t\t<Key word=\"rst\" />" +
            //            //"\n\t\t<Key word=\"sbc\" />" +
            //            //"\n\t\t<Key word=\"scf\" />" +
            //            //"\n\t\t<Key word=\"set\" />" +
            //            //"\n\t\t<Key word=\"sla\" />" +
            //            //"\n\t\t<Key word=\"sll\" />" +
            //            //"\n\t\t<Key word=\"sra\" />" +
            //            //"\n\t\t<Key word=\"srl\" />" +
            //            //"\n\t\t<Key word=\"sub\" />" +
            //            //"\n\t\t<Key word=\"xor\" />" + 
            //#endregion
            //        string[] conditions = { "z", "nz", "c", "nc", "p", "m", "v", "nv", "pe", "po" };
            //        foreach (string keyword in conditions)
            //        {
            //            file += "\n\t\t<Key word=\"" + keyword + "\" />";
            //        }
            //        #region conditions
            //                    //"\n\t\t<Key word=\"z\" />" +
            //            //"\n\t\t<Key word=\"nz\" />" +
            //            //"\n\t\t<Key word=\"c\" />" +
            //            //"\n\t\t<Key word=\"nc\" />" +
            //            //"\n\t\t<Key word=\"p\" />" +
            //            //"\n\t\t<Key word=\"m\" />" +
            //            //"\n\t\t<Key word=\"v\" />" +
            //            //"\n\t\t<Key word=\"nv\" />" +
            //            //"\n\t\t<Key word=\"pe\" />" +
            //            //"\n\t\t<Key word=\"po\" />" + 
            //#endregion
            //        file += "\n\t</KeyWords>" +
            //            "\n\t</RuleSet>" +
            //            "\n\t<RuleSet name=\"DocCommentSet\" ignorecase=\"false\">" +
            //            "\n\t\t<Delimiters>&amp;&lt;&gt;~!@%^*()-+=|\\#/{}[]:;\"' ,	.?</Delimiters>" +
            //            "\n\t<Span name=\"XmlTag\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"SystemColors.WindowText\">" +
            //            "\n\t\t<Begin>&lt;</Begin>" +
            //            "\n\t\t<End>&gt;</End>" +
            //            "\n\t</Span>" +
            //            "\n\t<KeyWords name=\"ErrorWords\" bold=\"false\" italic=\"false\" color=\"SystemColors.WindowText\">" +
            //            "\n\t\t<Key word=\"TODO\" />" +
            //            "\n\t\t<Key word=\"FIXME\" />" +
            //            "\n\t</KeyWords>" +
            //            "\n\t<KeyWords name=\"WarningWords\" bold=\"false\" italic=\"false\" color=\"SystemColors.WindowText\">" +
            //            "\n\t\t<Key word=\"HACK\" />" +
            //            "\n\t\t<Key word=\"UNDONE\" />" +
            //            "\n\t</KeyWords>" +
            //            "\n\t</RuleSet>" +
            //            "\n</RuleSets>" +
            //            "\n</SyntaxDefinition>";

            #endregion

            #region syntaxFile

            string lineColor = ColorToHexString(Settings.Default.lineColor);
            string commentsColor = ColorToHexString(Settings.Default.commentColor);
            string blockCommentsColor = ColorToHexString(Settings.Default.blockCommentColor);
            string stringsColor = ColorToHexString(Settings.Default.stringColor);
            string directivesColor = ColorToHexString(Settings.Default.directiveColor);
            string preProcessorColor = ColorToHexString(Settings.Default.preProcessorColor);
            string opcodesColor = ColorToHexString(Settings.Default.opcodeColor);
            string registersColor = ColorToHexString(Settings.Default.registerColor);
            string conditionsColor = ColorToHexString(Settings.Default.conditionColor);
            string labelsColor = ColorToHexString(Settings.Default.labelColor);
            if (!Settings.Default.commentsEnabled)
                commentsColor = "000000";
            if (!Settings.Default.blockCommentEnabled)
                blockCommentsColor = "000000";
            if (!Settings.Default.stringsEnabled)
                stringsColor = "000000";
            if (!Settings.Default.directivesEnabled)
                directivesColor = "000000";
            if (!Settings.Default.preProcessorEnabled)
                preProcessorColor = "000000";
            if (!Settings.Default.opcodesEnabled)
                opcodesColor = "000000";
            if (!Settings.Default.registersEnabled)
                registersColor = "000000";
            if (!Settings.Default.conditionsEnabled)
                conditionsColor = "000000";
            if (!Settings.Default.labelsEnabled)
                labelsColor = "000000";

            string file = "<?xml version=\"1.0\" encoding=\"utf-8\"?>" +
                          "\n<SyntaxDefinition name=\"Z80 Assembly\" extensions=\".asm;.inc;.z80\">" +
                          "\n\t<Environment>" +
                          "\n<Custom name=\"TypeReference\" bold=\"false\" italic=\"false\" color=\"#04ABAB\" />" +
                          "\n\t\t<Custom name=\"UnknownEntity\" bold=\"false\" italic=\"false\" color=\"#AB0404\" />" +
                          "\n\t\t<Default bold=\"false\" italic=\"false\" color=\"Black\" bgcolor=\"White\" /> " +
                          "\n\t\t<Selection bold=\"false\" italic=\"false\" color=\"White\" bgcolor=\"#316AC5\" />" +
                          "\n\t\t<VRuler bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
                          "\n\t\t<InvalidLines bold=\"false\" italic=\"false\" color=\"Red\" />" +
                          "\n\t\t<CaretMarker bold=\"false\" italic=\"false\" color=\"#" + lineColor + "\" />" +
                          "\n\t\t<LineNumbers bold=\"false\" italic=\"false\" color=\"#ACA899\" bgcolor=\"White\" />" +
                          "\n\t\t<FoldLine bold=\"false\" italic=\"false\" color=\"Gray\" bgcolor=\"Black\" />" +
                          "\n\t\t<FoldMarker bold=\"false\" italic=\"false\" color=\"Gray\" bgcolor=\"White\" />" +
                          "\n\t\t<SelectedFoldLine bold=\"false\" italic=\"false\" color=\"Black\" />" +
                          "\n\t\t<EOLMarkers bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
                          "\n\t\t<SpaceMarkers bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
                          "\n\t\t<TabMarkers bold=\"false\" italic=\"false\" color=\"#F1EFE2\" bgcolor=\"White\" />" +
                          "\n\t\t<FirstWord bold=\"false\" italic=\"false\" color=\"#" + labelsColor + "\" />" +
                          "\n\t</Environment>" +
                          "\n\t<Properties>" +
                          "\n\t\t<Property name=\"LineComment\" value=\";\" />" +
                          "\n\t</Properties>" +
                          "\n\t<Digits name=\"Digits\" bold=\"false\" italic=\"false\" color=\"DarkBlue\" />" +
                          "\n\t<RuleSets>" +
                          "\n\t<RuleSet ignorecase=\"true\" escapecharacter=\"\\\">" +
                          "\n\t<Delimiters>&amp;&lt;&gt;~!%^*()-+=|\\/{}[]:;\"' ,	?</Delimiters>" +
                          //comments
                          "\n\t<Span name=\"DocLineComment\" rule=\"DocCommentSet\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"Green\">" +
                          "\n\t\t<Begin bold=\"false\" italic=\"false\" color=\"Gray\">///@!/@</Begin>" +
                          "\n\t</Span>" +
                          "\n\t<Span name=\"LineComment\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"#" +
                          commentsColor + "\">" +
                          "\n\t\t<Begin>;</Begin>" +
                          "\n\t</Span>" +
                          //block comments
                          "\n\t<Span name=\"BlockComment\" stopateol=\"false\" bold=\"false\" italic=\"false\" color=\"#" +
                          blockCommentsColor + "\">" +
                          "\n\t\t<Begin>#comment</Begin>" +
                          "\n\t\t<End>#endcomment</End>" +
                          "\n\t</Span>" +
                          //strings
                          "\n\t<Span name=\"String\" escapecharacter=\"\\\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"#" +
                          stringsColor + "\">" +
                          "\n\t\t<Begin>\"</Begin>" +
                          "\n\t\t<End>\"</End>" +
                          "\n\t</Span>" +
                          "\n\t<Span name=\"Char\" escapecharacter=\"\\\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"#" +
                          stringsColor + "\">" +
                          "\n\t\t<Begin>'</Begin>" +
                          "\n\t\t<End>'</End>" +
                          "\n\t</Span>" +
                          "\n\t<MarkPrevious bold=\"true\" italic=\"false\" color=\"MidnightBlue\">(</MarkPrevious>" +
                          //"\n\t<MarkPrevious bold=\"false\" italic=\"false\" color=\"#" + labelsColor + "\" markmarker=\"true\">:</MarkPrevious>" +
                          //punctuation
                          "\n\t<KeyWords name=\"Punctuation\" bold=\"false\" italic=\"false\" color=\"Black\">" +
                          "\n\t\t<Key word=\"?\" />" +
                          "\n\t\t<Key word=\",\" />" +
                          "\n\t\t<Key word=\";\" />" +
                          "\n\t\t<Key word=\"(\" />" +
                          "\n\t\t<Key word=\")\" />" +
                          "\n\t\t<Key word=\"[\" />" +
                          "\n\t\t<Key word=\"]\" />" +
                          "\n\t\t<Key word=\"{\" />" +
                          "\n\t\t<Key word=\"}\" />" +
                          "\n\t\t<Key word=\"+\" />" +
                          "\n\t\t<Key word=\"-\" />" +
                          "\n\t\t<Key word=\"/\" />" +
                          "\n\t\t<Key word=\"%\" />" +
                          "\n\t\t<Key word=\"*\" />" +
                          "\n\t\t<Key word=\"&lt;\" />" +
                          "\n\t\t<Key word=\"&gt;\" />" +
                          "\n\t\t<Key word=\"^\" />" +
                          "\n\t\t<Key word=\"=\" />" +
                          "\n\t\t<Key word=\"~\" />" +
                          "\n\t\t<Key word=\"!\" />" +
                          "\n\t\t<Key word=\"|\" />" +
                          "\n\t\t<Key word=\"&amp;\" />" +
                          "\n\t</KeyWords>" +
                          //registers
                          "\n\t<KeyWords name=\"Registers\" bold=\"false\" italic=\"false\" color=\"#" + registersColor +
                          "\">" +
                          "\n\t\t<Key word=\"a\" />" +
                          "\n\t\t<Key word=\"b\" />" +
                          "\n\t\t<Key word=\"c\" />" +
                          "\n\t\t<Key word=\"d\" />" +
                          "\n\t\t<Key word=\"e\" />" +
                          "\n\t\t<Key word=\"f\" />" +
                          "\n\t\t<Key word=\"h\" />" +
                          "\n\t\t<Key word=\"l\" />" +
                          "\n\t\t<Key word=\"i\" />" +
                          "\n\t\t<Key word=\"af\" />" +
                          "\n\t\t<Key word=\"bc\" />" +
                          "\n\t\t<Key word=\"de\" />" +
                          "\n\t\t<Key word=\"hl\" />" +
                          "\n\t\t<Key word=\"ix\" />" +
                          "\n\t\t<Key word=\"iy\" />" +
                          "\n\t\t<Key word=\"ixl\" />" +
                          "\n\t\t<Key word=\"ixh\" />" +
                          "\n\t\t<Key word=\"iyl\" />" +
                          "\n\t\t<Key word=\"iyh\" />" +
                          "\n\t\t<Key word=\"hx\" />" +
                          "\n\t\t<Key word=\"lx\" />" +
                          "\n\t\t<Key word=\"ly\" />" +
                          "\n\t\t<Key word=\"hy\" />" +
                          "\n\t\t<Key word=\"pc\" />" +
                          "\n\t</KeyWords>" +
                          //preprocessor
                          "\n\t<KeyWords name=\"Preprocessor\" bold=\"false\" italic=\"false\" color=\"#" +
                          preProcessorColor + "\">" +
                          "\n\t\t<Key word=\"#include\" />" +
                          "\n\t\t<Key word=\"#define\" />" +
                          "\n\t\t<Key word=\"#ifdef\" />" +
                          "\n\t\t<Key word=\"#ifndef\" />" +
                          "\n\t\t<Key word=\"#if\" />" +
                          "\n\t\t<Key word=\"#else\" />" +
                          "\n\t\t<Key word=\"#endif\" />" +
                          "\n\t\t<Key word=\"#undefine\" />" +
                          "\n\t\t<Key word=\"#macro\" />" +
                          "\n\t\t<Key word=\"#endmacro\" />" +
                          "\n\t\t<Key word=\"#region\" />" +
                          "\n\t\t<Key word=\"#endregion\" />" +
                          "\n\t</KeyWords>" +
                          //directives
                          "\n\t<KeyWords name=\"Directives\" bold=\"false\" italic=\"false\" color=\"#" +
                          directivesColor + "\">" +
                          "\n\t\t<Key word=\".org\" />" +
                          "\n\t\t<Key word=\".db\" />" +
                          "\n\t\t<Key word=\".dw\" />" +
                          "\n\t\t<Key word=\".echo\" />" +
                          "\n\t\t<Key word=\".list\" />" +
                          "\n\t\t<Key word=\".nolist\" />" +
                          "\n\t\t<Key word=\".end\" />" +
                          "\n\t\t<Key word=\".fill\" />" +
                          "\n\t\t<Key word=\".block\" />" +
                          "\n\t\t<Key word=\".addinstr\" />" +
                          "\n\t\t<Key word=\".word\" />" +
                          "\n\t\t<Key word=\".byte\" />" +
                          "\n\t\t<Key word=\".show\" />" +
                          "\n\t\t<Key word=\".error\" />" +
                          "\n\t\t<Key word=\".seek\" />" +
                          "\n\t</KeyWords>" +
                          //opcodes
                          "\n\t<KeyWords name=\"Opcodes\" bold=\"false\" italic=\"false\" color=\"#" + opcodesColor +
                          "\">" +
                          "\n\t\t<Key word=\"adc\" />" +
                          "\n\t\t<Key word=\"add\" />" +
                          "\n\t\t<Key word=\"and\" />" +
                          "\n\t\t<Key word=\"bit\" />" +
                          "\n\t\t<Key word=\"call\" />" +
                          "\n\t\t<Key word=\"ccf\" />" +
                          "\n\t\t<Key word=\"cpdr\" />" +
                          "\n\t\t<Key word=\"cpd\" />" +
                          "\n\t\t<Key word=\"cpir\" />" +
                          "\n\t\t<Key word=\"cpi\" />" +
                          "\n\t\t<Key word=\"cpl\" />" +
                          "\n\t\t<Key word=\"cp\" />" +
                          "\n\t\t<Key word=\"daa\" />" +
                          "\n\t\t<Key word=\"dec\" />" +
                          "\n\t\t<Key word=\"di\" />" +
                          "\n\t\t<Key word=\"djnz\" />" +
                          "\n\t\t<Key word=\"ei\" />" +
                          "\n\t\t<Key word=\"exx\" />" +
                          "\n\t\t<Key word=\"ex\" />" +
                          "\n\t\t<Key word=\"halt\" />" +
                          "\n\t\t<Key word=\"im\" />" +
                          "\n\t\t<Key word=\"inc\" />" +
                          "\n\t\t<Key word=\"indr\" />" +
                          "\n\t\t<Key word=\"ind\" />" +
                          "\n\t\t<Key word=\"inir\" />" +
                          "\n\t\t<Key word=\"ini\" />" +
                          "\n\t\t<Key word=\"in\" />" +
                          "\n\t\t<Key word=\"jp\" />" +
                          "\n\t\t<Key word=\"jr\" />" +
                          "\n\t\t<Key word=\"lddr\" />" +
                          "\n\t\t<Key word=\"ldd\" />" +
                          "\n\t\t<Key word=\"ldir\" />" +
                          "\n\t\t<Key word=\"ldi\" />" +
                          "\n\t\t<Key word=\"ld\" />" +
                          "\n\t\t<Key word=\"neg\" />" +
                          "\n\t\t<Key word=\"nop\" />" +
                          "\n\t\t<Key word=\"or\" />" +
                          "\n\t\t<Key word=\"otdr\" />" +
                          "\n\t\t<Key word=\"otir\" />" +
                          "\n\t\t<Key word=\"outd\" />" +
                          "\n\t\t<Key word=\"outi\" />" +
                          "\n\t\t<Key word=\"out\" />" +
                          "\n\t\t<Key word=\"pop\" />" +
                          "\n\t\t<Key word=\"push\" />" +
                          "\n\t\t<Key word=\"res\" />" +
                          "\n\t\t<Key word=\"reti\" />" +
                          "\n\t\t<Key word=\"retn\" />" +
                          "\n\t\t<Key word=\"ret\" />" +
                          "\n\t\t<Key word=\"rla\" />" +
                          "\n\t\t<Key word=\"rlca\" />" +
                          "\n\t\t<Key word=\"rlc\" />" +
                          "\n\t\t<Key word=\"rld\" />" +
                          "\n\t\t<Key word=\"rl\" />" +
                          "\n\t\t<Key word=\"rra\" />" +
                          "\n\t\t<Key word=\"rrca\" />" +
                          "\n\t\t<Key word=\"rrc\" />" +
                          "\n\t\t<Key word=\"rrd\" />" +
                          "\n\t\t<Key word=\"rr\" />" +
                          "\n\t\t<Key word=\"rst\" />" +
                          "\n\t\t<Key word=\"sbc\" />" +
                          "\n\t\t<Key word=\"scf\" />" +
                          "\n\t\t<Key word=\"set\" />" +
                          "\n\t\t<Key word=\"sla\" />" +
                          "\n\t\t<Key word=\"sll\" />" +
                          "\n\t\t<Key word=\"sra\" />" +
                          "\n\t\t<Key word=\"srl\" />" +
                          "\n\t\t<Key word=\"sub\" />" +
                          "\n\t\t<Key word=\"xor\" />" +
                          "\n\t</KeyWords>" +
                          //conditions
                          "\n\t<KeyWords name=\"Conditions\" bold=\"false\" italic=\"false\" color=\"#" +
                          conditionsColor + "\">" +
                          "\n\t\t<Key word=\"z\" />" +
                          "\n\t\t<Key word=\"nz\" />" +
                          //"\n\t\t<Key word=\"c\" />" +
                          "\n\t\t<Key word=\"nc\" />" +
                          "\n\t\t<Key word=\"p\" />" +
                          "\n\t\t<Key word=\"m\" />" +
                          "\n\t\t<Key word=\"v\" />" +
                          "\n\t\t<Key word=\"nv\" />" +
                          "\n\t\t<Key word=\"pe\" />" +
                          "\n\t\t<Key word=\"po\" />" +
                          "\n\t</KeyWords>" +
                          "\n\t</RuleSet>" +
                          "\n\t<RuleSet name=\"DocCommentSet\" ignorecase=\"false\">" +
                          "\n\t\t<Delimiters>&amp;&lt;&gt;~!@%^*()-+=|\\#/{}[]:;\"' ,	.?</Delimiters>" +
                          "\n\t<Span name=\"XmlTag\" stopateol=\"true\" bold=\"false\" italic=\"false\" color=\"SystemColors.WindowText\">" +
                          "\n\t\t<Begin>&lt;</Begin>" +
                          "\n\t\t<End>&gt;</End>" +
                          "\n\t</Span>" +
                          "\n\t<KeyWords name=\"ErrorWords\" bold=\"false\" italic=\"false\" color=\"SystemColors.WindowText\">" +
                          "\n\t\t<Key word=\"TODO\" />" +
                          "\n\t\t<Key word=\"FIXME\" />" +
                          "\n\t</KeyWords>" +
                          "\n\t<KeyWords name=\"WarningWords\" bold=\"false\" italic=\"false\" color=\"SystemColors.WindowText\">" +
                          "\n\t\t<Key word=\"HACK\" />" +
                          "\n\t\t<Key word=\"UNDONE\" />" +
                          "\n\t</KeyWords>" +
                          "\n\t</RuleSet>" +
                          "\n</RuleSets>" +
                          "\n</SyntaxDefinition>";

            #endregion

			try
			{
				File.Delete(Application.UserAppDataPath + "\\Z80Asm.xshd");
				FileStream stream = new FileStream(Application.UserAppDataPath + "\\Z80Asm.xshd", FileMode.CreateNew);
				StreamWriter writeXML = new StreamWriter(stream);
				writeXML.WriteLine(file);
				writeXML.Flush();
				writeXML.Close();
			}
			catch (Exception ex)
			{
				MessageBox.Show("Error creating highlighting file!\n" + ex.ToString());
			}
            //create new provider with the temp directory
            FileSyntaxModeProvider fsmProvider = new FileSyntaxModeProvider(Application.UserAppDataPath);
            // Attach to the text editor
            HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider);
            if (string.IsNullOrEmpty(Settings.Default.externalHighlight))
                return;
            FileSyntaxModeProvider fsmProvider2 = new FileSyntaxModeProvider(Path.GetDirectoryName(Settings.Default.externalHighlight));
            HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider2);
        }

        /// <summary>
        /// Convert a Color to a hex string.
        /// </summary>
        /// <returns>ex: "FFFFFF", "AB12E9"</returns>
        public static string ColorToHexString(Color color)
        {
            return color.R.ToString("X2") + color.G.ToString("X2") + color.B.ToString("X2");
        }
    }
}