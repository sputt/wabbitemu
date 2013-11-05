using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using System;
using System.Windows.Forms;
using Label = Revsoft.Wabbitcode.Services.Parser.Label;


namespace Revsoft.Wabbitcode.DockingWindows
{
	public partial class LabelList : ToolWindow
	{
		#region Private Properties

		private readonly IDockingService _dockingService;
		private readonly IDocumentService _documentService;
		private readonly IParserService _parserService;

		#endregion

		public LabelList(IDockingService dockingService, IDocumentService documentService, IParserService parserService)
			: base(dockingService)
		{
			InitializeComponent();

			_dockingService = dockingService;
			_documentService = documentService;
			_parserService = parserService;

		    _parserService.OnParserFinished += (sender, args) =>
		    {
		        if (_dockingService.ActiveDocument == null || 
                    string.IsNullOrEmpty(_dockingService.ActiveDocument.FileName) ||
                    args.FileName != _dockingService.ActiveDocument.FileName)
		        {
		            return;
		        }

		        _dockingService.Invoke(UpdateLabelBox);
		    };
            DockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;
		}

        void DockingService_OnActiveDocumentChanged(object sender, EventArgs e)
        {
            if (_dockingService.ActiveDocument != null)
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
            if (_dockingService.ActiveDocument == null)
            {
                return;
            }

            string fileName = _dockingService.ActiveDocument.FileName;
			if (string.IsNullOrEmpty(fileName))
			{
				return;
			}

			ParserInformation info = _parserService.GetParserInfo(fileName);
			if (info == null)
			{
				return;
			}

			bool showEquates = includeEquatesBox.Checked;
			ListBox.ObjectCollection labelsToAdd = new ListBox.ObjectCollection(labelsBox);

			foreach (IParserData data in info)
			{
				var label = data as Label;
				if (data is Label && !label.IsReusable)
				{
					labelsToAdd.Add(label);
				}
				else if ((data is Equate || data is Define || data is Macro) && (showEquates))
				{
					labelsToAdd.Add(data);
				}
			}

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
			Settings.Default.alphabetizeLabels = alphaBox.Checked;
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
			Settings.Default.showEquates = includeEquatesBox.Checked;
			UpdateLabelBox();
		}

		private void labelsBox_DoubleClick(object sender, EventArgs e)
		{
			if (labelsBox.SelectedItem == null)
			{
				return;
			}

			_documentService.GotoLabel((IParserData)labelsBox.SelectedItem);
			_dockingService.ActiveDocument.Focus();
		}

		private void labelsBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar != (char)Keys.Enter)
			{
				return;
			}

			_documentService.GotoLabel((ILabel)labelsBox.SelectedItem);
			_dockingService.ActiveDocument.Focus();
		}
	}
}