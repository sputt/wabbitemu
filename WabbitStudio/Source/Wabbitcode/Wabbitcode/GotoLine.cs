using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
    public partial class GotoLine : Form
    {
		int MaxLineNum;
        public GotoLine(int maxLine)
        {
			MaxLineNum = maxLine;
            InitializeComponent();
        }
        private void okButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

		private void inputBox_Validating(object sender, CancelEventArgs e)
		{
			Validate();
		}

		private void inputBox_TextChanged(object sender, EventArgs e)
		{
			Validate();
		}

		private bool Validate()
		{
			int line = 0;
			int.TryParse(inputBox.Text, out line);
			if (line > 0 && line <= MaxLineNum)
			{
				okButton.Enabled = true;
				errorProvider.SetError(inputBox, "");
				return true;
			}
			okButton.Enabled = false;
			errorProvider.SetError(inputBox, "A number between (1, " + MaxLineNum + ") is required");
			return false;
		}
    }
}
