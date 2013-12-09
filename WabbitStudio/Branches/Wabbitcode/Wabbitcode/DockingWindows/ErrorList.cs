using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;


namespace Revsoft.Wabbitcode.DockingWindows
{
	public partial class ErrorList : ToolWindow
	{
	    public const string WindowIdentifier = "Error List";
        public override string WindowName
        {
            get { return WindowIdentifier; }
        }

	    private const string FcreateFile = "Built-in fcreate";
		private int _errors;
		private int _warnings;
	    private readonly IProjectService _projectService;

		public ErrorList()
		{
			InitializeComponent();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            var assemblerService = ServiceFactory.Instance.GetServiceInstance<IAssemblerService>();

			errorGridView.ContextMenu = contextMenu1;
			assemblerService.AssemblerFileFinished += Instance_AssemblerFileFinished;
			assemblerService.AssemblerProjectFinished += Instance_AssemblerProjectFinished;
		}

		#region IClipboardOperation

		public override void Copy()
		{
			var data = errorGridView.GetClipboardContent();
			if (data != null)
			{
				Clipboard.SetDataObject(data);
			}
		}

		#endregion

	    private void ParseOutput(IEnumerable<BuildError> parsedErrors)
		{
			_errors = 0;
			_warnings = 0;
			errorGridView.Rows.Clear();
			foreach (BuildError error in parsedErrors)
			{
				if (error.IsWarning)
				{
					AddWarning(error.Description, error.File, error.LineNum.ToString(), warnToolButton.Checked);
				}
				else
				{
					AddError(error.Description, error.File, error.LineNum.ToString(), errorToolButton.Checked);
				}
			}

			errorToolButton.Text = _errors + " Errors";
			warnToolButton.Text = _warnings + " Warnings";
		}

		private void AddError(string description, string file, string line, bool visible)
		{
		    AddRow(description, file, line, visible, false);
		}

	    private void AddRow(string description, string file, string line, bool visible, bool isWarning)
	    {
	        DataGridViewRow row = new DataGridViewRow();
	        row.CreateCells(errorGridView);

	        row.Cells[0].Value = imageListIcons.Images[isWarning ? 1 : 0];
	        row.Cells[1].Value = isWarning ? ++_warnings : ++_errors;
	        row.Cells[2].Value = description;
	        row.Cells[3].Value = Path.GetFileName(file);
	        row.Cells[3].Tag = file;
	        row.Cells[4].Value = Convert.ToInt32(line);
	        row.Tag = isWarning ? "Warning" : "Error";
	        row.Visible = visible;
	        errorGridView.Rows.Add(row);
	    }

	    private void AddWarning(string description, string file, string line, bool visible)
		{
            AddRow(description, file, line, visible, true);
		}

		private void Copy(object sender, EventArgs e)
		{
			Copy();
		}

		private void errorGridView_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex < 0)
			{
				return;
			}

			int line = (int)errorGridView.Rows[e.RowIndex].Cells[4].Value;
			string file = errorGridView.Rows[e.RowIndex].Cells[3].Tag.ToString();
		    if (file != FcreateFile)
		    {
		        new GotoLineAction(file, line - 1).Execute();
		    }
		}

		private void errorGridView_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Right)
			{
				return;
			}

            string file = errorGridView.SelectedRows[0].Cells[3].Tag.ToString();
		    gotoMenuItem.Enabled = file != FcreateFile;
			contextMenu1.Show(errorGridView, new Point(e.X, e.Y));
		}

		private void errorToolButton_CheckedChanged(object sender, EventArgs e)
		{
		    foreach (DataGridViewRow row in errorGridView.Rows.Cast<DataGridViewRow>()
                .Where(row => row.Tag.ToString() == "Error"))
		    {
		        row.Visible = errorToolButton.Checked;
		    }
		}

	    private void fixMenuItem_Click(object sender, EventArgs e)
		{
			int row = errorGridView.SelectedRows[0].Index;
			int line = (int)errorGridView.Rows[row].Cells[4].Value;
			string file = errorGridView.Rows[row].Cells[3].Tag.ToString();
			string error = errorGridView.Rows[row].Cells[2].Value.ToString();
            new GotoLineAction(file, line - 1).Execute();
	        if (!error.Contains("Relative jump"))
	        {
	            return;
	        }

            // TODO: look at
	        //_documentService.ActiveDocument.FixError(line, DocumentService.FixableErrorType.RelativeJump);
	        errorGridView.Rows.RemoveAt(row);
		}

		private void gotoMenuItem_Click(object sender, EventArgs e)
		{
			int row = errorGridView.SelectedRows[0].Index;
			int line = (int)errorGridView.Rows[row].Cells[4].Value;
			string file = errorGridView.Rows[row].Cells[3].Tag.ToString();
            new GotoLineAction(file, line).Execute();
		}

		private void Instance_AssemblerFileFinished(object sender, AssemblyFinishFileEventArgs e)
		{
            // TODO: when assembler service gets finalized this should change
			if (!_projectService.Project.IsInternal)
			{
				return;
			}

			this.Invoke(() =>
			{
			    ParseOutput(e.Output.ParsedErrors);
			    if (!e.AssemblySucceeded)
			    {
			        Show();
			    }
			});
		}

		private void Instance_AssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
		{
		    if (!IsHandleCreated)
		    {
		        return;
		    }

			this.Invoke(() =>
			{
			    ParseOutput(e.Output.ParsedErrors);
                if (!e.AssemblySucceeded)
                {
                    Show();
                }
			});
		}

		private void warnToolButton_CheckedChanged(object sender, EventArgs e)
		{
			foreach (DataGridViewRow row in errorGridView.Rows.Cast<DataGridViewRow>().Where(row => row.Tag.ToString() == "Warning"))
			{
				row.Visible = warnToolButton.Checked;
			}
		}
	}
}