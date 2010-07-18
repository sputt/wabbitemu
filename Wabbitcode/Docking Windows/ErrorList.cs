using System;
using System.IO;
using System.Windows.Forms;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class ErrorList : ToolWindow
	{
		public ErrorList()
		{
			InitializeComponent();
			errorGridView.ContextMenu = contextMenu1;
		}
		private int errors;
		private int warnings;

		private void addError(string description, string file, string line, bool visible)
		{
			int row = errorGridView.Rows.Add();
			//ComponentResourceManager resources = new ComponentResourceManager(typeof(ErrorList));
			errorGridView.Rows[row].Cells[0].Value = imageListIcons.Images[0]; //resources.GetObject("CategoryColumn.Image");
			errorGridView.Rows[row].Cells[1].Value = row + 1;
			errorGridView.Rows[row].Cells[2].Value = description;
			errorGridView.Rows[row].Cells[3].Value = Path.GetFileName(file);
			errorGridView.Rows[row].Cells[3].Tag = file;
			errorGridView.Rows[row].Cells[4].Value = Convert.ToInt32(line);
			errorGridView.Rows[row].Tag = "Error";
			errorGridView.Rows[row].Visible = visible;
			errors++;
		}

		private void addWarning(string description, string file, string line, bool visible)
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
			warnings++;
		}

		public void ParseOutput(string outputText, string startDir)
		{
			errors = 0;
			warnings = 0;
			errorGridView.Rows.Clear();
			AssemblerService.ErrorsInFiles.Clear();
			string[] lines = outputText.Split('\n');
			foreach (string line in lines)
			{
				int thirdColon;
				int firstColon;
				string file;
				string lineNum;
				string description;
				int secondColon;
				if (line.Contains("error"))
				{
					firstColon = line.IndexOf(':', 3);
					secondColon = line.IndexOf(':', firstColon + 1);
					thirdColon = line.IndexOf(':', secondColon + 1);
                    if (firstColon < 0 || secondColon < 0 || thirdColon < 0)
                    {
                        AssemblerService.ErrorsInFiles[AssemblerService.ErrorsInFiles.Count - 1].description += line;
                    }
                    else
                    {
                        file = Path.Combine(startDir, line.Substring(0, firstColon));
                        lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
                        description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                        AssemblerService.ErrorsInFiles.Add(new Errors(file, Convert.ToInt32(lineNum), description, false));
                        addError(description, file, lineNum, errorToolButton.Checked);
                    }
				}
				if (!line.Contains("warning"))
					continue;
				firstColon = line.IndexOf(':', 3);
				secondColon = line.IndexOf(':', firstColon + 1);
				thirdColon = line.IndexOf(':', secondColon + 1);
                if (firstColon < 0 || secondColon < 0 || thirdColon < 0)
                {
                    AssemblerService.ErrorsInFiles[AssemblerService.ErrorsInFiles.Count - 1].description += line;
                }
                else
                {
                    file = Path.Combine(startDir, line.Substring(0, firstColon));
                    lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
                    description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                    AssemblerService.ErrorsInFiles.Add(new Errors(file, Convert.ToInt32(lineNum), description, true));
                    addWarning(description, file, lineNum, warnToolButton.Checked);
                }
			}
			errorToolButton.Text = errors + " Errors";
			warnToolButton.Text = warnings + " Warnings";
			foreach (newEditor child in DockingService.Documents)
				child.UpdateIcons();
		}

		private void errorGridView_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex < 0)
				return;
			int line = (int)errorGridView.Rows[e.RowIndex].Cells[4].Value;
			string file = errorGridView.Rows[e.RowIndex].Cells[3].Tag.ToString();
			DocumentService.GotoLine(file, line);
		}

		private void errorToolButton_CheckedChanged(object sender, EventArgs e)
		{
			foreach (DataGridViewRow row in errorGridView.Rows)
				if (row.Tag.ToString() == "Error")
					row.Visible = errorToolButton.Checked;
		}

		private void warnToolButton_CheckedChanged(object sender, EventArgs e)
		{
			foreach (DataGridViewRow row in errorGridView.Rows)
				if (row.Tag.ToString() == "Warning")
					row.Visible = warnToolButton.Checked;
		}

		private void ErrorList_VisibleChanged(object sender, EventArgs e)
		{
			if (Disposing == false)
				DockingService.MainForm.UpdateChecks();
		}

		private void Copy(object sender, EventArgs e)
		{
			Copy();
		}

		public override void Copy()
		{
			Clipboard.SetDataObject(errorGridView.GetClipboardContent());
		}

		private void gotoMenuItem_Click(object sender, EventArgs e)
		{
			int row = errorGridView.SelectedRows[0].Index;
			int line = (int)errorGridView.Rows[row].Cells[4].Value;
			string file = errorGridView.Rows[row].Cells[3].Tag.ToString();
			DocumentService.GotoLine(file, line);
		}

		private void fixMenuItem_Click(object sender, EventArgs e)
		{
			int row = errorGridView.SelectedRows[0].Index;
			int line = (int)errorGridView.Rows[row].Cells[4].Value;
			string file = errorGridView.Rows[row].Cells[3].Tag.ToString();
			string error = errorGridView.Rows[row].Cells[2].Value.ToString();
			int offset = DockingService.ActiveDocument.editorBox.Document.GetOffsetForLineNumber(line - 1);
			int endline = offset;
			while (endline < DockingService.ActiveDocument.editorBox.Text.Length && DockingService.ActiveDocument.editorBox.Text[endline] != '\n')
				endline++;
			DocumentService.GotoLine(file, line);
			if (error.Contains("Relative jump"))
			{
				string lineContent = DockingService.ActiveDocument.editorBox.Document.GetText(offset, endline - offset);
				lineContent = lineContent.Replace("jr", "jp");
				int scroll = DockingService.ActiveDocument.editorBox.ActiveTextAreaControl.VScrollBar.Value;
				DockingService.ActiveDocument.editorBox.Text = DockingService.ActiveDocument.editorBox.Text.Remove(offset, endline - offset);
				DockingService.ActiveDocument.editorBox.Text = DockingService.ActiveDocument.editorBox.Text.Insert(offset, lineContent);
				DockingService.ActiveDocument.editorBox.ActiveTextAreaControl.VScrollBar.Value = scroll;
				errorGridView.Rows.RemoveAt(row);
			}
		}

		private void errorGridView_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button != MouseButtons.Right)
				return;
			contextMenu1.Show(errorGridView, new System.Drawing.Point(e.X, e.Y));
		}
	}
}