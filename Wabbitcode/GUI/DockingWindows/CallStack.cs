using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class CallStack : ToolWindow
    {
        public const string WindowIdentifier = "Call Stack";

        public override string WindowName
        {
            get { return WindowIdentifier; }
        }

        private IWabbitcodeDebugger _debugger;

        private readonly List<DocumentLocation> _callLocations = new List<DocumentLocation>();

        public CallStack()
        {
            InitializeComponent();

            IDebuggerService debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            debuggerService.OnDebuggingStarted += mainForm_OnDebuggingStarted;
        }

        private void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
            _debugger.DebuggerStep += (o, args) =>
            {
                this.Invoke(UpdateStack);
                EnablePanel(true);
            };
            _debugger.DebuggerRunningChanged += (o, args) =>
            {
                this.Invoke(UpdateStack);
                EnablePanel(!args.Running);
            };
        }

        private void UpdateStack()
        {
            _callLocations.Clear();
            callStackView.Rows.Clear();
            var dataGridViewRows = new List<DataGridViewRow>();
            foreach (var call in _debugger.CallStack.Reverse())
            {
                var row = new DataGridViewRow();
                row.CreateCells(callStackView, call.CallType, call.CallName);
                dataGridViewRows.Add(row);
                _callLocations.Add(call.CallLocation);
            }

            callStackView.Rows.AddRange(dataGridViewRows.ToArray());
        }

        private void callStackView_DoubleClick(object sender, EventArgs e)
        {
            if (callStackView.SelectedRows.Count == 0)
            {
                return;
            }

            DocumentLocation location = _callLocations[callStackView.SelectedRows[0].Index];
            new GotoLineAction(location).Execute();
        }

        #region IClipboardOperation

        public override void Copy()
        {
            if (callStackView == null)
            {
                return;
            }

            DataObject data = callStackView.GetClipboardContent();
            if (data != null)
            {
                Clipboard.SetDataObject(data);
            }
        }

        #endregion

        private void callStackView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            callStackView.Rows[e.RowIndex].DefaultCellStyle.ForeColor = Enabled ? Color.Black : Color.Gray;
        }
    }
}