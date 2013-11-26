using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.Extensions
{
    public class AsmFormattingStrategy : DefaultFormattingStrategy
    {
        public override void FormatLine(TextEditor.TextArea textArea, int line, int cursorOffset, char ch)
        {
            base.FormatLine(textArea, line, cursorOffset, ch);
	        if (ch != '#')
	        {
		        return;
	        }

	        int indent = GetIndentation(textArea.Document, line).Length;
	        string textLine = TextUtilities.GetLineAsString(textArea.Document, line);
	        if (!string.IsNullOrEmpty(textLine.Trim()))
	        {
		        return;
	        }

	        textLine = textLine.Remove(0, indent);
	        LineSegment oldLine = textArea.Document.GetLineSegment(line);
	        SmartReplaceLine(textArea.Document, oldLine, textLine);
	        textArea.Caret.Column = 1;
        }
    }
}