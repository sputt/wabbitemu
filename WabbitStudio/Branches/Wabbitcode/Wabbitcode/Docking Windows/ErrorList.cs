using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;


namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class ErrorList : ToolWindow
	{
		private int _errors;
		private int _warnings;
		private readonly IDockingService _dockingService;
		private readonly IDocumentService _documentService;
		private readonly IProjectService _projectService;

		public ErrorList(IAssemblerService assemblerService, IDockingService dockingService,
			IDocumentService documentService, IProjectService projectService)
			: base(dockingService)
		{
			InitializeComponent();
			_dockingService = dockingService;
			_documentService = documentService;
			_projectService = projectService;

			errorGridView.ContextMenu = contextMenu1;
			assemblerService.AssemblerFileFinished += Instance_AssemblerFileFinished;
			assemblerService.AssemblerProjectFinished += Instance_AssemblerProjectFinished;
		}

		public override void Copy()
		{
			var data = errorGridView.GetClipboardContent();
			if (data != null)
			{
				Clipboard.SetDataObject(data);
			}
		}

		public void ParseOutput(List<Errors> parsedErrors)
		{
			_errors = 0;
			_warnings = 0;
			errorGridView.Rows.Clear();
			foreach (Errors error in parsedErrors)
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
			foreach (NewEditor child in _dockingService.Documents)
			{
				child.UpdateIcons(parsedErrors);
			}
		}

		private void AddError(string description, string file, string line, bool visible)
		{
			int row = errorGridView.Rows.Add();

			// ComponentResourceManager resources = new ComponentResourceManager(typeof(ErrorList));
			errorGridView.Rows[row].Cells[0].Value = imageListIcons.Images[0]; // resources.GetObject("CategoryColumn.Image");
			errorGridView.Rows[row].Cells[1].Value = row + 1;
			errorGridView.Rows[row].Cells[2].Value = description;
			errorGridView.Rows[row].Cells[3].Value = Path.GetFileName(file);
			errorGridView.Rows[row].Cells[3].Tag = file;
			errorGridView.Rows[row].Cells[4].Value = Convert.ToInt32(line);
			errorGridView.Rows[row].Tag = "Error";
			errorGridView.Rows[row].Visible = visible;
			_errors++;
		}

		private void AddWarning(string description, string file, string line, bool visible)
		{
			int row = errorGridView.Rows.Add();
			errorGridView.Rows[row].Cells[0].Value = imageListIcons.Images[1];
			errorGridView.Rows[row].Cells[1].Value = row + 1;
			errorGridView.Rows[row].Cells[2].Value = description;
			errorGridView.Rows[row].Cells[3].Value = Path.GetFileName(file);
			errorGridView.Rows[row].Cells[3].Tag = file;
			errorGridView.Rows[row].Cells[4].Value = Convert.ToInt32(line);
			errorGridView.Rows[row].Tag = "Warning";
			errorGridView.Rows[row].Visible = visible;
			_warnings++;
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
			_documentService.GotoLine(file, line);
		}

		private void errorGridView_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Right)
			{
				return;
			}

			contextMenu1.Show(errorGridView, new Point(e.X, e.Y));
		}

		private void ErrorList_VisibleChanged(object sender, EventArgs e)
		{
			if (Disposing == false)
			{
				_dockingService.MainForm.UpdateChecks();
			}
		}

		private void errorToolButton_CheckedChanged(object sender, EventArgs e)
		{
			foreach (DataGridViewRow row in errorGridView.Rows)
			{
				if (row.Tag.ToString() == "Error")
				{
					row.Visible = errorToolButton.Checked;
				}
			}
		}

		private void fixMenuItem_Click(object sender, EventArgs e)
		{
			int row = errorGridView.SelectedRows[0].Index;
			int line = (int)errorGridView.Rows[row].Cells[4].Value;
			string file = errorGridView.Rows[row].Cells[3].Tag.ToString();
			string error = errorGridView.Rows[row].Cells[2].Value.ToString();
			_documentService.GotoLine(file, line);
			if (error.Contains("Relative jump"))
			{
				_documentService.ActiveDocument.FixError(line, DocumentService.FixableErrorType.RelativeJump);
				errorGridView.Rows.RemoveAt(row);
			}
		}

		private void gotoMenuItem_Click(object sender, EventArgs e)
		{
			int row = errorGridView.SelectedRows[0].Index;
			int line = (int)errorGridView.Rows[row].Cells[4].Value;
			string file = errorGridView.Rows[row].Cells[3].Tag.ToString();
			_documentService.GotoLine(file, line);
		}

		private void Instance_AssemblerFileFinished(object sender, AssemblyFinishFileEventArgs e)
		{
			if (!_projectService.Project.IsInternal)
			{
				return;
			}

			_dockingService.Invoke(() => ParseOutput(e.Output.ParsedErrors));
		}

		private void Instance_AssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
		{
			_dockingService.Invoke(() => ParseOutput(e.Output.ParsedErrors));
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