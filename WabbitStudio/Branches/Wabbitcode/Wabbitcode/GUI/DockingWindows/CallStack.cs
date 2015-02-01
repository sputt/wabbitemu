using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class CallStack : ToolWindow
    {
        private readonly List<DocumentLocation> _callLocations = new List<DocumentLocation>();

        private IWabbitcodeDebugger _debugger;

        public CallStack()
        {
            InitializeComponent();

            var debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            debuggerService.OnDebuggingStarted += mainForm_OnDebuggingStarted;
            debuggerService.OnDebuggingEnded += mainForm_OnDebuggingEnded;
        }

        private void mainForm_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            _debugger.DebuggerRunningChanged -= OnDebuggerOnDebuggerRunningChanged;
            _debugger.DebuggerStep -= OnDebuggerOnDebuggerStep;
            _debugger = null;
        }

        private void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
            _debugger.DebuggerStep += OnDebuggerOnDebuggerStep;
            _debugger.DebuggerRunningChanged += OnDebuggerOnDebuggerRunningChanged;
        }

        private void OnDebuggerOnDebuggerRunningChanged(object o, DebuggerRunningEventArgs args)
        {
            this.Invoke(() =>
            {
                if (!args.Running)
                {
                    UpdateStack();
                }
                else
                {
                    _callLocations.Clear();
                    callStackView.Rows.Clear();
                }

                EnablePanel(!args.Running);
            });
        }

        private void OnDebuggerOnDebuggerStep(object o, DebuggerStepEventArgs args)
        {
            this.Invoke(UpdateStack);
            EnablePanel(true);
        }

        private void UpdateStack()
        {
            _callLocations.Clear();

            // Add instruction pointer first
            DocumentLocation currentLoc = _debugger.GetAddressLocation(_debugger.CPU.PC);
            _callLocations.Add(new DocumentLocation(currentLoc.FileName, currentLoc.LineNumber - 1));

            callStackView.Rows.Clear();
            var dataGridViewRows = new List<DataGridViewRow>();
            foreach (var call in _debugger.CallStack.Reverse())
            {
                var row = new DataGridViewRow();

                CallerInformation callerInformation = call.CallerInformation;
                string callType = callerInformation.Command + " " + callerInformation.Condition;
                row.CreateCells(callStackView, callType, callerInformation.CallName);
                dataGridViewRows.Add(row);

                currentLoc = call.CallerInformation.DocumentLocation;
                _callLocations.Add(new DocumentLocation(currentLoc.FileName, currentLoc.LineNumber - 1));
            }

            // We added an extra location for the instruction pointer, so add an extra row for the top level call
            var appRow = new DataGridViewRow();
            appRow.CreateCells(callStackView, "Top level", "OS");
            dataGridViewRows.Add(appRow);
            callStackView.Rows.AddRange(dataGridViewRows.ToArray());
        }

        private void callStackView_DoubleClick(object sender, EventArgs e)
        {
            if (callStackView.SelectedRows.Count == 0)
            {
                return;
            }

            DocumentLocation location = _callLocations[callStackView.SelectedRows[0].Index];
            AbstractUiAction.RunCommand(new GotoLineAction(location));
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