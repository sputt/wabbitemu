using System;
using System.IO;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
    public partial class NewBreakpointForm : Form
    {
        public NewBreakpointForm()
        {
            InitializeComponent();
        }

        private void textChange(object sender, EventArgs e)
        {
            textLabel.Text = "Line Text:";
            string file = fileBox.Text;
            int lineNum;
            if (!Int32.TryParse(lineBox.Text, out lineNum))
                return;
            lineNum--;
            if (!File.Exists(file))
                return;
            StreamReader reader = new StreamReader(file);
            string[] lines = reader.ReadToEnd().Split('\n');
            reader.Close();
            if (lines.Length <= lineNum)
                return;
            textLabel.Text = "Line Text: " + lines[lineNum];
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            int lineNum;
            if (!Int32.TryParse(lineBox.Text, out lineNum))
                return;
            lineNum--;
            if (File.Exists(fileBox.Text))
                DocumentService.GotoFile(fileBox.Text);
            else
            {
                MessageBox.Show("File doesn't exist!");
                return;
            }

			if (DockingService.ActiveDocument == null) 
                return;
            DockingService.ActiveDocument.ScrollToLine(lineNum);
            DockingService.ActiveDocument.ToggleBreakpoint(lineNum);
        }

        private void browseButton_Click(object sender, EventArgs e)
        {
			OpenFileDialog openFileDialog = new OpenFileDialog()
            {
                Filter = "All Know File Types | *.asm; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
                           " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
                FilterIndex = 0,
                RestoreDirectory = true,
                Title = "Open File",
            };
            if (openFileDialog.ShowDialog() == DialogResult.OK)
                fileBox.Text = openFileDialog.FileName;
        }
    }
}
