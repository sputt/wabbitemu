using System;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class ExpressionWindow : ToolWindow
    {
        private int _lineStartIndex;
        private string _currentLine;
        private readonly IAssemblerFactory _assemblerFactory = DependencyFactory.Resolve<IAssemblerFactory>();
        private readonly IAssembler _assembler;

        public ExpressionWindow()
        {
            InitializeComponent();
            _assembler = _assemblerFactory.CreateAssembler();
            _currentLine = string.Empty;
            DisplayLineStart();
        }

        private void DisplayNewLine()
        {
            expressionBox.Text += Environment.NewLine;
        }

        private void DisplayLineStart()
        {
            expressionBox.Text += " > ";
            expressionBox.SelectionStart = expressionBox.TextLength;
            _lineStartIndex = expressionBox.SelectionStart;
        }

        public override void Copy()
        {
            string value = expressionBox.SelectedText;
            if (string.IsNullOrEmpty(value))
            {
                value = _currentLine;
            }

            Clipboard.SetText(value);
        }

        private void expressionBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (expressionBox.SelectionStart < _lineStartIndex)
            {
                expressionBox.SelectionStart = expressionBox.TextLength;
            }

            if (e.KeyChar == (char) Keys.Enter)
            {
                ParseLine();
            }
            else if (!char.IsControl(e.KeyChar))
            {
                _currentLine += e.KeyChar;
            }
            else if (e.KeyChar == (char) Keys.Back)
            {
                if (string.IsNullOrEmpty(_currentLine))
                {
                    e.Handled = true;
                    return;
                }
                _currentLine = _currentLine.Substring(0, _currentLine.Length - 1);
            }
        }

        private void ParseLine()
        {
            string output = _assembler.Assemble(_currentLine);
            _currentLine = string.Empty;

            Match match = Regex.Match(output, "Pass one... \r\nPass two... \r\n(?<value>.*\r\n)*Done\r\nAssembly time: .* seconds\r\n",
                RegexOptions.Compiled | RegexOptions.Singleline);
            if (match.Success)
            {
                string filteredOutput = match.Groups["value"].Value;
                DisplayOutput(filteredOutput);
                DisplayLineStart();
            }
            else
            {
                DisplayOutput("Error in input");
                DisplayLineStart();
            }
        }

        private void DisplayOutput(string output)
        {
            expressionBox.Text += output;
            DisplayNewLine();
        }
    }
}