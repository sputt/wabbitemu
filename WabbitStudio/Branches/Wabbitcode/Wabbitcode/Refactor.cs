using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interface;
using System;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
	public partial class RefactorForm : Form
	{
		private readonly string _word;
		private readonly IDockingService _dockingService;
		private readonly IProjectService _projectService;

		public RefactorForm(IDockingService dockingService, IProjectService projectService)
		{
			_projectService = projectService;
			_dockingService = dockingService;
			InitializeComponent();
			_word = dockingService.ActiveDocument.GetWord();
			Text = string.Format("Refactor '{0}'", _word);
			nameBox.Text = _word;
		}

		public override sealed string Text
		{
			get { return base.Text; }
			set { base.Text = value; }
		}

		private void cancelButton_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void okButton_Click(object sender, EventArgs e)
		{
			var refs = _projectService.FindAllReferences(_word);
			TextEditorControl editor = null;
			foreach (var file in refs)
			{
				bool alreadyOpen = false;
				string fileName = file[0].File;
				NewEditor openDoc = null;
				foreach (NewEditor doc in _dockingService.Documents.Where(doc => string.Equals(doc.FileName, fileName, StringComparison.OrdinalIgnoreCase)))
				{
					alreadyOpen = true;
					editor = doc.EditorBox;
					openDoc = doc;
					break;
				}

				if (!alreadyOpen)
				{
					editor = new TextEditorControl();
					editor.LoadFile(fileName);
				}

				foreach (var reference in file)
				{
					int offset = editor.Document.GetOffsetForLineNumber(reference.Line) + reference.Col;
					int len = reference.ReferenceString.Length;
					editor.Document.Replace(offset, len, nameBox.Text);
				}

				try
				{
					if (alreadyOpen)
					{
						openDoc.SaveFile();
					}
					else
					{
						editor.SaveFile(fileName);
					}
				}
				catch (Exception ex)
				{
					DockingService.ShowError("Error saving refactor", ex);
				}
			}
		}

		private void previewButton_Click(object sender, EventArgs e)
		{
			tabControl.TabPages.Clear();
			Height = Height + 400;
			var refs = _projectService.FindAllReferences(_word);
			foreach (var file in refs)
			{
				string fileName = file[0].File;
				var tab = new TabPage(Path.GetFileName(fileName));
				tabControl.TabPages.Add(tab);
				var editor = new TextEditorControl
				{
					Dock = DockStyle.Fill
				};
				editor.LoadFile(fileName);
				editor.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(fileName);
				editor.TextRenderingHint = Settings.Default.antiAlias ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;

				editor.TextEditorProperties.MouseWheelScrollDown = !Settings.Default.inverseScrolling;
				editor.ShowLineNumbers = Settings.Default.lineNumbers;
				editor.Font = Settings.Default.editorFont;
				editor.LineViewerStyle = Settings.Default.lineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None;
				tab.Controls.Add(editor);
				foreach (var reference in file)
				{
					int offset = editor.Document.GetOffsetForLineNumber(reference.Line) + reference.Col;
					int len = reference.ReferenceString.Length;
					editor.Document.Replace(offset, len, nameBox.Text);
					editor.Document.MarkerStrategy.AddMarker(new TextMarker(offset, nameBox.Text.Length, TextMarkerType.SolidBlock, Color.LightGreen));
				}

				editor.Document.ReadOnly = true;
			}
		}
	}
}