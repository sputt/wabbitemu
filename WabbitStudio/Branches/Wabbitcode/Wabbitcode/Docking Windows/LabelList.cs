using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using System;
using System.Windows.Forms;
using Label = Revsoft.Wabbitcode.Services.Parser.Label;


namespace Revsoft.Wabbitcode.Docking_Windows
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
		}

		public override void Copy()
		{
			Clipboard.SetDataObject(labelsBox.Items[labelsBox.SelectedIndex].ToString());
		}

		private void AddLabels()
		{
			string fileName = _dockingService.ActiveDocument.FileName;
			if (string.IsNullOrEmpty(fileName))
			{
				return;
			}

			ParserInformation info = _parserService.GetParserInfo(fileName);
			NativeMethods.TurnOffDrawing(labelsBox.Handle);
			bool showEquates = includeEquatesBox.Checked;
			labelsBox.Items.Clear();

			foreach (IParserData data in info.GeneratedList)
			{
				var label = data as Label;
				if (data is Label && !label.IsReusable)
				{
					labelsBox.Items.Add(label);
				}
				else if ((data is Equate || data is Define || data is Macro) && (showEquates))
				{
					labelsBox.Items.Add(data);
				}
			}

			NativeMethods.TurnOnDrawing(labelsBox.Handle);
		}

		private void ClearLabels()
		{
			labelsBox.Items.Clear();
		}

		internal void DisableLabelBox()
		{
			ClearLabels();
			labelsBox.Enabled = false;
			alphaBox.Enabled = false;
			includeEquatesBox.Enabled = false;
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

		public void UpdateLabelBox()
		{
			EnableLabelBox();
			AddLabels();
		}

		private void includeEquatesBox_CheckedChanged(object sender, EventArgs e)
		{
			Settings.Default.showEquates = includeEquatesBox.Checked;
			UpdateLabelBox();
		}

		private void LabelList_VisibleChanged(object sender, EventArgs e)
		{
			_dockingService.MainForm.UpdateChecks();
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