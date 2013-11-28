using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using System;
using System.Globalization;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.DockingWindows
{
    public partial class StackViewer : ToolWindow
    {
        public const string WindowIdentifier = "Stack Viewer";
        public override string WindowName
        {
            get
            {
                return WindowIdentifier;
            }
        }

        private IWabbitcodeDebugger _debugger;
        private const int StackDataColIndex = 1;

        public StackViewer()
        {
            InitializeComponent();

            IDebuggerService debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
            debuggerService.OnDebuggingStarted += mainForm_OnDebuggingStarted;
            debuggerService.OnDebuggingEnded += mainForm_OnDebuggingEnded;
        }

        void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
            _debugger.OnDebuggerStep += (o, args) =>
            {
                this.Invoke(UpdateStack);
                EnablePanel(true);
            };
            _debugger.OnDebuggerRunningChanged += (o, args) =>
            {
                this.Invoke(UpdateStack);
                EnablePanel(!args.Running);
            };
        }

        void mainForm_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            _debugger = null;
        }

        private void UpdateStack()
        {
            stackView.Rows.Clear();
            var rowData = new List<DataGridViewRow>();
            foreach (var data in _debugger.MachineStack.Reverse())
            {
                var row = new DataGridViewRow();
                row.CreateCells(stackView, data.Address.ToString("X").PadLeft(4, '0'), 
                    data.Data.ToString("X").PadLeft(4, '0'));
                rowData.Add(row);
            }

            stackView.Rows.AddRange(rowData.ToArray());
        }

        private void stackView_DoubleClick(object sender, EventArgs e)
        {
            if (stackView.SelectedRows.Count == 0)
            {
                return;
            }

            string stackValue = stackView.SelectedRows[0].Cells[StackDataColIndex].Value.ToString();
            stackValue = stackValue.TrimStart().Substring(0, 4);
            ushort address = ushort.Parse(stackValue, NumberStyles.HexNumber);
            _debugger.GotoAddress(address);
        }

        #region IClipboardOperation

        public override void Copy()
        {
            if (stackView == null)
            {
                return;
            }

            DataObject data = stackView.GetClipboardContent();
            if (data != null)
            {
                Clipboard.SetDataObject(data);
            }
        }

        #endregion

        private void stackView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            stackView.Rows[e.RowIndex].DefaultCellStyle.ForeColor = Enabled ? Color.Black : Color.Gray;
        }
    }
}