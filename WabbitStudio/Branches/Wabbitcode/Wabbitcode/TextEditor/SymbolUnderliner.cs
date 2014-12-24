using System;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.TextEditor
{
    internal class SymbolUnderliner
    {
        private readonly IParserService _parserService = DependencyFactory.Resolve<IParserService>();
        private readonly WabbitcodeTextEditor _editor;

        private TextMarker _currentMarker;
        private int _currentMarkerLine;

        public SymbolUnderliner(WabbitcodeTextEditor editor)
        {
            _editor = editor;
            _editor.ActiveTextAreaControl.TextArea.MouseClick += editorBox_MouseClick;
            _editor.ActiveTextAreaControl.TextArea.MouseHover += editorBox_MouseHover;
        }

        private void editorBox_MouseHover(object sender, EventArgs e)
        {
            RemoveCurrentMarker();

            if ((Control.ModifierKeys & Keys.Control) == 0)
            {
                return;
            }

            TextArea textArea = (TextArea) sender;
            var point = textArea.PointToClient(Control.MousePosition);
            var location = GetWordUnderCursor(point);
            var textWord = GetTextWordAtLocation(location);
            if (textWord == null)
            {
                return;
            }

            string symbolName = textWord.Word;
            IParserData data = _parserService.GetParserData(symbolName, Settings.Default.CaseSensitive).FirstOrDefault();
            if (data == null)
            {
                return;
            }

            IDocument document = textArea.Document;
            int offset = document.GetOffsetForLineNumber(location.Line) + textWord.Offset;
            _currentMarkerLine = location.Line;
            _currentMarker = new TextMarker(offset, textWord.Length, TextMarkerType.Underlined, Color.Black);
            document.MarkerStrategy.AddMarker(_currentMarker);
            document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, _currentMarkerLine));
            document.CommitUpdate();
        }

        private void editorBox_MouseClick(object sender, MouseEventArgs e)
        {
            if ((Control.ModifierKeys & Keys.Control) == 0)
            {
                return;
            }

            var location = GetWordUnderCursor(e.Location);
            var textWord = GetTextWordAtLocation(location);
            if (textWord == null)
            {
                return;
            }

            string text = textWord.Word;
            FilePath filePath = new FilePath(_editor.FileName);
            AbstractUiAction.RunCommand(new GotoDefinitionAction(filePath, text, location.Line));
            RemoveCurrentMarker();
        }

        private void RemoveCurrentMarker()
        {
            _editor.Document.MarkerStrategy.RemoveMarker(_currentMarker);
            _editor.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, _currentMarkerLine));
            _editor.Document.CommitUpdate();
            _currentMarkerLine = -1;
            _currentMarker = null;
        }


        private TextWord GetTextWordAtLocation(TextLocation location)
        {
            if (location.Line >= _editor.Document.TotalNumberOfLines)
            {
                return null;
            }

            LineSegment segment = _editor.Document.GetLineSegment(location.Line);
            TextWord textWord = segment.GetWord(location.Column);
            return textWord;
        }

        private TextLocation GetWordUnderCursor(Point point)
        {
            TextView textView = _editor.ActiveTextAreaControl.TextArea.TextView;
            point = Point.Subtract(point, new Size(textView.DrawingPosition.Location));
            TextLocation location = textView.GetLogicalPosition(point);
            return location;
        }
    }
}
