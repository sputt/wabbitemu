using System;
using System.Drawing;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
	public partial class OutputWindow : ToolWindow, ISelectable
	{
	    public const string WindowIdentifier = "Output";

	    public override string WindowName
	    {
            get { return WindowIdentifier; }
	    }

		public OutputWindow()
		{
		    InitializeComponent();

		    IAssemblerService assemblerService = ServiceFactory.Instance.GetServiceInstance<IAssemblerService>();

			outputWindowBox.ContextMenu = contextMenu1;
            Settings.Default.SettingChanging += Default_SettingChanging;

            assemblerService.AssemblerProjectFinished += AssemblerService_OnAssemblerProjectFinished;
		}

	    private void AssemblerService_OnAssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
	    {
	        if (!IsHandleCreated)
	        {
	            return;
	        }

	        this.Invoke(() =>
	        {
	            ClearOutput();
	            AddText(e.Output.OutputText);
	            HighlightOutput();
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

	    private void HighlightOutput()
		{
			int i = 0;
			foreach (string line in outputWindowBox.Lines)
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

	    private void AddText(string outputText)
		{
			outputWindowBox.Text += outputText;
		}

	    private void ClearOutput()
		{
			outputWindowBox.Clear();
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
            new GotoLineAction(file, lineNumber - 1).Execute();
		}

		private void selectAllOuputButton_Click(object sender, EventArgs e)
		{
			SelectAll();
		}

	    public void SelectAll()
	    {
            outputWindowBox.SelectAll();
	    }
	}
}