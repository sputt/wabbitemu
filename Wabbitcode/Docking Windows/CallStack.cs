using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Windows.Forms;
using IFileReaderService = Revsoft.Wabbitcode.Services.IFileReaderService;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class CallStack : ToolWindow
	{
		private WabbitcodeDebugger _debugger;
		private ushort _oldSp;

        private readonly List<int> _stackList = new List<int>();
        private readonly Stack<DocumentLocation> _calledAddrList = new Stack<DocumentLocation>();
        private readonly IDockingService _dockingService;
        private readonly IDocumentService _documentService;
	    private readonly IFileReaderService _fileReaderService;
		private readonly ISymbolService _symbolService;

        private const int CallTypeIndex = 0;
        private const int NameIndex = 1;

		public CallStack(IDockingService dockingService, IDocumentService documentService,
            IFileReaderService fileReaderService, ISymbolService symbolService)
			: base(dockingService)
		{
			InitializeComponent();

			_dockingService = dockingService;
			_dockingService.MainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;
		    _documentService = documentService;
		    _fileReaderService = fileReaderService;
			_symbolService = symbolService;
		}

		void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
		{
			_debugger = e.Debugger;
            // TODO: fix for non apps
            _oldSp = 0xFFDF;
			_debugger.OnDebuggerStep += (o, args) => _dockingService.Invoke(UpdateStack);
			_debugger.OnDebuggerBreakpointHit += (o, args) => _dockingService.Invoke(UpdateStack);
			_debugger.OnDebuggerRunningChanged += (o, args) => _dockingService.Invoke(UpdateStack);
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
            int page = _debugger.GetRelativePageNum((ushort)data);
            // TODO: fix for non apps
            if (data < 0x4000 || data >= 0x8000)
            {
                return;
            }

            DocumentLocation key;
            do
            {
                key = _symbolService.ListTable.GetFileLocation(page, --data, data >= 0x8000);
            } while (key == null);
            string line = _fileReaderService.GetLine(key.FileName, key.LineNumber);
            Regex callRegex = new Regex(@"\s*(?<command>\w*call)[\(?|\s]\s*((?<condition>z|nz|c|nc),\s*)?(?<call>\w*?)\)?\s*(;.*)?$", RegexOptions.Compiled | RegexOptions.IgnoreCase);
            Match match = callRegex.Match(line);
            if (!match.Success)
            {
                return;
            }

            _calledAddrList.Push(key);
            string callTypeString = match.Groups["command"].Value + " " + match.Groups["condition"].Value;
            string nameString = match.Groups["call"].Value;

            row.CreateCells(callStackView);
            row.Cells[CallTypeIndex].Value = callTypeString;
            row.Cells[NameIndex].Value = nameString;
            callStackView.Rows.Insert(0, row);
            _stackList.Add(address + 2);
        }

		private void RemoveLastRow()
		{
		    int sp = _debugger.CPU.SP;
		    if (!_stackList.Contains(sp))
		    {
		        return;
		    }

			if (callStackView.Rows.Count == 0)
			{
				// uh oh
				throw new Exception("Stack underflow");
			}

            _stackList.Remove(sp);
		    _calledAddrList.Pop();
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

		    DocumentLocation location = _calledAddrList.ElementAt(callStackView.SelectedRows[0].Index);
		    _documentService.GotoLine(location.FileName, location.LineNumber);
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