using System;
using System.Drawing;
using System.Windows.Forms;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class OutputWindow : ToolWindow
    {
        public OutputWindow()
        {
            InitializeComponent();
            outputWindowBox.ContextMenu = contextMenu1;
        }

        private void outputWindowBox_DoubleClick(object sender, EventArgs e)
        {
			int errorLine = outputWindowBox.GetLineFromCharIndex(outputWindowBox.SelectionStart);
			string errorline = outputWindowBox.Lines[errorLine];
			int line = errorline.IndexOf(':', 2);
			if (line == -1)
				return;
			string file = errorline.Substring(0, line);
			//if (!file.Contains("\\"))
			//	file = originaldir + '\\' + file;
			int endLine = errorline.IndexOf(':', line + 1);
			if (endLine == -1)
				return;
			string realLine = errorline.Substring(line + 1, endLine - line - 1);
			int scrollToLine = Convert.ToInt32(realLine);
			DocumentService.GotoLine(file, scrollToLine);
        }

        public void HighlightOutput()
        {
            int i = 0;
            foreach (String line in outputWindowBox.Lines)
            {
                if (line.Contains("error"))
                {
                    outputWindowBox.Select(outputWindowBox.GetFirstCharIndexFromLine(i),
                                                        outputWindowBox.GetFirstCharIndexFromLine(i + 1) -
                                                        outputWindowBox.GetFirstCharIndexFromLine(i));
                    outputWindowBox.SelectionColor = Color.Red;
                }
                if (line.Contains("warning"))
                {
                    outputWindowBox.Select(outputWindowBox.GetFirstCharIndexFromLine(i),
                                                        outputWindowBox.GetFirstCharIndexFromLine(i + 1) -
                                                        outputWindowBox.GetFirstCharIndexFromLine(i));
                    outputWindowBox.SelectionColor = Color.Gold;
                }
                i++;
            }
        }

        private void copyOutputButton_Click(object sender, EventArgs e)
        {
            outputWindowBox.Copy();
        }

        private void selectAllOuputButton_Click(object sender, EventArgs e)
        {
            outputWindowBox.SelectAll();
        }

		internal void UpdateFont(Font font)
		{
			outputWindowBox.Font = font;
		}

		public override void Copy()
		{
			outputWindowBox.Copy();
		}

		internal void SetText(string outputText)
		{
			outputWindowBox.Text = outputText;
			outputWindowBox.SelectionStart = outputWindowBox.Text.Length;
			outputWindowBox.ScrollToCaret();
		}

		internal void AddText(string outputText)
		{
			outputWindowBox.Text += outputText;
		}

        public IntPtr OutputBoxHandle { 
            get 
            {
                if (outputWindowBox.InvokeRequired)
                    return (IntPtr)outputWindowBox.Invoke(new GetHandleDelegate(GetHandle));
                else
                    return outputWindowBox.Handle; 
            } 
        }

        private delegate IntPtr GetHandleDelegate();
        private IntPtr GetHandle()
        {
            return outputWindowBox.Handle;
        }

        private delegate void ClearOutputDelegate();
        internal void ClearOutput()
        {
            if (outputWindowBox.InvokeRequired)
                outputWindowBox.Invoke(new ClearOutputDelegate(ClearOutput));
            else
                outputWindowBox.Clear();
        }
    }
}