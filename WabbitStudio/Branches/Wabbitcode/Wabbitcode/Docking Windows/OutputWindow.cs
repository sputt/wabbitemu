using Revsoft.Wabbitcode.Services;
using System;
using System.Drawing;


namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class OutputWindow : ToolWindow
    {
        public OutputWindow()
        {
            InitializeComponent();
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

        internal void SetText(string outputText)
        {
            outputWindowBox.Text = outputText;
            outputWindowBox.SelectionStart = outputWindowBox.Text.Length;
            outputWindowBox.ScrollToCaret();
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
            // expected input:
            // file:line:error code:description
            int errorLine = outputWindowBox.GetLineFromCharIndex(outputWindowBox.SelectionStart);
            string errorline = outputWindowBox.Lines[errorLine];
            int line = errorline.IndexOf(':', 2);
            if (line == -1)
            {
                return;
            }

            string file = errorline.Substring(0, line);
            int endLine = errorline.IndexOf(':', line + 1);
            if (endLine == -1)
            {
                return;
            }

            string realLine = errorline.Substring(line + 1, endLine - line - 1);
            int scrollToLine;
            if (int.TryParse(realLine, out scrollToLine))
            {
                DocumentService.GotoLine(file, scrollToLine);
            }
        }

        private void selectAllOuputButton_Click(object sender, EventArgs e)
        {
            outputWindowBox.SelectAll();
        }
    }
}