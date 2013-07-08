using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Forms;

using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class StackViewer : ToolWindow
    {
        private MainFormRedone mainForm;
        private WabbitcodeDebugger debugger;

        public StackViewer(MainFormRedone mainForm)
        {
            InitializeComponent();

            this.mainForm = mainForm;
        }

        public void AddStackData(int address, int data)
        {
            string dataString = data.ToString("X4");
            if (data > 0x8000)
            {
                List<string> possibles = debugger.SymbolTable.FindEntryByValue(data.ToString("X4"));
                if (possibles.Count > 0)
                {
                    foreach (string value in possibles)
                    {
                        dataString += " (" + value.ToLower() + ")";
                    }
                }
            }

            DataGridViewRow row = new DataGridViewRow();
            stackView.Rows.Insert(0, row);
            stackView.Rows[0].Cells[0].Value = address.ToString("X4");
            stackView.Rows[0].Cells[1].Value = dataString;
        }

        public override void Copy()
        {
            Clipboard.SetDataObject(stackView.GetClipboardContent());
        }

        public void RemoveLastRow()
        {
            if (stackView.Rows.Count == 0)
            {
                throw new Exception("Stack underflow");
            }

            stackView.Rows.Remove(stackView.Rows[0]);
        }

        internal void Clear()
        {
            stackView.Rows.Clear();
        }

        private void stackView_DoubleClick(object sender, System.EventArgs e)
        {
            if (stackView.SelectedRows.Count == 0)
            {
                return;
            }

            ushort address = ushort.Parse(
                                 stackView.Rows[stackView.SelectedRows[0].Index].Cells[1].Value.ToString(),
                                 NumberStyles.HexNumber);
            debugger.GotoAddress(address);
        }
    }
}