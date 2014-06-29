using System;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.DocumentWindows;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;
using Label = Revsoft.Wabbitcode.Services.Parser.Label;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class LabelList : ToolWindow
    {
        #region Private Properties

        private readonly IParserService _parserService;

        #endregion

        public LabelList()
        {
            InitializeComponent();

            _parserService = DependencyFactory.Resolve<IParserService>();

            _parserService.OnParserFinished += (sender, args) =>
            {
                var document = DockingService.ActiveDocument as AbstractFileEditor;
                if (document == null || string.IsNullOrEmpty(document.FileName) ||
                    args.FileName != document.FileName || !IsHandleCreated)
                {
                    return;
                }

                this.Invoke(UpdateLabelBox);
            };
            DockingService.ActiveDocumentChanged += DockingServiceActiveDocumentChanged;
        }

        private void DockingServiceActiveDocumentChanged(object sender, EventArgs e)
        {
            if (DockingService.ActiveDocument != null)
            {
                UpdateLabelBox();
            }
            else
            {
                ClearLabels();
            }
        }

        public override void Copy()
        {
            Clipboard.SetDataObject(labelsBox.SelectedItem.ToString());
        }

        private void AddLabels()
        {
            var fileEditor = DockingService.ActiveDocument as AbstractFileEditor;
            if (fileEditor == null)
            {
                return;
            }

            FilePath fileName = fileEditor.FileName;
            if (fileName == null)
            {
                return;
            }

            ParserInformation info = _parserService.GetParserInfo(fileName);
            if (info == null)
            {
                ClearLabels();
                return;
            }

            bool showEquates = includeEquatesBox.Checked;
            ListBox.ObjectCollection labelsToAdd = new ListBox.ObjectCollection(labelsBox);

            var labels = info.Where(d => (d is Label && !((Label) d).IsReusable) ||
                                         ((d is Equate || d is Define || d is Macro) && showEquates))
                .Cast<object>().ToArray();
            labelsToAdd.AddRange(labels);

            labelsBox.Items.Clear();
            labelsBox.Items.AddRange(labelsToAdd);
        }

        private void ClearLabels()
        {
            labelsBox.Items.Clear();
        }

        private void EnableLabelBox()
        {
            labelsBox.Enabled = true;
            alphaBox.Enabled = true;
            includeEquatesBox.Enabled = true;
        }

        private void alphaBox_CheckedChanged(object sender, EventArgs e)
        {
            Settings.Default.AlphabetizeLabels = alphaBox.Checked;
            labelsBox.Sorted = alphaBox.Checked;
            UpdateLabelBox();
        }

        private void UpdateLabelBox()
        {
            EnableLabelBox();
            AddLabels();
        }

        private void includeEquatesBox_CheckedChanged(object sender, EventArgs e)
        {
            Settings.Default.ShowEquates = includeEquatesBox.Checked;
            UpdateLabelBox();
        }

        private void labelsBox_DoubleClick(object sender, EventArgs e)
        {
            if (labelsBox.SelectedItem == null)
            {
                return;
            }

            GotoLabel((IParserData) labelsBox.SelectedItem);
        }

        private void labelsBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != (char) Keys.Enter)
            {
                return;
            }

            GotoLabel((ILabel) labelsBox.SelectedItem);
            var activeForm = DockingService.ActiveDocument as Form;
            if (activeForm != null)
            {
                activeForm.Focus();
            }
        }

        private static void GotoLabel(IParserData parserData)
        {
            AbstractUiAction.RunCommand(new GotoLabelAction(parserData));
        }
    }
}