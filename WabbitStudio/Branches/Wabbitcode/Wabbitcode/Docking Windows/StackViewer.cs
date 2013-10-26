using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using System;
using System.Globalization;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class StackViewer : ToolWindow
    {
        private IWabbitcodeDebugger _debugger;
        private readonly IDockingService _dockingService;
        private const int StackDataColIndex = 1;

        public StackViewer(IDockingService dockingService)
            : base(dockingService)
        {
            InitializeComponent();

            _dockingService = dockingService;
            _dockingService.MainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;
            _dockingService.MainForm.OnDebuggingEnded += mainForm_OnDebuggingEnded;
        }

        void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
            _debugger.OnDebuggerStep += (o, args) => _dockingService.Invoke(UpdateStack);
            _debugger.OnDebuggerRunningChanged += (o, args) => _dockingService.Invoke(UpdateStack);
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