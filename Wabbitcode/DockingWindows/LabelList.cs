using System.Linq;
using Revsoft.Wabbitcode.Extensions;
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
	    public const string WindowIdentifier = "Label List";
        public override string WindowName
        {
            get
            {
                return WindowIdentifier;
            }
        }

		#region Private Properties

		private readonly IDocumentService _documentService;
		private readonly IParserService _parserService;

		#endregion

		public LabelList()
		{
			InitializeComponent();

            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            _parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();

		    _parserService.OnParserFinished += (sender, args) =>
		    {
		        var document = DockingService.ActiveDocument as AbstractFileEditor;
		        if (document == null || 
                    string.IsNullOrEmpty(document.FileName) ||
                    !FileOperations.CompareFilePath(args.FileName, document.FileName) ||
                    !IsHandleCreated)
		        {
		            return;
		        }

		        this.Invoke(UpdateLabelBox);
		    };
            DockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;
		}

        void DockingService_OnActiveDocumentChanged(object sender, EventArgs e)
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

		    string fileName = fileEditor.FileName;
			if (string.IsNullOrEmpty(fileName))
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

			_documentService.GotoLabel((IParserData)labelsBox.SelectedItem);
		}

		private void labelsBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar != (char)Keys.Enter)
			{
				return;
			}

			_documentService.GotoLabel((ILabel)labelsBox.SelectedItem);
		    var activeForm = DockingService.ActiveDocument as Form;
		    if (activeForm != null)
		    {
		        activeForm.Focus();
		    }
		}
	}
}