using System;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class FindResultsWindow : ToolWindow
    {
        public FindResultsWindow()
        {
            InitializeComponent();
        }

        private void FindResults_VisibleChanged(object sender, EventArgs e)
        {
            DockingService.MainForm.UpdateChecks();
        }

        public void NewFindResults(string serachString, string projectfile)
        {
            if (string.IsNullOrEmpty(projectfile))
                projectfile = " all open files";
            findResultsBox.Text = "Searching for \"" + serachString + "\" in " + projectfile + "\n";
        }

        public void AddFindResult(string file, int lineNum, string line)
        {
            findResultsBox.Text += file + "(" + lineNum + "):" + line + "\n";
        }

        private void findResultsBox_DoubleClick(object sender, EventArgs e)
        {
            int line = findResultsBox.GetLineFromCharIndex(findResultsBox.SelectionStart);
            string lineContents = findResultsBox.Lines[line];
            int firstParen = lineContents.IndexOf('(');
            int secondParen = lineContents.IndexOf(')');
            string file = lineContents.Substring(0, firstParen);
            int lineNum = Convert.ToInt32(lineContents.Substring(firstParen + 1, secondParen - firstParen - 1));
            DocumentService.GotoLine(file, lineNum + 1);
        }

		public override void Copy()
		{
			findResultsBox.Copy();
		}

		internal void DoneSearching()
		{
			 findResultsBox.Text += "Done Searching";
		}
	}
}