using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Services.Interface;
using System;
using System.Drawing;


namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class OutputWindow : ToolWindow
	{
		private readonly IDocumentService _documentService;
		public OutputWindow(IDockingService dockingService, IDocumentService documentService)
			: base(dockingService)
		{
			InitializeComponent();

			_documentService = documentService;

			outputWindowBox.ContextMenu = contextMenu1;
		}

		public override void Copy()
		{
			outputWindowBox.Copy();
		}

		public void HighlightOutput()
		{
			int i = 0;
			foreach (String line in outputWindowBox.Lines)
			{
				if (line.Contains("error"))
				{
					outputWindowBox.Select(
						outputWindowBox.GetFirstCharIndexFromLine(i),
						outputWindowBox.GetFirstCharIndexFromLine(i + 1) -
						outputWindowBox.GetFirstCharIndexFromLine(i));
					outputWindowBox.SelectionColor = Color.Red;
				}

				if (line.Contains("warning"))
				{
					outputWindowBox.Select(
						outputWindowBox.GetFirstCharIndexFromLine(i),
						outputWindowBox.GetFirstCharIndexFromLine(i + 1) -
						outputWindowBox.GetFirstCharIndexFromLine(i));
					outputWindowBox.SelectionColor = Color.Gold;
				}

				i++;
			}
		}

		internal void AddText(string outputText)
		{
			outputWindowBox.Text += outputText;
		}

		internal void ClearOutput()
		{
			outputWindowBox.Clear();
		}

		internal void UpdateFont(Font font)
		{
			outputWindowBox.Font = font;
		}

		private void copyOutputButton_Click(object sender, EventArgs e)
		{
			outputWindowBox.Copy();
		}

		private void outputWindowBox_DoubleClick(object sender, EventArgs e)
		{
			// file:line:error code:description
			// SPASM uses the format %s:%d: %s %s%03X: %s currently
			int errorLine = outputWindowBox.GetLineFromCharIndex(outputWindowBox.SelectionStart);
			string lineContents = outputWindowBox.Lines[errorLine];
			Match match = Regex.Match(lineContents, @"(?<fileName>.+):(?<lineNum>\d+): (?<errorCode>.+): (?<description>.+)");
			if (!match.Success)
			{
				return;
			}

			string file = match.Groups["fileName"].Value;
			int lineNumber = Convert.ToInt32(match.Groups["lineNum"].Value);
			_documentService.GotoLine(file, lineNumber);
		}

		private void selectAllOuputButton_Click(object sender, EventArgs e)
		{
			outputWindowBox.SelectAll();
		}
	}
}