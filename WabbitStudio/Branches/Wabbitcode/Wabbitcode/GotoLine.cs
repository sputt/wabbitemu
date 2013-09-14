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
	        string errorString = string.Format("A number between (1, {0}) is required", _maxLineNum);
            int line;
            if (int.TryParse(inputBox.Text, out line))
            {
	            if (line <= 0 || line > _maxLineNum)
	            {
					SetInputError(errorString);
		            return;
	            }

	            SetInputError(string.Empty);
	            return;
            }

            SetInputError(errorString);
        }

		/// <summary>
		/// Sets the error string of the error provider. An empty string provided means there is no
		/// error.
		/// </summary>
		/// <param name="errorString">The error string to show</param>
	    private void SetInputError(string errorString)
	    {
		    okButton.Enabled = string.IsNullOrEmpty(errorString);
		    errorProvider.SetError(inputBox, errorString);
	    }
    }
}