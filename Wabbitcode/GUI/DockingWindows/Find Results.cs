using System;
using System.Text;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class FindResultsWindow : ToolWindow
    {
        private readonly StringBuilder _stringBuilder;
        private int _numResults;

        public FindResultsWindow()
        {
            InitializeComponent();

            _stringBuilder = new StringBuilder();
        }

        public override void Copy()
        {
            findResultsBox.Copy();
        }

        public void NewFindResults(string searchString, string projectfile)
        {
            if (string.IsNullOrEmpty(projectfile))
            {
                projectfile = " all open files";
            }

            _numResults = 0;
            _stringBuilder.Clear();
            _stringBuilder.Append(string.Format("Searching for \"{0}\" in {1}\n", searchString, projectfile));
        }


        public void AddFindResult(string file, int lineNum, string line)
        {
            int newLineIndex = line.IndexOf('\n');
            int zeroIndex = line.IndexOf('\0');
            if (newLineIndex == -1)
            {
                newLineIndex = line.Length;
            }

            if (zeroIndex == -1)
            {
                zeroIndex = line.Length;
            }

            int removeIndex = Math.Min(newLineIndex, zeroIndex);
            if (removeIndex != line.Length)
            {
                line = line.Remove(removeIndex);
            }

            _stringBuilder.Append(string.Format("{0} ({1}): {2}\n", file, lineNum + 1, line));
            _numResults++;
        }

        public void AddFindResult(Reference reference)
        {
            AddFindResult(reference.File, reference.Line, reference.LineContents);
        }

        public void DoneSearching()
        {
            _stringBuilder.Append(string.Format("Done Searching. {0} results", _numResults));
            findResultsBox.Text = _stringBuilder.ToString();
        }

        private void findResultsBox_DoubleClick(object sender, EventArgs e)
        {
            int line = findResultsBox.GetLineFromCharIndex(findResultsBox.SelectionStart);
            string lineContents = findResultsBox.Lines[line];
            Match match = Regex.Match(lineContents, @"(?<fileName>.+) \((?<lineNum>\d+)\): (?<line>.+)");
            if (!match.Success)
            {
                return;
            }

            FilePath file = new FilePath(match.Groups["fileName"].Value);
            line = Convert.ToInt32(match.Groups["lineNum"].Value);
            AbstractUiAction.RunCommand(new GotoLineAction(file, line - 1));
        }
    }
}