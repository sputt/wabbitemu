using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services;
using System.IO;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;
using System.Drawing.Text;

namespace Revsoft.Wabbitcode
{
    public partial class RefactorForm : Form
    {
		string word;
        public RefactorForm()
        {
            InitializeComponent();
			word = DockingService.ActiveDocument.GetWord();
			Text = "Refactor '" + word + "'";
			nameBox.Text = word;
		}

		private void previewButton_Click(object sender, EventArgs e)
		{
			this.tabControl.TabPages.Clear();
			this.Height = this.Height + 400;
			var refs = ParserService.FindAllReferences(word);
			foreach (var file in refs)
			{
				string fileName = file[0].File;
				var tab = new TabPage(Path.GetFileName(fileName));
				tabControl.TabPages.Add(tab);
				var editor = new TextEditorControl();
				editor.Dock = DockStyle.Fill;
				editor.LoadFile(fileName);
				editor.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(fileName);
				if (Settings.Default.antiAlias)
					editor.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
				else
					editor.TextRenderingHint = TextRenderingHint.SingleBitPerPixel;
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

		private void okButton_Click(object sender, EventArgs e)
		{
			DocumentService.InternalSave = true;
			var refs = ParserService.FindAllReferences(word);
			TextEditorControl editor = null;
			foreach (var file in refs)
			{
				bool alreadyOpen = false;
				string fileName = file[0].File;
				NewEditor openDoc = null;
				foreach (NewEditor doc in DockingService.Documents)
				{
					if (string.Equals(doc.FileName, fileName, StringComparison.OrdinalIgnoreCase))
					{
						alreadyOpen = true;
						editor = doc.EditorBox;
						openDoc = doc;
						break;
					}
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
						openDoc.SaveFile();
					else
						editor.SaveFile(fileName);
				}
				catch (Exception ex)
				{
					DockingService.ShowError("Error saving refactor", ex);
				}
			}
			//sleep for a little bit just to make sure the last save is done
			//this is already super high should be fine even for those using butterflies
			System.Threading.Thread.Sleep(200);
			DocumentService.InternalSave = false;
		}

		private void cancelButton_Click(object sender, EventArgs e)
		{
			this.Close();
		}
    }
}
