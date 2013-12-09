using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;
using IFileReaderService = Revsoft.Wabbitcode.Services.Interfaces.IFileReaderService;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    public enum SearchMode
    {
        Find,
        Replace,
        FindInFiles,
    }

	public partial class FindAndReplaceForm : Form
	{
	    private static FindAndReplaceForm _instance;
	    public static FindAndReplaceForm Instance
	    {
	        get { return _instance ?? (_instance = new FindAndReplaceForm()); }
	    }

        private const int FindTabIndex = 0;
        private const int ReplaceTabIndex = 1;
        private const int FindFilesTabIndex = 2;

		private bool _lastSearchLoopedAround;
		private bool _lastSearchWasBackward;

	    private readonly FindResultsWindow _results;
		private TextEditorControl _editor;
		private TextEditorSearcher _search;
		private readonly IDockingService _dockingService;
		private readonly IProjectService _projectService;

	    private FindAndReplaceForm()
		{
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _dockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;

            _results = _dockingService.GetDockingWindow(FindResultsWindow.WindowIdentifier) as FindResultsWindow;

			InitializeComponent();
		}

        void DockingService_OnActiveDocumentChanged(object sender, EventArgs e)
        {
            ITextEditor textEditor = _dockingService.ActiveDocument as ITextEditor;
            if (textEditor == null || Visible == false)
            {
                return;
            }

            switch (findTabs.SelectedIndex)
            {
                case FindTabIndex:
                    textEditor.ShowFindForm(Owner, SearchMode.Find);
                    break;
                case ReplaceTabIndex:
                    textEditor.ShowFindForm(Owner, SearchMode.Replace);
                    break;
                case FindFilesTabIndex:
                    textEditor.ShowFindForm(Owner, SearchMode.FindInFiles);
                    break;
            }
        }

	    private TextRange FindNext(bool matchCase, bool matchWholeWord, bool searchBackward, string messageIfNotFound)
		{
			if (string.IsNullOrEmpty(findFindBox.Text))
			{
				MessageBox.Show("No string specified to look for!");
				return null;
			}

	        if (_editor == null)
	        {
	            MessageBox.Show("No open document");
                return null;
	        }

			_lastSearchWasBackward = searchBackward;
			_search.LookFor = findFindBox.Text;
			_search.MatchCase = matchCase;
	        _search.MatchWholeWordOnly = matchWholeWord;

			var caret = _editor.ActiveTextAreaControl.Caret;
			int startFrom = caret.Offset - (searchBackward ? 1 : 0);
			TextRange range = _search.FindNext(startFrom, searchBackward, out _lastSearchLoopedAround);
			if (range != null)
			{
				SelectResult(range);
			}
			else if (!string.IsNullOrEmpty(messageIfNotFound))
			{
				MessageBox.Show(messageIfNotFound);
			}

			return range;
		}

		public void ShowFor(Form owner, TextEditorControl editor, SearchMode mode)
		{
		    _editor = editor;
            IFileReaderService fileReaderService = ServiceFactory.Instance.GetServiceInstance<IFileReaderService>();
            string fileText = editor == null ? string.Empty : fileReaderService.GetFileText(editor.FileName);
            _search = new TextEditorSearcher(fileText);

		    Owner = owner;
			Show();

			findNextFindButton.Focus();
			findFindBox.SelectAll();
			findFindBox.Focus();

		    switch (mode)
		    {
                case SearchMode.Find:
                    findTabs.SelectTab(findPage);
                    findFindBox.Focus();
                    findFindBox.SelectAll();
		            break;
                case SearchMode.Replace:
                    findTabs.SelectTab(replacePage);
                    replaceFindBox.Focus();
                    replaceFindBox.SelectAll();
                    break;
                case SearchMode.FindInFiles:
                    findTabs.SelectTab(findInFilesPage);
                    findFilesBox.Focus();
                    findFilesBox.SelectAll();
                    break;
		    }
		}

		private void findNextFindButton_Click(object sender, EventArgs e)
		{
            bool matchCase = matchCaseFindCheckbox.Checked;
            bool matchWholeWord = matchWholeWordFindCheckbox.Checked;
            FindNext(matchCase, matchWholeWord, false, "Text not found");
		}

		private void findPrevFindButton_Click(object sender, EventArgs e)
		{
            bool matchCase = matchCaseFindCheckbox.Checked;
            bool matchWholeWord = matchWholeWordFindCheckbox.Checked;
			FindNext(matchCase, matchWholeWord, true, "Text not found");
		}

	    private void FindInFiles(string textToFind, bool matchCase, bool matchWholeWord)
	    {
            IProject project = _projectService.Project;
            if (!project.IsInternal)
            {
                IEnumerable<ProjectFile> files = project.GetProjectFiles();
                _results.NewFindResults(findFindBox.Text, project.ProjectName);
                foreach (ProjectFile file in files)
                {
                    if (!File.Exists(file.FileFullPath))
                    {
                        continue;
                    }

                    string fileText = GetTextForFile(project, file);
                    FindTextInFile(file.FileFullPath, fileText, textToFind, matchWholeWord, matchCase);
                }
            }

            _results.DoneSearching();
            _dockingService.ShowDockPanel(_results);
	    }

		private void FindTextInFile(string fileName, string fileText, string textToFind, bool matchWholeWord, bool matchCase)
		{
			string pattern = string.Format("^(?<line>.*?{1}{0}{1}.*?)$", Regex.Escape(textToFind), matchWholeWord ? "\\b" : String.Empty);
			RegexOptions options = matchCase ? RegexOptions.None : RegexOptions.IgnoreCase;
			MatchCollection matches = Regex.Matches(fileText, pattern, RegexOptions.Compiled | RegexOptions.Multiline | options);
			foreach (Match match in matches)
			{
				int lineNumber = fileText.Substring(0, match.Index).Count(c => c == '\n');
				_results.AddFindResult(fileName, lineNumber, match.Groups["line"].Value);
			}
		}

		private static string GetTextForFile(IProject project, ProjectFile file)
		{
			string fileText = string.Empty;
			try
			{
				using (StreamReader reader = new StreamReader(Path.Combine(project.ProjectDirectory, file.FileFullPath)))
				{
					fileText = reader.ReadToEnd();
				}
			}
			catch (IOException)
			{
				Debug.WriteLine("Failed to open file {0}, while searching in all files", file.FileFullPath);
			}
			return fileText;
		}

		private void InsertText(string text)
		{
			var textArea = _editor.ActiveTextAreaControl.TextArea;
			textArea.Document.UndoStack.StartUndoGroup();
			try
			{
				if (textArea.SelectionManager.HasSomethingSelected)
				{
					textArea.Caret.Position = textArea.SelectionManager.SelectionCollection[0].StartPosition;
					textArea.SelectionManager.RemoveSelectedText();
				}

				textArea.InsertString(text);
			}
			finally
			{
				textArea.Document.UndoStack.EndUndoGroup();
			}
		}

		private void SelectResult(TextRange range)
		{
			TextLocation p1 = _editor.Document.OffsetToPosition(range.Offset);
			TextLocation p2 = _editor.Document.OffsetToPosition(range.Offset + range.Length);
			_editor.ActiveTextAreaControl.SelectionManager.SetSelection(p1, p2);
			_editor.ActiveTextAreaControl.ScrollTo(p1.Line, p1.Column);

			// Also move the caret to the end of the selection, because when the user
			// presses F3, the caret is where we start searching next time.
			_editor.ActiveTextAreaControl.Caret.Position =
				_editor.Document.OffsetToPosition(range.Offset + range.Length);
		}

		private void txtLookFor_TextChanged(object sender, EventArgs e)
		{
		}

        private void findInFilesButton_Click(object sender, EventArgs e)
        {
            bool matchCase = matchCaseFilesCheckbox.Checked;
            bool matchWholeWord = matchWholeWordFilesCheckbox.Checked;
            string textToFind = findFilesBox.Text;
            FindInFiles(textToFind, matchCase, matchWholeWord);
            Close();
        }

        private void replaceButton_Click(object sender, EventArgs e)
        {
            bool matchCase = matchCaseReplaceCheckbox.Checked;
            bool matchWholeWord = matchWholeWordReplaceCheckbox.Checked;
            var sm = _editor.ActiveTextAreaControl.SelectionManager;
            if (string.Equals(sm.SelectedText, findFindBox.Text, StringComparison.OrdinalIgnoreCase))
            {
                InsertText(replaceReplaceBox.Text);
            }

            FindNext(matchCase, matchWholeWord, _lastSearchWasBackward, "Text not found.");
        }

        private void replaceAllButton_Click(object sender, EventArgs e)
        {
            bool matchCase = matchCaseReplaceCheckbox.Checked;
            bool matchWholeWord = matchWholeWordReplaceCheckbox.Checked;
            int count = 0;
            // if the replacement string contains the original search string
            // (e.g. replace "red" with "very red") we must avoid looping around and
            // replacing forever! To fix, start replacing at beginning of region (by
            // moving the caret) and stop as soon as we loop around.
            // _editor.ActiveTextAreaControl.Caret.Position =
            // _editor.Document.OffsetToPosition(_search.BeginOffset);

            _editor.Document.UndoStack.StartUndoGroup();
            try
            {
                while (FindNext(matchCase, matchWholeWord, false, null) != null)
                {
                    if (_lastSearchLoopedAround)
                    {
                        break;
                    }

                    // Replace
                    count++;
                    InsertText(replaceReplaceBox.Text);
                }
            }
            finally
            {
                _editor.Document.UndoStack.EndUndoGroup();
            }

            if (count == 0)
            {
                MessageBox.Show("No occurrances found.");
            }
            else
            {
                MessageBox.Show(string.Format("Replaced {0} occurrances.", count));
                Close();
            }
        }

        private void findTabs_SelectedIndexChanged(object sender, EventArgs e)
        {
            Text = findTabs.SelectedTab.Text;
            switch (findTabs.SelectedIndex)
            {
                case FindTabIndex:
                    AcceptButton = findNextFindButton;
                    break;
                case ReplaceTabIndex:
                    AcceptButton = findNextReplaceButton;
                    break;
                case FindFilesTabIndex:
                    AcceptButton = findInFilesButton;
                    break;
            }
        }

        private void FindAndReplaceForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Prevent dispose, as this form can be re-used
            if (e.CloseReason == CloseReason.FormOwnerClosing)
            {
                return;
            }

            if (Owner != null)
            {
                Owner.Select();    // prevent another app from being activated instead
            }

            e.Cancel = true;
            Hide();

            if (_editor != null)
            {
                _editor.Refresh(); // must repaint manually
            }
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            Close();
        }
	}

	public class TextRange : AbstractSegment
	{
		public TextRange(int offset, int length)
		{
			this.offset = offset;
			this.length = length;
		}
	}
}