using System;
using System.Drawing;
using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class OutputWindow : ToolWindow, ISelectable
    {
        private readonly IAssemblerService _assemblerService;

        public OutputWindow()
        {
            InitializeComponent();

            _assemblerService = DependencyFactory.Resolve<IAssemblerService>();

            outputWindowBox.ContextMenu = contextMenu1;
            Settings.Default.SettingChanging += Default_SettingChanging;

            _assemblerService.AssemblerProjectFinished += AssemblerService_OnAssemblerProjectFinished;
        }

        private void AssemblerService_OnAssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
        {
            this.Invoke(() =>
            {
                UpdateOutput(e.Output.OutputText);
            });
        }

        private void Default_SettingChanging(object sender, System.Configuration.SettingChangingEventArgs e)
        {
            if (e.SettingName == "OutputFont")
            {
                outputWindowBox.Font = (Font) e.NewValue;
            }
        }

        public override void Copy()
        {
            outputWindowBox.Copy();
        }

        private void UpdateOutput(string newContents)
        {
            var vertScrollValue = outputWindowBox.VerticalScroll.Value;
            var horzScrollValue = outputWindowBox.HorizontalScroll.Value;
            var selectionList = outputWindowBox.ActiveTextAreaControl.SelectionManager.SelectionCollection;
            var caretLine = outputWindowBox.ActiveTextAreaControl.Caret.Line;
            var caretCol = outputWindowBox.ActiveTextAreaControl.Caret.Column;

            outputWindowBox.Document.TextContent = newContents;
            outputWindowBox.Document.MarkerStrategy.RemoveAll(m => true);
            for (int i = 0; i < outputWindowBox.Document.TotalNumberOfLines; i++)
            {
                LineSegment segment = outputWindowBox.Document.GetLineSegment(i);
                string line =  outputWindowBox.Document.GetText(segment);
                if (line.Contains("error") || line.Contains("Error"))
                {
                    var errorMarker = new TextMarker(segment.Offset, 
                        segment.TotalLength,
                        TextMarkerType.SolidBlock,
                        Color.Red);
                    outputWindowBox.Document.MarkerStrategy.AddMarker(errorMarker);
                }

                if (line.Contains("warning") || line.Contains("Warning"))
                {
                    var errorMarker = new TextMarker(segment.Offset,
                        segment.TotalLength,
                        TextMarkerType.SolidBlock,
                        Color.Gold);
                    outputWindowBox.Document.MarkerStrategy.AddMarker(errorMarker);
                }
            }

            outputWindowBox.ActiveTextAreaControl.Caret.Line = caretLine;
            outputWindowBox.ActiveTextAreaControl.Caret.Column = caretCol;
            outputWindowBox.VerticalScroll.Value = vertScrollValue;
            outputWindowBox.HorizontalScroll.Value = horzScrollValue;
            foreach (var selection in selectionList)
            {
                outputWindowBox.ActiveTextAreaControl.SelectionManager.SetSelection(selection);
            }

            outputWindowBox.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
            outputWindowBox.Document.CommitUpdate();
        }


        private void copyOutputButton_Click(object sender, EventArgs e)
        {
            outputWindowBox.Copy();
        }

        private void outputWindowBox_DoubleClick(object sender, EventArgs e)
        {
            // file:line:error code:description
            // SPASM uses the format %s:%d: %s %s%03X: %s currently
            int errorLineOffset =
                outputWindowBox.ActiveTextAreaControl.SelectionManager.SelectionCollection.First().Offset;
            int errorLine = outputWindowBox.Document.GetLineNumberForOffset(errorLineOffset);
            var segment = outputWindowBox.Document.GetLineSegment(errorLine);
            string lineContents = outputWindowBox.Document.GetText(segment);
            Match match = Regex.Match(lineContents, @"(?<fileName>.+):(?<lineNum>\d+): (?<errorCode>.+): (?<description>.+)");
            if (!match.Success)
            {
                return;
            }

            FilePath file = new FilePath(match.Groups["fileName"].Value);
            int lineNumber = Convert.ToInt32(match.Groups["lineNum"].Value);
            AbstractUiAction.RunCommand(new GotoLineAction(file, lineNumber - 1));
        }

        private void selectAllOuputButton_Click(object sender, EventArgs e)
        {
            SelectAll();
        }

        public void SelectAll()
        {
            var document = outputWindowBox.Document;
            int numLines = document.TotalNumberOfLines - 1;
            TextLocation selectStart = new TextLocation(0, 0);
            TextLocation selectEnd = new TextLocation(document.GetLineSegment(numLines).Length, numLines);
            outputWindowBox.ActiveTextAreaControl.SelectionManager.SetSelection(new DefaultSelection(document, selectStart, selectEnd));
        }
    }
}