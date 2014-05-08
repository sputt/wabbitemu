using System;
using System.Linq;
using Revsoft.Wabbitcode.GUI.ToolBars;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    internal class StatusBarService : IStatusBarService
    {
        private WabbitcodeStatusBar _statusBar;

        #region IService

        public void DestroyService()
        {
        }

        public void InitService(params object[] objects)
        {
            if (objects.Length != 1)
            {
                throw new ArgumentException("Wrong number of params for StatusBarService");
            }

            var statusBar = objects.First() as WabbitcodeStatusBar;
            if (statusBar == null)
            {
                throw new ArgumentException("Expected first argument to be a status bar");
            }

            _statusBar = statusBar;
        }

        #endregion

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
