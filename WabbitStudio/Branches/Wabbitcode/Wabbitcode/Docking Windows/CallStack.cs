using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class CallStack : ToolWindow
    {
        public CallStack()
        {
            InitializeComponent();
        }

        public void AddStackData(int address, int data)
        {
            DataGridViewRow row = new DataGridViewRow();
            string dataString = data.ToString("X4");
            ListFileKey key = DebuggerService.GetListKey((ushort) data, DebuggerService.GetPageNum((ushort) address));
            if (key != null)
                dataString += " (Possible Call)";
            if (data > 0x4000)
            {
                List<string> possibles = DebuggerService.SymbolTable.FindEntryByValue(data.ToString("X4"));
                if (possibles.Count > 0)
                    foreach (string value in possibles)
                        dataString += " (" + value.ToLower() +")";
            }
            callStackView.Rows.Insert(0, row);
            callStackView.Rows[0].Cells[0].Value = address.ToString("X4");
            callStackView.Rows[0].Cells[1].Value = dataString;
        }

        public void RemoveLastRow()
        {
			if (callStackView.Rows.Count == 0)
			{
                //uh oh
				throw new Exception("Stack underflow");
			}
            callStackView.Rows.Remove(callStackView.Rows[0]);
        }

        private void callStackView_DoubleClick(object sender, System.EventArgs e)
        {
			if (callStackView.SelectedRows.Count == 0)
				return;
            string stackValue = callStackView.Rows[callStackView.SelectedRows[0].Index].Cells[1].Value.ToString();
            stackValue = stackValue.TrimStart().Substring(0, 4);
			ushort address = ushort.Parse(stackValue, NumberStyles.HexNumber);
			DebuggerService.GotoAddress(address);
        }

        public override void Copy()
        {
            Clipboard.SetDataObject(callStackView.GetClipboardContent());
        }

		internal void Clear()
		{
			callStackView.Rows.Clear();
		}
	}
}