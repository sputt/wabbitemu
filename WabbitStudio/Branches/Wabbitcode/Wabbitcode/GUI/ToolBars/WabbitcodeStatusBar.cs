using System.Drawing;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Assembler;

namespace Revsoft.Wabbitcode.GUI.ToolBars
{
    public sealed class WabbitcodeStatusBar : StatusStrip
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
            SuspendLayout();

            Dock = DockStyle.None;
            Location = new Point(0, 0);
            Name = "statusBar";
            Size = new Size(990, 22);
            TabIndex = 4;

            Items.AddRange(new ToolStripItem[]
            {
                _statusLabel,
                _progressBar,
                _lineCodeInfo,
                _lineStatusLabel,
                _colStatusLabel
            });
            Name = "statusBar";

            ResumeLayout(false);
        }

        public void SetCodeCountInfo(CodeCountInfo info)
        {
            if (InvokeRequired)
            {
                this.BeginInvoke(() => SetCodeCountInfo(info));
                return;
            }
            _lineCodeInfo.Text = info == null ? string.Empty : string.Format("Min: {0} Max: {1} Size: {2}", info.Min, info.Max, info.Size);
        }

        public void SetText(string text)
        {
            if (InvokeRequired)
            {
                this.BeginInvoke(() => SetText(text));
                return;
            }
            _statusLabel.Text = text;
        }

        public void SetCaretPosition(int line, int column)
        {
            if (InvokeRequired)
            {
                this.BeginInvoke(() => SetCaretPosition(line, column));
                return;
            }
            string lineString = line == -1 ? string.Empty : "Ln: " + line;
            string colString = column == -1 ? string.Empty : "Col: " + column;
            _lineStatusLabel.Text = lineString;
            _colStatusLabel.Text = colString;
        }

        public void ShowProgressBar(bool show)
        {
            if (InvokeRequired)
            {
                this.BeginInvoke(() => ShowProgressBar(show));
                return;
            }

            _progressBar.Visible = show;
        }

        public void IncrementProgressBarProgress(int value)
        {
            if (InvokeRequired)
            {
                this.BeginInvoke(() => IncrementProgressBarProgress(value));
                return;
            }

            if (_progressBar.Value + value > _progressBar.Maximum)
            {
                _progressBar.Value = _progressBar.Maximum;
                return;
            }
            _progressBar.Value += value;
        }
    }
}