using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;
using System;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.DockingWindows
{
	public partial class BreakpointManagerWindow : ToolWindow
	{
	    public const string WindowIdentifier = "Breakpoint Manager";

        public override string WindowName
        {
            get
            {
                return WindowIdentifier;
            }
        }

		// TODO: this entire class looks nasty
		private readonly IDocumentService _documentService;
		private readonly IProjectService _projectService;

		public BreakpointManagerWindow()
		{
			InitializeComponent();

            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
		}

		private void UpdateManager()
		{
			breakpointGridView.Rows.Clear();
			var breakpoints = WabbitcodeBreakpointManager.Breakpoints.ToArray();
			foreach (var breakpoint in breakpoints)
			{
				int index = breakpointGridView.Rows.Add(new DataGridViewRow());
				IProject project = _projectService.Project;
				breakpointGridView.Rows[index].Cells[0].Value = breakpoint.Enabled;
				if (!project.IsInternal)
				{
					breakpointGridView.Rows[index].Cells[1].Value = Path.GetFileName(breakpoint.File) + ":" + breakpoint.LineNumber;
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
			IProject project = _projectService.Project;
			if (!project.IsInternal)
			{
				file = Path.Combine(project.ProjectDirectory, file);
			}
			int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
		    WabbitcodeBreakpoint breakpoint = WabbitcodeBreakpointManager.Breakpoints.FirstOrDefault(
                b => FileOperations.CompareFilePath(b.File, file) && b.LineNumber == lineNum);
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
			IProject project = _projectService.Project;
			if (!project.IsInternal)
			{
				file = Path.Combine(project.ProjectDirectory, file);
			}
			int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
			WabbitcodeBreakpointManager.RemoveBreakpoint(file, lineNum);
		}

		private void breakpointToolStripMenuItem_Click(object sender, EventArgs e)
		{
			NewBreakpointForm form = new NewBreakpointForm(DockingService, _documentService);
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
			WabbitcodeBreakpointManager.RemoveAllBreakpoints();
			UpdateManager();
		}

		private void delBreakToolStripButton_Click(object sender, EventArgs e)
		{
			string value = breakpointGridView.SelectedRows[0].Cells[1].Value.ToString();
			int splitter = value.IndexOf(':', 4);
			string file = value.Substring(0, splitter);
			IProject project = _projectService.Project;
			if (!project.IsInternal)
			{
				file = Path.Combine(project.ProjectDirectory, file);
			}

			int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
			WabbitcodeBreakpointManager.RemoveBreakpoint(file, lineNum);
		}

		private void disableAllToolStripButton_Click(object sender, EventArgs e)
		{
			foreach (var breakpoint in WabbitcodeBreakpointManager.Breakpoints)
			{
				breakpoint.Enabled = false;
			}

			UpdateManager();
		}

		private void gotoToolStripButton_Click(object sender, EventArgs e)
		{
		}

		#region IClipboardOperation

		public override void Copy()
		{
			// TODO: implement
		}

		#endregion
	}
}