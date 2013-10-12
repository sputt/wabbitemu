using System;
using System.ComponentModel;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode
{
    public partial class GotoSymbol : Form
    {
	    private readonly IParserService _parserService;

        public GotoSymbol(IParserService parserService)
        {
	        _parserService = parserService;
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

		private void ValidateInput()
		{
			string symbolName = inputBox.Text;
			IParserData data = _parserService.GetParserData(symbolName, Settings.Default.caseSensitive).FirstOrDefault();
			SetInputError(data == null ? "Invalid symbol name" : string.Empty);
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

        private void okButton_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}