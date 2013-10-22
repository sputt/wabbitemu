using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Globalization;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class StackViewer : ToolWindow
    {
        private WabbitcodeDebugger _debugger;
        private ushort _oldSp;
        private readonly IDockingService _dockingService;
        private readonly ISymbolService _symbolService;
        private const int AddressColIndex = 0;
        private const int StackDataColIndex = 1;

        public StackViewer(IDockingService dockingService, ISymbolService symbolService)
            : base(dockingService)
        {
            InitializeComponent();

            _dockingService = dockingService;
            _dockingService.MainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;
            _dockingService.MainForm.OnDebuggingEnded += mainForm_OnDebuggingEnded;
            _symbolService = symbolService;
        }

        void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
            _oldSp = 0xFFFF;
            _debugger.OnDebuggerStep += (o, args) => _dockingService.Invoke(UpdateStack);
            _debugger.OnDebuggerBreakpointHit += (o, args) => _dockingService.Invoke(UpdateStack);
            _debugger.OnDebuggerRunningChanged += (o, args) => _dockingService.Invoke(UpdateStack);
        }

        void mainForm_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            _debugger = null;
        }

        private void UpdateStack()
        {
            if (_debugger == null)
            {
                return;
            }

            int currentSP = _debugger.CPU.SP;

            // if someone has changed sp we dont want a really big callstack
            if (currentSP < 0xFE66)
            {
                return;
            }

            while (_oldSp != currentSP - 2)
            {
                if (_oldSp > currentSP - 2)
                {
                    AddStackData(_oldSp, _debugger.ReadShort(_oldSp));
                    _oldSp -= 2;
                }
                else
                {
                    RemoveLastRow();
                    _oldSp += 2;
                }
            }
        }

        private void AddStackData(int address, int data)
        {
            DataGridViewRow row = new DataGridViewRow();
            string dataString = data.ToString("X4");
            int page = _debugger.GetRelativePageNum((ushort)data);
            DocumentLocation key = _symbolService.ListTable.GetFileLocation(page, data, data >= 0x8000);
            if (key != null)
            {
                dataString += " (Possible Call)";
            }

            if (data > 0x4000)
            {
                List<string> possibles = _symbolService.SymbolTable.GetLabelsFromAddress(dataString);
                if (possibles.Count > 0)
                {
                    dataString = possibles.Aggregate(dataString, (current, value) => current + (" (" + value.ToLower() + ")"));
                }
            }

            row.CreateCells(stackView);
            row.Cells[AddressColIndex].Value = address.ToString("X4");
            row.Cells[StackDataColIndex].Value = dataString;
            stackView.Rows.Insert(0, row);
        }

        private void RemoveLastRow()
        {
            if (stackView.Rows.Count == 0)
            {
                // uh oh
                throw new Exception("Stack underflow");
            }

            stackView.Rows.Remove(stackView.Rows[0]);
        }

        internal void Clear()
        {
            stackView.Rows.Clear();
        }

        private void stackView_DoubleClick(object sender, EventArgs e)
        {
            if (stackView.SelectedRows.Count == 0)
            {
                return;
            }

            string stackValue = stackView.SelectedRows[0].Cells[1].Value.ToString();
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
    }
}