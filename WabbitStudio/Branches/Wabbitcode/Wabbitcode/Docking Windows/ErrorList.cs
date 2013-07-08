using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;

using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;

using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class ErrorList : ToolWindow
    {
        private int errors;
        private int warnings;

        public ErrorList()
        {
            InitializeComponent();
            errorGridView.ContextMenu = contextMenu1;
            AssemblerService.Instance.AssemblerFileFinished += this.Instance_AssemblerFileFinished;
            AssemblerService.Instance.AssemblerProjectFinished += this.Instance_AssemblerProjectFinished;
        }

        public override void Copy()
        {
            Clipboard.SetDataObject(errorGridView.GetClipboardContent());
        }

        public void ParseOutput(List<Errors> parsedErrors, string startDir)
        {
            this.errors = 0;
            this.warnings = 0;
            errorGridView.Rows.Clear();
            foreach (Errors error in parsedErrors)
            {
                if (error.IsWarning)
                {
                    this.AddWarning(error.Description, error.File, error.LineNum.ToString(), warnToolButton.Checked);
                }
                else
                {
                    this.AddError(error.Description, error.File, error.LineNum.ToString(), errorToolButton.Checked);
                }
            }

            errorToolButton.Text = this.errors + " Errors";
            warnToolButton.Text = this.warnings + " Warnings";
            foreach (NewEditor child in DockingService.Documents)
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
            this.errors++;
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
            this.warnings++;
        }

        private void Copy(object sender, EventArgs e)
        {
            this.Copy();
        }

        private void errorGridView_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.RowIndex < 0)
            {
                return;
            }

            int line = (int)errorGridView.Rows[e.RowIndex].Cells[4].Value;
            string file = errorGridView.Rows[e.RowIndex].Cells[3].Tag.ToString();
            DocumentService.GotoLine(file, line);
        }

        private void errorGridView_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right)
            {
                return;
            }

            contextMenu1.Show(errorGridView, new System.Drawing.Point(e.X, e.Y));
        }

        private void ErrorList_VisibleChanged(object sender, EventArgs e)
        {
            if (Disposing == false)
            {
                DockingService.MainForm.UpdateChecks();
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
            DocumentService.GotoLine(file, line);
            if (error.Contains("Relative jump"))
            {
                DocumentService.ActiveDocument.FixError(line, DocumentService.FixableErrorType.RelativeJump);
                errorGridView.Rows.RemoveAt(row);
            }
        }

        private void gotoMenuItem_Click(object sender, EventArgs e)
        {
            int row = errorGridView.SelectedRows[0].Index;
            int line = (int)errorGridView.Rows[row].Cells[4].Value;
            string file = errorGridView.Rows[row].Cells[3].Tag.ToString();
            DocumentService.GotoLine(file, line);
        }

        private void Instance_AssemblerFileFinished(object sender, AssemblyFinishFileEventArgs e)
        {
            if (!ProjectService.IsInternal)
            {
                return;
            }

            DockingService.MainForm.Invoke(() =>
                                           this.ParseOutput(e.Output.ParsedErrors, Path.GetDirectoryName(e.InputFile)));
        }

        private void Instance_AssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
        {
            DockingService.MainForm.Invoke(() =>
                                           this.ParseOutput(e.Output.ParsedErrors, e.Project.ProjectDirectory));
        }

        private void warnToolButton_CheckedChanged(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in errorGridView.Rows)
            {
                if (row.Tag.ToString() == "Warning")
                {
                    row.Visible = warnToolButton.Checked;
                }
            }
        }
    }
}