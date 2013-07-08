namespace Revsoft.Wabbitcode
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Data;
    using System.Drawing;
    using System.Text;
    using System.Windows.Forms;

    public partial class GotoLine : Form
    {
        private int MaxLineNum;

        public GotoLine(int maxLine)
        {
            this.MaxLineNum = maxLine;
            InitializeComponent();
        }

        private void inputBox_TextChanged(object sender, EventArgs e)
        {
            this.ValidateInput();
        }

        private void inputBox_Validating(object sender, CancelEventArgs e)
        {
            this.ValidateInput();
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private bool ValidateInput()
        {
            int line = 0;
            int.TryParse(inputBox.Text, out line);
            if (line > 0 && line <= this.MaxLineNum)
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