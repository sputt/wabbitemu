using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.TextEditor;
using Revsoft.Wabbitcode.TextEditor.Interfaces;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    public sealed partial class RefactorExtractMethodForm : Form
    {
        private const int PreviewHeight = 400;
        private readonly string _extractedMethod;
        private string _methodName;
        private readonly IProjectService _projectService;
        private readonly Dictionary<string, TextEditorControl> _editors = new Dictionary<string, TextEditorControl>();
        private readonly List<List<Reference>> _references;

        public RefactorExtractMethodForm(ITextEditor editor, IProjectService projectService)
        {
            _projectService = projectService;
            InitializeComponent();

            _extractedMethod = editor.GetSelection();
            Text = string.Format("Extract Method '{0}'", _extractedMethod);
            nameBox.Text = _methodName;
            _references = _projectService.FindAllReferences(_extractedMethod).ToList();
            SetupPreview();
            UpdateEditorReferences();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            var refs = _projectService.FindAllReferences(_extractedMethod);
            foreach (var file in refs)
            {
                string fileName = file.First().File;

                TextEditorControl editor = new WabbitcodeTextEditor();
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

        private void SetupPreview()
        {
            tabControl.Visible = true;
            tabControl.TabPages.Clear();
            Height += PreviewHeight;
            prevRefButton.Visible = true;
            nextRefButton.Visible = true;

            foreach (var file in _references)
            {
                string fileName = file.First().File;
                var tab = new TabPage(Path.GetFileName(fileName)) {Tag = fileName};
                tabControl.TabPages.Add(tab);
                var editor = new WabbitcodeTextEditor
                {
                    Dock = DockStyle.Fill
                };
                editor.LoadFile(fileName);

                tab.Controls.Add(editor);
                _editors.Add(fileName, editor);

                editor.Document.ReadOnly = true;
            }
        }

        private void UpdateEditorReferences()
        {
            foreach (var file in _references)
            {
                TextEditorControl editor = _editors[file.First().File];
                foreach (var reference in file)
                {
                    int offset = editor.Document.GetOffsetForLineNumber(reference.Line) + reference.Col;
                    int len = reference.ReferenceString.Length;
                    editor.Document.Replace(offset, len, nameBox.Text);
                    editor.Document.MarkerStrategy.AddMarker(new TextMarker(offset, nameBox.Text.Length, TextMarkerType.SolidBlock, Color.LightGreen));
                    editor.Document.BookmarkManager.AddMark(new Bookmark(editor.Document, new TextLocation(0, reference.Line)));
                }
            }
        }

        private void prevRefButton_Click(object sender, EventArgs e)
        {
            TextEditorControl editor = _editors[tabControl.Tag.ToString()];
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
                editor = _editors[tabControl.Tag.ToString()];
                textArea = editor.ActiveTextAreaControl.TextArea;
                mark = textArea.Document.BookmarkManager.GetLastMark(b => true);
            }

            textArea.Caret.Position = mark.Location;
            textArea.SelectionManager.ClearSelection();
            textArea.SetDesiredColumn();
        }

        private void nextRefButton_Click(object sender, EventArgs e)
        {
            TextEditorControl editor = _editors[tabControl.Tag.ToString()];
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

                editor = _editors[tabControl.Tag.ToString()];
                textArea = editor.ActiveTextAreaControl.TextArea;
                mark = textArea.Document.BookmarkManager.GetFirstMark(b => true);
            }

            textArea.Caret.Position = mark.Location;
            textArea.SelectionManager.ClearSelection();
            textArea.SetDesiredColumn();
        }

        private void nameBox_TextChanged(object sender, EventArgs e)
        {
            _methodName = nameBox.Text;
        }
    }
}