namespace Revsoft.Wabbitcode
{
    using System;
    using System.ComponentModel;
    using System.Windows.Forms;

    public partial class GotoLine : Form
    {
        private readonly int _maxLineNum;

        public GotoLine(int maxLine)
        {
            _maxLineNum = maxLine;
            InitializeComponent();
        }

        private void inputBox_TextChanged(object sender, EventArgs e)
        {
            ValidateInput();
        }

        private void inputBox_Validating(object sender, CancelEventArgs e)
        {
            ValidateInput();
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void ValidateInput()
        {
            int line;
            if (int.TryParse(inputBox.Text, out line))
            {
	            if (line <= 0 || line > _maxLineNum)
	            {
		            return;
	            }

	            okButton.Enabled = true;
	            errorProvider.SetError(inputBox, string.Empty);
	            return;
            }

            okButton.Enabled = false;
            errorProvider.SetError(inputBox, "A number between (1, " + _maxLineNum + ") is required");
        }
    }
}