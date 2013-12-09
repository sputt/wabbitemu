using System;
using System.IO;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
	public partial class NewBreakpointForm : Form
	{
		#region Private Members

	    #endregion

		public NewBreakpointForm()
		{
			InitializeComponent();
		}

		private void browseButton_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "All Know File Types | *.asm; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
						 " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
				FilterIndex = 0,
				RestoreDirectory = true,
				Title = "Open File",
			};
			if (openFileDialog.ShowDialog() == DialogResult.OK)
			{
				fileBox.Text = openFileDialog.FileName;
			}
		}

		private void okButton_Click(object sender, EventArgs e)
		{
			int lineNum;
			if (!int.TryParse(lineBox.Text, out lineNum))
			{
				return;
			}

			lineNum--;
			string fileName = fileBox.Text;
			if (File.Exists(fileName))
			{
                new GotoFileAction(fileName).Execute();
			}
			else
			{
				DockingService.ShowError("File doesn't exist!");
				return;
			}

			WabbitcodeBreakpointManager.AddBreakpoint(fileName, lineNum);
		}

		private void TextChange(object sender, EventArgs e)
		{
			textLabel.Text = "Line Text:";
			string file = fileBox.Text;
			int lineNum;
			if (!int.TryParse(lineBox.Text, out lineNum))
			{
				return;
			}

			lineNum--;
			if (!File.Exists(file))
			{
				return;
			}

			StreamReader reader = new StreamReader(file);
			string[] lines = reader.ReadToEnd().Split('\n');
			reader.Close();
			if (lines.Length <= lineNum)
			{
				return;
			}

			textLabel.Text = "Line Text: " + lines[lineNum];
		}
	}
}