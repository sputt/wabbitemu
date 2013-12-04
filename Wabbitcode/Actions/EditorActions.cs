using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Actions;
using Revsoft.Wabbitcode.DockingWindows;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using IFileReaderService = Revsoft.Wabbitcode.Services.Interfaces.IFileReaderService;

namespace Revsoft.Wabbitcode.Actions
{
    public class GotoLabelAction : AbstractUiAction
    {
        private readonly string _fileName;
        private readonly string _text;
        private readonly int _currentLine;
        private readonly IDockingService _dockingService;
        private readonly IDocumentService _documentService;
        private readonly IParserService _parserService;
        private readonly IProjectService _projectService;
        private readonly IFileReaderService _fileReaderService;
        private readonly FindResultsWindow _findResults;

        public GotoLabelAction(string fileName, string text, int currentLine)
        {
            _fileName = fileName;
            _text = text;
            _currentLine = currentLine;
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            _fileReaderService = ServiceFactory.Instance.GetServiceInstance<IFileReaderService>();
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _findResults = _dockingService.GetDockingWindow(FindResultsWindow.WindowIdentifier) as FindResultsWindow;
        }

        public override void Execute()
        {
            IList<IParserData> parserData;
            if (_text.StartsWith("+") || _text.StartsWith("-") || _text == "_")
            {
                int steps = _text.Count(c => c == '+') - _text.Count(c => c == '-');
                if (steps > 0)
                {
                    steps--;
                }
                var parserInfo = _parserService.GetParserInfo(_fileName);
                List<ILabel> reusableLabels = parserInfo.LabelsList.Where(l => l.IsReusable).ToList();
                ILabel currentLabel = reusableLabels.FirstOrDefault(l => l.Location.Line >= _currentLine);
                if (currentLabel == null)
                {
                    return;
                }

                int index = reusableLabels.IndexOf(currentLabel) + steps;
                parserData = new List<IParserData> { reusableLabels[index] };
            }
            else
            {
                parserData = _parserService.GetParserData(_text, Settings.Default.CaseSensitive).ToList();
            }

            if (parserData.Count == 1)
            {
                _documentService.GotoLabel(parserData.Single());
            }
            else
            {
                _findResults.NewFindResults(_text, _projectService.Project.ProjectName);
                foreach (IParserData data in parserData)
                {
                    string line = _fileReaderService.GetLine(data.Parent.SourceFile, data.Location.Line + 1);
                    _findResults.AddFindResult(data.Parent.SourceFile, data.Location.Line, line);
                }
                _findResults.DoneSearching();
                _dockingService.ShowDockPanel(_findResults);
            }
        }

        public static void FromDialog()
        {
            GotoSymbol gotoSymbolBox = new GotoSymbol();
            if (gotoSymbolBox.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            string symbolString = gotoSymbolBox.inputBox.Text;
            new GotoLabelAction(string.Empty, symbolString, 0).Execute();
        }
    }

    public class GotoLineAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public GotoLineAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            var editor = _dockingService.ActiveDocument as ITextEditor;
            if (editor == null)
            {
                return;
            }

            GotoLine gotoBox = new GotoLine(editor.TotalLines);
            DialogResult gotoResult = gotoBox.ShowDialog();
            if (gotoResult != DialogResult.OK)
            {
                return;
            }

            int line = Convert.ToInt32(gotoBox.inputBox.Text);
            editor.GotoLine(line);
        }
    }

    public class ShowCodeCompletion : IEditAction
    {
        public Keys[] Keys { get; set; }
        public void Execute(TextArea textArea)
        {
            WabbitcodeTextEditor editor = textArea.MotherTextEditorControl as WabbitcodeTextEditor;
            if (editor != null)
            {
                editor.StartCtrlSpaceCompletion();
            }
        }
    }
}
