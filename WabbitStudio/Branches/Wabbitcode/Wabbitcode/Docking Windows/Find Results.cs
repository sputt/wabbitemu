using Revsoft.Wabbitcode.Services;
using System;


namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class FindResultsWindow : ToolWindow
	{
		public FindResultsWindow(IDockingService dockingService)
			: base(dockingService)
		{
			InitializeComponent();
		}

		public void AddFindResult(string file, int lineNum, string line)
		{
			findResultsBox.Text += file + "(" + (lineNum + 1) + "): " + line + "\n";
		}

		public override void Copy()
		{
			findResultsBox.Copy();
		}

		public void NewFindResults(string serachString, string projectfile)
		{
			if (string.IsNullOrEmpty(projectfile))
			{
				projectfile = " all open files";
			}

			findResultsBox.Text = "Searching for \"" + serachString + "\" in " + projectfile + "\n";
		}

		internal void AddFindResult(Reference reference)
		{
			AddFindResult(reference.File, reference.Line, reference.LineContents);
		}

		internal void DoneSearching()
		{
			findResultsBox.Text += "Done Searching";
		}

		private void findResultsBox_DoubleClick(object sender, EventArgs e)
		{
			try
			{
				int line = findResultsBox.GetLineFromCharIndex(findResultsBox.SelectionStart);
				string lineContents = findResultsBox.Lines[line];
				int firstParen = lineContents.IndexOf('(');
				if (firstParen == -1)
				{
					return;
				}

				int secondParen = lineContents.IndexOf(')');
				string file = lineContents.Substring(0, firstParen);
				int lineNum = Convert.ToInt32(lineContents.Substring(firstParen + 1, secondParen - firstParen - 1));
				DocumentService.GotoLine(file, lineNum);
			}
			catch (Exception ex)
			{
				DockingService.ShowError(ex.ToString());
			}
		}
	}
}