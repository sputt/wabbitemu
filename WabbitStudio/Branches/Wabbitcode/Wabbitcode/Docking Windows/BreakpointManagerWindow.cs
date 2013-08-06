using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.IO;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class BreakpointManagerWindow : ToolWindow
	{
		private WabbitcodeDebugger _debugger;
		private readonly IDockingService _dockingService;
		private readonly IDocumentService _documentService;
		private readonly IProject _project;

		public BreakpointManagerWindow(IDockingService dockingService, IDocumentService documentService, IProject project)
			: base(dockingService)
		{
			InitializeComponent();

			_dockingService = dockingService;
			_dockingService.MainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;
			_documentService = documentService;

			_project = project;
		}

		void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
		{
			_debugger = e.Debugger;
		}

		private void UpdateManager()
		{
			if (_debugger == null)
			{
				return;
			}
			breakpointGridView.Rows.Clear();
			foreach (WabbitcodeBreakpoint breakpoint in _debugger.Breakpoints)
			{
				int index = breakpointGridView.Rows.Add(new DataGridViewRow());
				breakpointGridView.Rows[index].Cells[0].Value = breakpoint.Enabled;
				if (!_project.IsInternal)
				{
					breakpointGridView.Rows[index].Cells[1].Value = breakpoint.File.Remove(0, _project.ProjectDirectory.Length + 1) + ":" + breakpoint.LineNumber;
				}
				else
				{
					breakpointGridView.Rows[index].Cells[1].Value = breakpoint.File + ":" + breakpoint.LineNumber;
				}

				if (breakpoint.BreakConditions != null)
				{
					breakpointGridView.Rows[index].Cells[2].Value = breakpoint.BreakConditions.Count;
				}
				else
				{
					breakpointGridView.Rows[index].Cells[2].Value = "No Condition";
				}

				breakpointGridView.Rows[index].Cells[3].Value = breakpoint.HitCountConditionNumber;
				breakpointGridView.Rows[index].Cells[4].Value = "$" + breakpoint.Address.ToString("X");
				breakpointGridView.Rows[index].Cells[5].Value = "$" + breakpoint.Page.ToString("X");
				breakpointGridView.Rows[index].Cells[6].Value = breakpoint.IsRam;
			}
		}

		private void breakpointGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
		{
			if (e.ColumnIndex != 0 || breakpointGridView.SelectedRows.Count == 0 || breakpointGridView.SelectedRows[0].Cells[1].Value == null)
			{
				return;
			}
			string value = breakpointGridView.SelectedRows[0].Cells[1].Value.ToString();
			int splitter = value.IndexOf(':', 4);
			string file = value.Substring(0, splitter);
			if (!_project.IsInternal)
			{
				file = Path.Combine(_project.ProjectDirectory, file);
			}
			int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
			WabbitcodeBreakpoint breakpoint = _debugger.FindBreakpoint(file, lineNum);
			if (breakpoint == null)
			{
				return;
			}
			breakpoint.Enabled = (bool)breakpointGridView.Rows[e.RowIndex].Cells[0].Value;
		}

		private void breakpointGridView_UserDeletingRow(object sender, DataGridViewRowCancelEventArgs e)
		{
			string value = e.Row.Cells[1].Value.ToString();
			int splitter = value.IndexOf(':', 4);
			string file = value.Substring(0, splitter);
			if (!_project.IsInternal)
			{
				file = Path.Combine(_project.ProjectDirectory, file);
			}
			int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
			_documentService.GotoFile(file);
			_documentService.ActiveDocument.RemoveBreakpoint(lineNum);
		}

		private void breakpointToolStripMenuItem_Click(object sender, EventArgs e)
		{
			NewBreakpointForm form = new NewBreakpointForm(_dockingService, _documentService);
			form.ShowDialog();
		}

		private void ColButtonClick(object sender, EventArgs e)
		{
			ToolStripMenuItem item = (ToolStripMenuItem)sender;
			int index = item.Owner.Items.IndexOf(item);
			breakpointGridView.Columns[index].Visible = item.Checked;
		}

		private void delAllBreakToolStripButton_Click(object sender, EventArgs e)
		{
			foreach (NewEditor child in _dockingService.Documents)
			{
				child.ClearBreakpoints();
			}

			_debugger.Breakpoints.Clear();
			UpdateManager();
		}

		private void delBreakToolStripButton_Click(object sender, EventArgs e)
		{
			string value = breakpointGridView.SelectedRows[0].Cells[1].Value.ToString();
			int splitter = value.IndexOf(':', 4);
			string file = value.Substring(0, splitter);
			if (!_project.IsInternal)
			{
				file = Path.Combine(_project.ProjectDirectory, file);
			}

			int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
			_documentService.GotoFile(file);
			_documentService.ActiveDocument.RemoveBreakpoint(lineNum);
		}

		private void disableAllToolStripButton_Click(object sender, EventArgs e)
		{
			for (int i = 0; i < _debugger.Breakpoints.Count; i++)
			{
				WabbitcodeBreakpoint breakpoint = _debugger.Breakpoints[i];
				breakpoint.Enabled = false;
				_debugger.Breakpoints[i] = breakpoint;
			}

			UpdateManager();
		}

		private void gotoToolStripButton_Click(object sender, EventArgs e)
		{
		}
	}
}