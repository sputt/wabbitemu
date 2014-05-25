using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.GUI.ToolBars;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class StatusBarService : IStatusBarService
    {
        private readonly WabbitcodeStatusBar _statusBar;

        public StatusBarService(WabbitcodeStatusBar statusBar)
        {
            _statusBar = statusBar;
        }

        public void SetCaretPosition(int line, int column)
        {
            _statusBar.SetCaretPosition(line, column);
        }

        public void SetText(string text)
        {
            _statusBar.SetText(text);
        }

        public void SetCodeCountInfo(CodeCountInfo info)
        {
            _statusBar.SetCodeCountInfo(info);
        }

        public void ShowProgressBar(bool show)
        {
            _statusBar.ShowProgressBar(show);
        }

        public void IncrementProgressBarProgress(int value)
        {
            _statusBar.IncrementProgressBarProgress(value);
        }

        public void ShowStatusBar()
        {
            _statusBar.Show();
        }

        public void HideStatusBar()
        {
            _statusBar.Hide();
        }
    }
}