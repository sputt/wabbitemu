using System;
using System.Linq;
using Revsoft.Wabbitcode.GUI;
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
    }

    public interface IStatusBarService : IService
    {
        /// <summary>
        /// Updates the caret position in the status bar
        /// </summary>
        /// <param name="line">Line of the editor. -1 specifies to hide this field</param>
        /// <param name="column">Column of the editor. -1 specifies to hide this field</param>
        void SetCaretPosition(int line, int column);
        /// <summary>
        /// Sets the text of the largest section of the status bar
        /// </summary>
        /// <param name="text">Text to display in the status bar</param>
        void SetText(string text);
        /// <summary>
        /// Updates the code info with the latest size, min, and max string in the status bar.
        /// </summary>
        /// <param name="info">Code count information containing min runtime, max runtime and size. If null the field will be hidden</param>
        void SetCodeCountInfo(CodeCountInfo info);
    }
}
