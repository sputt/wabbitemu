using System.Drawing;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Assembler;

namespace Revsoft.Wabbitcode.GUI.ToolBars
{
    sealed class WabbitcodeStatusBar : StatusStrip
    {
        private readonly ToolStripStatusLabel _statusLabel = new ToolStripStatusLabel
        {
            Spring = true,
            TextAlign = ContentAlignment.MiddleLeft
        };

        private readonly ToolStripProgressBar _progressBar = new ToolStripProgressBar
        {
            Visible = false
        };

        private readonly ToolStripStatusLabel _lineCodeInfo = new ToolStripStatusLabel();
        private readonly ToolStripStatusLabel _lineStatusLabel = new ToolStripStatusLabel();
        private readonly ToolStripStatusLabel _colStatusLabel = new ToolStripStatusLabel();

        public WabbitcodeStatusBar()
        {
            Items.AddRange(new ToolStripItem[] {
            _statusLabel,
            _progressBar,
            _lineCodeInfo,
            _lineStatusLabel,
            _colStatusLabel});
            Name = "statusBar";
        }

        public void SetCodeCountInfo(CodeCountInfo info)
        {
            if (InvokeRequired)
            {
                this.Invoke(() => SetCodeCountInfo(info));
                return;
            }
            _lineCodeInfo.Text = info == null ? string.Empty : string.Format("Min: {0} Max: {1} Size: {2}", info.Min, info.Max, info.Size);
        }

        public void SetText(string text)
        {
            if (InvokeRequired)
            {
                this.Invoke(() => SetText(text));
                return;
            }
            _statusLabel.Text = text;
        }

        public void SetCaretPosition(int line, int column)
        {
            if (InvokeRequired)
            {
                this.Invoke(() => SetCaretPosition(line, column));
                return;
            }
            string lineString = line == -1 ? string.Empty : "Ln: " + line;
            string colString = column == -1 ? string.Empty : "Col: " + column;
            _lineStatusLabel.Text = lineString;
            _colStatusLabel.Text = colString;
        }
    }
}
