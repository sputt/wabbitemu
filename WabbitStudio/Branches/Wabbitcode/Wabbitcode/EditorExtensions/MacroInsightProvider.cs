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
    internal class MacroInsightProvider : IInsightDataProvider
    {
        private TextArea _textArea;
        private int _initialOffset;
        private List<string> _data;

        public void SetupDataProvider(string fileName, TextArea textArea)
        {
            _textArea = textArea;
            _initialOffset = textArea.Caret.Offset;

            var caret = textArea.Caret;
            var segment = textArea.Document.GetLineSegment(caret.Line);
		    var wordWord = segment.GetWord(caret.Column - 1);
            string word = wordWord == null ? string.Empty : wordWord.Word;

            IParserService parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
            var parserData = parserService.GetParserData(word, Settings.Default.CaseSensitive).ToList();
            _data = new List<string>();
            _data.AddRange(parserData.Where(d => !string.IsNullOrEmpty(d.Description) || d is IMacro)
                .Select(d =>
                {
                    string description = d.Description;
                    IMacro macro = d as IMacro;
                    if (macro != null)
                    {
                        string argDesc = macro.Arguments.Aggregate("(", (current, arg) => current + (arg + ", "));
                        if (argDesc.Length > 2)
                        {
                            argDesc = argDesc.Remove(argDesc.Length - 2);
                        }
                        argDesc += ")";
                        description = argDesc + "\n" + description;
                    }
                    return description;
                }));
        }

        public bool CaretOffsetChanged()
        {
            bool closeDataProvider = _textArea.Caret.Offset <= _initialOffset;
            int brackets = 0;
            if (closeDataProvider)
            {
                return true;
            }

            bool insideChar = false;
            bool insideString = false;
            for (int offset = _initialOffset; offset < Math.Min(_textArea.Caret.Offset, _textArea.Document.TextLength); ++offset)
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
                        if (!(insideChar || insideString))
                        {
                            return true;
                        }
                        break;
                }
            }

            return false;
        }

        public string GetInsightData(int number)
        {
            return _data[number];
        }

        public int InsightDataCount
        {
            get
            {
                return _data.Count;
            }
        }

        public int DefaultIndex
        {
            get
            {
                return 0;
            }
        }
    }
}
