using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using System;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode
{
	public sealed partial class RefactorForm : Form
	{
		private const int PreviewHeight = 400;
		private readonly string _word;
	    private readonly IProjectService _projectService;
		private TextEditorControl[] _editors;
		private bool _isPreviewed;

		public RefactorForm(ITextEditor editor, IProjectService projectService)
		{
			_projectService = projectService;
		    InitializeComponent();

			_word = editor.GetWordAtCaret();
			Text = string.Format("Refactor '{0}'", _word);
			nameBox.Text = _word;
		}

		private void cancelButton_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void okButton_Click(object sender, EventArgs e)
		{
			var refs = _projectService.FindAllReferences(_word);
		    foreach (var file in refs)
			{
			    string fileName = file[0].File;

				TextEditorControl editor = new TextEditorControl();
				editor.LoadFile(fileName);

				foreach (var reference in file)
				{
					int offset = editor.Document.GetOffsetForLineNumber(reference.Line) + reference.Col;
					int len = reference.ReferenceString.Length;
					editor.Document.Replace(offset, len, nameBox.Text);
				}

				try
				{
                    editor.SaveFile(fileName);
				}
				catch (Exception ex)
				{
					DockingService.ShowError("Error saving refactor", ex);
				}
			}
		}

		private void previewButton_Click(object sender, EventArgs e)
		{
			if (_isPreviewed)
			{
				HidePreview();
			}
			else
			{
				SetupPreview();
			}
			_isPreviewed = !_isPreviewed;
		}

		private void HidePreview()
		{
			tabControl.Visible = false;
			tabControl.TabPages.Clear();
			Height -= PreviewHeight;
			prevRefButton.Visible = false;
			nextRefButton.Visible = false;
		}

		private void SetupPreview()
		{
			tabControl.Visible = true;
			tabControl.TabPages.Clear();
			Height += PreviewHeight;
			prevRefButton.Visible = true;
			nextRefButton.Visible = true;

			var refs = _projectService.FindAllReferences(_word).ToArray();
			_editors = new TextEditorControl[refs.Count()];
			int i = 0;
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
				editor.TextRenderingHint = Settings.Default.AntiAlias ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;

				editor.TextEditorProperties.MouseWheelScrollDown = !Settings.Default.InverseScrolling;
				editor.ShowLineNumbers = Settings.Default.LineNumbers;
				editor.Font = Settings.Default.EditorFont;
				editor.LineViewerStyle = Settings.Default.LineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None;
				tab.Controls.Add(editor);
				_editors[i++] = editor;
				foreach (var reference in file)
				{
					int offset = editor.Document.GetOffsetForLineNumber(reference.Line) + reference.Col;
					int len = reference.ReferenceString.Length;
					editor.Document.Replace(offset, len, nameBox.Text);
					editor.Document.MarkerStrategy.AddMarker(new TextMarker(offset, nameBox.Text.Length, TextMarkerType.SolidBlock, Color.LightGreen));
					editor.Document.BookmarkManager.AddMark(new Bookmark(editor.Document, new TextLocation(0, reference.Line)));
				}

				editor.Document.ReadOnly = true;
			}
		}

		private void prevRefButton_Click(object sender, EventArgs e)
		{
			TextEditorControl editor = _editors[tabControl.SelectedIndex];
			TextArea textArea = editor.ActiveTextAreaControl.TextArea;
			Bookmark mark = textArea.Document.BookmarkManager.GetPrevMark(textArea.Caret.Line);
			if (mark == null)
			{
				return;
			}

			if (textArea.Caret.Position <= mark.Location)
			{
				tabControl.SelectedIndex--;
				if (tabControl.SelectedIndex < 0)
				{
					tabControl.SelectedIndex = tabControl.TabCount - 1;
				}
				editor = _editors[tabControl.SelectedIndex];
				textArea = editor.ActiveTextAreaControl.TextArea;
				mark = textArea.Document.BookmarkManager.GetLastMark(b => true);
			}

			textArea.Caret.Position = mark.Location;
			textArea.SelectionManager.ClearSelection();
			textArea.SetDesiredColumn();
		}

		private void nextRefButton_Click(object sender, EventArgs e)
		{
			TextEditorControl editor = _editors[tabControl.SelectedIndex];
			TextArea textArea = editor.ActiveTextAreaControl.TextArea;
			Bookmark mark = textArea.Document.BookmarkManager.GetNextMark(textArea.Caret.Line);
			if (mark == null)
			{
				return;
			}

			if (textArea.Caret.Position >= mark.Location)
			{
				if (tabControl.SelectedIndex + 1 == tabControl.TabCount)
				{
					tabControl.SelectedIndex = 0;
				}
				else
				{
					tabControl.SelectedIndex++;
				}

				editor = _editors[tabControl.SelectedIndex];
				textArea = editor.ActiveTextAreaControl.TextArea;
				mark = textArea.Document.BookmarkManager.GetFirstMark(b => true);
			}

			textArea.Caret.Position = mark.Location;
			textArea.SelectionManager.ClearSelection();
			textArea.SetDesiredColumn();
		}
	}
}