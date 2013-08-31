using System.Linq;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class CallStack : ToolWindow
	{
		private WabbitcodeDebugger _debugger;
		private ushort _oldSp;
		private readonly IDockingService _dockingService;
		private readonly ISymbolService _symbolService;

		public CallStack(IDockingService dockingService, ISymbolService symbolService)
			: base(dockingService)
		{
			InitializeComponent();

			_dockingService = dockingService;
			_dockingService.MainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;
			_symbolService = symbolService;
		}

		void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
		{
			_debugger = e.Debugger;
			_oldSp = 0xFFFF;
			_debugger.OnDebuggerBreakpointHit += (o, args) => _dockingService.Invoke(UpdateStack);
		}

		private void UpdateStack()
		{
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
			int page = _debugger.GetPageNum((ushort)address);
			DocumentLocation key = _symbolService.ListTable.GetFileLocation(data, page);
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

			callStackView.Rows.Insert(0, row);
			callStackView.Rows[0].Cells[0].Value = address.ToString("X4");
			callStackView.Rows[0].Cells[1].Value = dataString;
		}

		private void RemoveLastRow()
		{
			if (callStackView.Rows.Count == 0)
			{
				// uh oh
				throw new Exception("Stack underflow");
			}

			callStackView.Rows.Remove(callStackView.Rows[0]);
		}

		internal void Clear()
		{
			callStackView.Rows.Clear();
		}

		private void callStackView_DoubleClick(object sender, EventArgs e)
		{
			if (callStackView.SelectedRows.Count == 0)
			{
				return;
			}

			string stackValue = callStackView.Rows[callStackView.SelectedRows[0].Index].Cells[1].Value.ToString();
			stackValue = stackValue.TrimStart().Substring(0, 4);
			ushort address = ushort.Parse(stackValue, NumberStyles.HexNumber);
			_debugger.GotoAddress(address);
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
	}
}