using System;
using System.IO;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class BreakpointManager : ToolWindow
    {
        public BreakpointManager()
        {
            InitializeComponent();
        }

        private void breakpointToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NewBreakpointForm form = new NewBreakpointForm();
            form.ShowDialog();
        }

        public void UpdateManager()
        {
            breakpointGridView.Rows.Clear();
            foreach (WabbitcodeBreakpoint breakpoint in DebuggerService.Breakpoints)
            {
                int index = breakpointGridView.Rows.Add(new DataGridViewRow());
                breakpointGridView.Rows[index].Cells[0].Value = breakpoint.Enabled;
                if (!ProjectService.IsInternal)
                    breakpointGridView.Rows[index].Cells[1].Value =
                        breakpoint.file.Remove(0, ProjectService.ProjectDirectory.Length + 1) + ":" + breakpoint.lineNumber;
                else
                    breakpointGridView.Rows[index].Cells[1].Value = breakpoint.file + ":" + breakpoint.lineNumber;
                if (breakpoint.breakCondition != null)
                    breakpointGridView.Rows[index].Cells[2].Value = breakpoint.breakCondition.Count;
                else
                    breakpointGridView.Rows[index].Cells[2].Value = "No Condition";
                breakpointGridView.Rows[index].Cells[3].Value = breakpoint.hitCountConditionNumber;
                breakpointGridView.Rows[index].Cells[4].Value = "$" + breakpoint.Address.ToString("X");
                breakpointGridView.Rows[index].Cells[5].Value = "$" + breakpoint.Page.ToString("X");
                breakpointGridView.Rows[index].Cells[6].Value = breakpoint.IsRam;
            }
        }

        private void delBreakToolStripButton_Click(object sender, EventArgs e)
        {
            string value = breakpointGridView.SelectedRows[0].Cells[1].Value.ToString();
            int splitter = value.IndexOf(':', 4);
            string file = value.Substring(0, splitter);
            if (!ProjectService.IsInternal)
                file = Path.Combine(ProjectService.ProjectDirectory, file);
            int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
            DocumentService.GotoFile(file);
            DocumentService.ActiveDocument.RemoveBreakpoint(lineNum);
        }

        private void delAllBreakToolStripButton_Click(object sender, EventArgs e)
        {
            foreach (NewEditor child in DockingService.Documents)
                child.ClearBreakpoints();
            DebuggerService.Breakpoints.Clear();
            UpdateManager();
        }

        private void disableAllToolStripButton_Click(object sender, EventArgs e)
        {
			for (int i = 0; i < DebuggerService.Breakpoints.Count; i++)
            {
				WabbitcodeBreakpoint breakpoint = DebuggerService.Breakpoints[i];
                breakpoint.Enabled = false;
				DebuggerService.Breakpoints[i] = breakpoint;
            }
            UpdateManager();
        }

        private void gotoToolStripButton_Click(object sender, EventArgs e)
        {

        }

        private void breakpointGridView_UserDeletingRow(object sender, DataGridViewRowCancelEventArgs e)
        {
            string value = e.Row.Cells[1].Value.ToString();
            int splitter = value.IndexOf(':', 4);
            string file = value.Substring(0, splitter);
            if (!ProjectService.IsInternal)
                file = Path.Combine(ProjectService.ProjectDirectory, file);
            int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
            DocumentService.GotoFile(file);
            DocumentService.ActiveDocument.RemoveBreakpoint(lineNum);
        }

        private void colButtonClick(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem) sender;
            int index = item.Owner.Items.IndexOf(item);
            breakpointGridView.Columns[index].Visible = item.Checked;
        }

        private void breakpointGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex != 0 || breakpointGridView.SelectedRows.Count == 0 || breakpointGridView.SelectedRows[0].Cells[1].Value == null)
                return;
            string value = breakpointGridView.SelectedRows[0].Cells[1].Value.ToString();
            int splitter = value.IndexOf(':', 4);
            string file = value.Substring(0, splitter);
            if (!ProjectService.IsInternal)
                file = Path.Combine(ProjectService.ProjectDirectory, file);
            int lineNum = Convert.ToInt32(value.Substring(splitter + 1, value.Length - splitter - 1));
			WabbitcodeBreakpoint breakpoint = DebuggerService.FindBreakpoint(file, lineNum);
			if (breakpoint == null)
				return;
            breakpoint.Enabled = (bool)breakpointGridView.Rows[e.RowIndex].Cells[0].Value;
        }
    }
}