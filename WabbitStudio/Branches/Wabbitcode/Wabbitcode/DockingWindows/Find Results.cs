using System.Text;
using System.Text.RegularExpressions;
using System;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;


namespace Revsoft.Wabbitcode.DockingWindows
{
	public partial class FindResultsWindow : ToolWindow
	{
	    public const string WindowName = "Find Results";

		private readonly IDocumentService _documentService;
		private readonly StringBuilder _stringBuilder;
		private int _numResults;

		public FindResultsWindow()
		{
			InitializeComponent();

            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
			_stringBuilder = new StringBuilder();
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

			_numResults = 0;
			_stringBuilder.Clear();
			_stringBuilder.Append(string.Format("Searching for \"{0}\" in {1}\n", serachString, projectfile));
		}


		public void AddFindResult(string file, int lineNum, string line)
		{
			_stringBuilder.Append(string.Format("{0} ({1}): {2}\n", file, lineNum + 1, line));
			_numResults++;
		}

		public  void AddFindResult(Reference reference)
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

			string file = match.Groups["fileName"].Value;
			int lineNumber = Convert.ToInt32(match.Groups["lineNum"].Value);
			_documentService.GotoLine(file, lineNumber);
		}
	}
}