using System;
using System.Collections;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;
using Revsoft.TextEditor;
using System.Windows.Forms;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class LabelList : ToolWindow
    {
        public LabelList()
        {
            InitializeComponent();
        }

        private void LabelList_VisibleChanged(object sender, EventArgs e)
        {
			DockingService.MainForm.UpdateChecks();
        }

        private void labelsBox_DoubleClick(object sender, EventArgs e)
        {
            if (labelsBox.SelectedItem == null)
                return;
			DocumentService.GotoLabel((ILabel)labelsBox.SelectedItem);
			DockingService.ActiveDocument.Focus();
        }

        private void includeEquatesBox_CheckedChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.showEquates = includeEquatesBox.Checked;
			DocumentService.ActiveDocument.UpdateLabelBox();
        }

        private void alphaBox_CheckedChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.alphabetizeLabels = alphaBox.Checked;
			//this is the retardedest thing ever :|
			//
			labelsBox.Sorted = alphaBox.Checked;
			DocumentService.ActiveDocument.UpdateLabelBox();
        }

		public override void Copy()
		{
			Clipboard.SetDataObject(labelsBox.Items[labelsBox.SelectedIndex].ToString());
		}

		internal void AddLabels(ParserInformation list)
		{
			NativeMethods.TurnOffDrawing(labelsBox.Handle);
			bool ShowEquates = includeEquatesBox.Checked;
			labelsBox.Items.Clear();
			foreach (IParserData data in list.LabelsList)
			{
                Type dataType = data.GetType();
                if (dataType == typeof(Services.Parser.Label))
                {
                    ILabel label = data as ILabel;
                    if ((ShowEquates && label.IsEquate && !label.IsReusable) || (!label.IsReusable && !label.IsEquate))
                        labelsBox.Items.Add(label);
                }
                else if (dataType == typeof(Macro))
                {
                    labelsBox.Items.Add(data);
                }
                else if (dataType == typeof(Define))
                {
                    if (ShowEquates)
                        labelsBox.Items.Add(data);
                }
                else if (dataType == typeof(IncludeFile))
                {
                    labelsBox.Items.Add(data);
                }					
			}
			NativeMethods.TurnOnDrawing(labelsBox.Handle);
		}

		internal void ClearLabels()
		{
			labelsBox.Items.Clear();
		}

		private void labelsBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar != (char)Keys.Enter)
				return;
			DocumentService.GotoLabel((ILabel)labelsBox.SelectedItem);
			DockingService.ActiveDocument.Focus();
		}
	}
}