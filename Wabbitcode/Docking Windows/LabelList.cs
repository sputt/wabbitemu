using System;
using System.Collections;
using System.Collections.Generic;
using System.Windows.Forms;

using Revsoft.TextEditor;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Parser;

using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class LabelList : ToolWindow
    {
        public LabelList()
        {
            InitializeComponent();
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
                if (data is Services.Parser.Label)
                {
                    if (!((ILabel)data).IsReusable)
                    {
                        labelsBox.Items.Add(data);
                    }
                }
                else if (data is Equate || data is Define)
                {
                    if (ShowEquates)
                    {
                        labelsBox.Items.Add(data);
                    }
                }
                else if (data is Macro || data is IncludeFile)
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

        internal void DisableLabelBox()
        {
            this.ClearLabels();
            labelsBox.Enabled = false;
            alphaBox.Enabled = false;
            includeEquatesBox.Enabled = false;
        }

        internal void EnableLabelBox()
        {
            labelsBox.Enabled = true;
            alphaBox.Enabled = true;
            includeEquatesBox.Enabled = true;
        }

        private void alphaBox_CheckedChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.alphabetizeLabels = alphaBox.Checked;
            labelsBox.Sorted = alphaBox.Checked;
            DocumentService.ActiveDocument.UpdateLabelBox();
        }

        private void includeEquatesBox_CheckedChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.showEquates = includeEquatesBox.Checked;
            DocumentService.ActiveDocument.UpdateLabelBox();
        }

        private void LabelList_VisibleChanged(object sender, EventArgs e)
        {
            DockingService.MainForm.UpdateChecks();
        }

        private void labelsBox_DoubleClick(object sender, EventArgs e)
        {
            if (labelsBox.SelectedItem == null)
            {
                return;
            }

            DocumentService.GotoLabel((ILabel)labelsBox.SelectedItem);
            DockingService.ActiveDocument.Focus();
        }

        private void labelsBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != (char)Keys.Enter)
            {
                return;
            }

            DocumentService.GotoLabel((ILabel)labelsBox.SelectedItem);
            DockingService.ActiveDocument.Focus();
        }
    }
}