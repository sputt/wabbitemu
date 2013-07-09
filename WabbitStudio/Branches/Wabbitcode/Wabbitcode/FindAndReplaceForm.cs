using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
    public partial class FindAndReplaceForm : Form
    {
        public bool _lastSearchLoopedAround;
        public bool _lastSearchWasBackward = false;

        private int col;
        private int row;
        private TextEditorControl _editor;
        private Dictionary<TextEditorControl, HighlightGroup> _highlightGroups = new Dictionary<TextEditorControl, HighlightGroup>();
        private TextEditorSearcher _search;

        public FindAndReplaceForm()
        {
            InitializeComponent();
            this._search = new TextEditorSearcher();
        }

        public bool BigSearch
        {
            get
            {
                return btnHighlightAll.Text == "Find all" || btnHighlightAll.Text == "Replace All";
            }

            set
            {
                if (value)
                {
                    btnHighlightAll.Text = this.ReplaceMode ? "Replace all" : "Find all";
                    btnReplace.Visible = btnReplaceAll.Visible = false;
                    btnHighlightAll.Visible = true;
                    btnFindNext.Visible = btnFindPrevious.Visible = false;
                    this.AcceptButton = btnHighlightAll;
                }
                else
                {
                    btnHighlightAll.Text = "Find && Highlight All";
                }

                lblReplaceWith.Visible = txtReplaceWith.Visible = this.ReplaceMode;
            }
        }

        public string LookFor
        {
            get
            {
                return txtLookFor.Text;
            }
        }

        public bool ReplaceMode
        {
            get
            {
                return txtReplaceWith.Visible;
            }

            set
            {
                this.btnReplace.Visible = value;
                this.btnReplaceAll.Visible = value;
                this.lblReplaceWith.Visible = value;
                this.txtReplaceWith.Visible = value;
                this.btnHighlightAll.Visible = !value;
                this.AcceptButton = value ? btnReplace : btnFindNext;
            }
        }

        TextEditorControl Editor
        {
            get
            {
                return this._editor;
            }

            set
            {
                this._editor = value;
                this._search.Document = this._editor.Document;
                this.UpdateTitleBar();
            }
        }

        public TextRange FindNext(bool viaF3, bool searchBackward, string messageIfNotFound)
        {
            if (string.IsNullOrEmpty(txtLookFor.Text))
            {
                MessageBox.Show("No string specified to look for!");
                return null;
            }

            _lastSearchWasBackward = searchBackward;
            this._search.LookFor = txtLookFor.Text;
            this._search.MatchCase = chkMatchCase.Checked;
            this._search.MatchWholeWordOnly = chkMatchWholeWord.Checked;

            var caret = this._editor.ActiveTextAreaControl.Caret;
            if (viaF3 && this._search.HasScanRegion && caret.Offset <= this._search.BeginOffset && caret.Offset >= this._search.EndOffset)
            {
                // user moved outside of the originally selected region
                this._search.ClearScanRegion();
                this.UpdateTitleBar();
            }

            int startFrom = caret.Offset - (searchBackward ? 1 : 0);
            TextRange range = this._search.FindNext(startFrom, searchBackward, out this._lastSearchLoopedAround);
            if (range != null)
            {
                this.SelectResult(range);
            }
            else if (!string.IsNullOrEmpty(messageIfNotFound))
            {
                MessageBox.Show(messageIfNotFound);
            }

            return range;
        }

        public void ShowFor(bool replaceMode, bool bigSearch)
        {
            this._editor = null;
            this._search.ClearScanRegion();

            this.Owner = DockingService.MainForm;
            this.Show();

            this.ReplaceMode = replaceMode;
            this.BigSearch = bigSearch;

            btnFindNext.Focus();

            txtLookFor.SelectAll();
            txtLookFor.Focus();
        }

        public void ShowFor(TextEditorControl editor, bool replaceMode, bool bigSearch)
        {
            this.Editor = editor;

            this.row = this.Editor.ActiveTextAreaControl.Caret.Line;
            this.col = this.Editor.ActiveTextAreaControl.Caret.Column;

            this._search.ClearScanRegion();
            var sm = editor.ActiveTextAreaControl.SelectionManager;
            if (sm.HasSomethingSelected && sm.SelectionCollection.Count == 1)
            {
                var sel = sm.SelectionCollection[0];
                if (sel.StartPosition.Line == sel.EndPosition.Line)
                {
                    txtLookFor.Text = sm.SelectedText;
                }
                else
                {
                    this._search.SetScanRegion(sel);
                }
            }
            else
            {
                // Get the current word that the caret is on
                Caret caret = editor.ActiveTextAreaControl.Caret;
                int start = TextUtilities.FindWordStart(editor.Document, caret.Offset);
                int endAt = TextUtilities.FindWordEnd(editor.Document, caret.Offset);
                txtLookFor.Text = editor.Document.GetText(start, endAt - start);
            }

            this.Owner = (Form)editor.TopLevelControl;
            this.Show();

            this.ReplaceMode = replaceMode;
            this.BigSearch = bigSearch;

            btnFindNext.Focus();

            txtLookFor.SelectAll();
            txtLookFor.Focus();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void btnFindNext_Click(object sender, EventArgs e)
        {
            this.FindNext(false, false, "Text not found");
        }

        private void btnFindPrevious_Click(object sender, EventArgs e)
        {
            this.FindNext(false, true, "Text not found");
        }

        private void btnHighlightAll_Click(object sender, EventArgs e)
        {
            if (this.BigSearch)
            {
                FindResultsWindow results = DockingService.FindResults;
                if (!ProjectService.IsInternal)
                {
                    List<ProjectFile> files = ProjectService.Project.GetProjectFiles();
                    string[] lines;
                    string text;
                    int loc;
                    int indexOfString;
                    try
                    {
                        StreamReader reader;
                        results.NewFindResults(txtLookFor.Text, ProjectService.ProjectName);
                        foreach (ProjectFile file in files)
                        {
                            if (!File.Exists(file.FileFullPath))
                            {
                                continue;
                            }

                            reader = new StreamReader(Path.Combine(ProjectService.ProjectDirectory, file.FileFullPath));
                            lines = reader.ReadToEnd().Split('\n');
                            reader.Close();
                            for (int i = 0; i < lines.Length; i++)
                            {
                                text = lines[i];
                                if (!text.Contains(txtLookFor.Text))
                                {
                                    continue;
                                }

                                loc = 0;
                                indexOfString = text.IndexOf(txtLookFor.Text, loc);
                                while (text.IndexOf(txtLookFor.Text, loc) != -1)
                                {
                                    indexOfString = text.IndexOf(txtLookFor.Text, loc);
                                    loc = indexOfString + 1;
                                    string phrase = text.Substring(indexOfString, txtLookFor.Text.Length);
                                    if (chkMatchCase.Checked && string.Compare(txtLookFor.Text, phrase, false) != 0)
                                    {
                                        continue;
                                    }

                                    if (chkMatchWholeWord.Checked && ((loc + 1 < text.Length && !char.IsWhiteSpace(text[loc + 1]))
                                                                      || (indexOfString - 1 > 0 && !char.IsWhiteSpace(text[indexOfString - 1]))))
                                    {
                                        continue;
                                    }

                                    results.AddFindResult(file.FileFullPath, i, lines[i]);
                                }
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        DockingService.ShowError("Error searching for text!", ex);
                    }
                }

                results.DoneSearching();
                DockingService.ShowDockPanel(results);
            }
            else
            {
                if (!_highlightGroups.ContainsKey(_editor))
                {
                    _highlightGroups[_editor] = new HighlightGroup(_editor);
                }

                HighlightGroup group = _highlightGroups[_editor];

                if (string.IsNullOrEmpty(this.LookFor))
                {
                    // Clear highlights
                    group.ClearMarkers();
                }
                else
                {
                    this._search.LookFor = txtLookFor.Text;
                    this._search.MatchCase = chkMatchCase.Checked;
                    this._search.MatchWholeWordOnly = chkMatchWholeWord.Checked;

                    bool looped = false;
                    int offset = 0, count = 0;
                    for (;;)
                    {
                        TextRange range = this._search.FindNext(offset, false, out looped);
                        if (range == null || looped)
                        {
                            break;
                        }

                        offset = range.Offset + range.Length;
                        count++;
                        var m = new TextMarker(
                            range.Offset,
                            range.Length,
                            TextMarkerType.SolidBlock,
                            Color.Yellow,
                            Color.Black);
                        group.AddMarker(m);
                    }

                    if (count == 0)
                    {
                        MessageBox.Show("Search text not found.");
                    }
                    else
                    {
                        Close();
                    }
                }
            }
        }

        private void btnReplaceAll_Click(object sender, EventArgs e)
        {
            int count = 0;
            /*BUG FIX: if the replacement string contains the original search string
            (e.g. replace "red" with "very red") we must avoid looping around and
            replacing forever! To fix, start replacing at beginning of region (by
            moving the caret) and stop as soon as we loop around.
            _editor.ActiveTextAreaControl.Caret.Position =
               _editor.Document.OffsetToPosition(_search.BeginOffset);*/

            this._editor.Document.UndoStack.StartUndoGroup();
            try
            {
                while (this.FindNext(false, false, null) != null)
                {
                    if (this._lastSearchLoopedAround)
                    {
                        break;
                    }

                    // Replace
                    count++;
                    this.InsertText(txtReplaceWith.Text);
                }
            }
            finally
            {
                this._editor.Document.UndoStack.EndUndoGroup();
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

        private void btnReplace_Click(object sender, EventArgs e)
        {
            var sm = this._editor.ActiveTextAreaControl.SelectionManager;
            if (string.Equals(sm.SelectedText, txtLookFor.Text, StringComparison.OrdinalIgnoreCase))
            {
                this.InsertText(txtReplaceWith.Text);
            }

            this.FindNext(false, _lastSearchWasBackward, "Text not found.");
        }

        private void FindAndReplaceForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Prevent dispose, as this form can be re-used
            if (e.CloseReason != CloseReason.FormOwnerClosing)
            {
                if (this.Owner != null)
                {
                    this.Owner.Select();    // prevent another app from being activated instead
                }

                e.Cancel = true;
                Hide();

                // Discard search region
                this._search.ClearScanRegion();
                if (this._editor != null)
                {
                    this._editor.Refresh(); // must repaint manually
                }
            }
        }

        private void InsertText(string text)
        {
            var textArea = this._editor.ActiveTextAreaControl.TextArea;
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
            TextLocation p1 = this._editor.Document.OffsetToPosition(range.Offset);
            TextLocation p2 = this._editor.Document.OffsetToPosition(range.Offset + range.Length);
            this._editor.ActiveTextAreaControl.SelectionManager.SetSelection(p1, p2);
            this._editor.ActiveTextAreaControl.ScrollTo(p1.Line, p1.Column);

            // Also move the caret to the end of the selection, because when the user
            // presses F3, the caret is where we start searching next time.
            this._editor.ActiveTextAreaControl.Caret.Position =
                this._editor.Document.OffsetToPosition(range.Offset + range.Length);
        }

        private void txtLookFor_TextChanged(object sender, EventArgs e)
        {
        }

        private void UpdateTitleBar()
        {
            string text = this.ReplaceMode ? "Find & replace" : "Find";
            if (this._editor != null && this._editor.FileName != null)
            {
                text += " - " + Path.GetFileName(this._editor.FileName);
            }

            if (this._search.HasScanRegion)
            {
                text += " (selection only)";
            }

            this.Text = text;
        }
    }

    /// <summary>Bundles a group of markers together so that they can be cleared
    /// together.</summary>
    public sealed class HighlightGroup : IDisposable
    {
        private IDocument _document;
        private TextEditorControl _editor;
        private List<TextMarker> _markers = new List<TextMarker>();

        public HighlightGroup(TextEditorControl editor)
        {
            this._editor = editor;
            this._document = editor.Document;
        }

        ~HighlightGroup()
        {
            this.Dispose();
        }

        public IList<TextMarker> Markers
        {
            get
            {
                return this._markers.AsReadOnly();
            }
        }

        public void AddMarker(TextMarker marker)
        {
            this._markers.Add(marker);
            this._document.MarkerStrategy.AddMarker(marker);
        }

        public void ClearMarkers()
        {
            foreach (TextMarker m in this._markers)
            {
                this._document.MarkerStrategy.RemoveMarker(m);
            }

            this._markers.Clear();
            this._editor.Refresh();
        }

        public void Dispose()
        {
            this.ClearMarkers();
            GC.SuppressFinalize(this);
        }
    }

    /// <summary>This class finds occurrances of a search string in a text
    /// editor's IDocument... it's like Find box without a GUI.</summary>
    public sealed class TextEditorSearcher : IDisposable
    {
        public bool MatchCase;
        public bool MatchWholeWordOnly;

        private IDocument _document;
        private string _lookFor;
        private string _lookFor2; // uppercase in case-insensitive mode

        // I would have used the TextAnchor class to represent the beginning and
        // end of the region to scan while automatically adjusting to changes in
        // the document--but for some reason it is sealed and its constructor is
        // internal. Instead I use a TextMarker, which is perhaps even better as
        // it gives me the opportunity to highlight the region. Note that all the
        // markers and coloring information is associated with the text document,
        // not the editor control, so TextEditorSearcher doesn't need a reference
        // to the TextEditorControl. After adding the marker to the document, we
        // must remember to remove it when it is no longer needed.
        private TextMarker _region = null;

        ~TextEditorSearcher()
        {
            this.Dispose();
        }

        /// <summary>Begins the start offset for searching</summary>
        public int BeginOffset
        {
            get
            {
                if (this._region != null)
                {
                    return this._region.Offset;
                }
                else
                {
                    return 0;
                }
            }
        }

        public IDocument Document
        {
            get
            {
                return this._document;
            }

            set
            {
                if (this._document != value)
                {
                    this.ClearScanRegion();
                    this._document = value;
                }
            }
        }

        /// <summary>Begins the end offset for searching</summary>
        public int EndOffset
        {
            get
            {
                if (this._region != null)
                {
                    return this._region.EndOffset;
                }
                else
                {
                    return this._document.TextLength;
                }
            }
        }

        public bool HasScanRegion
        {
            get
            {
                return this._region != null;
            }
        }

        public string LookFor
        {
            get
            {
                return this._lookFor;
            }

            set
            {
                this._lookFor = value;
            }
        }

        public void ClearScanRegion()
        {
            if (this._region != null)
            {
                this._document.MarkerStrategy.RemoveMarker(this._region);
                this._region = null;
            }
        }

        public void Dispose()
        {
            this.ClearScanRegion();
            GC.SuppressFinalize(this);
        }

        /// <summary>Finds next instance of LookFor, according to the search rules
        /// (MatchCase, MatchWholeWordOnly).</summary>
        /// <param name="beginAtOffset">Offset in Document at which to begin the search</param>
        /// <remarks>If there is a match at beginAtOffset precisely, it will be returned.</remarks>
        /// <returns>Region of document that matches the search string</returns>
        public TextRange FindNext(int beginAtOffset, bool searchBackward, out bool loopedAround)
        {
            Debug.Assert(!string.IsNullOrEmpty(this._lookFor));
            loopedAround = false;

            int startAt = this.BeginOffset, endAt = this.EndOffset;
            int curOffs = beginAtOffset; // .InRange(startAt, endAt);

            this._lookFor2 = this.MatchCase ? this._lookFor : this._lookFor.ToUpperInvariant();

            TextRange result;
            if (searchBackward)
            {
                result = this.FindNextIn(startAt, curOffs, true);
                if (result == null)
                {
                    loopedAround = true;
                    result = this.FindNextIn(curOffs, endAt, true);
                }
            }
            else
            {
                result = this.FindNextIn(curOffs, endAt, false);
                if (result == null)
                {
                    loopedAround = true;
                    result = this.FindNextIn(startAt, curOffs, false);
                }
            }

            return result;
        }

        /// <summary>Sets the region to search. The region is updated
        /// automatically as the document changes.</summary>
        public void SetScanRegion(ISelection sel)
        {
            this.SetScanRegion(sel.Offset, sel.Length);
        }

        /// <summary>Sets the region to search. The region is updated
        /// automatically as the document changes.</summary>
        public void SetScanRegion(int offset, int length)
        {
            var bkgColor = this._document.HighlightingStrategy.GetColorFor("Default").BackgroundColor;
            this._region = new TextMarker(
                offset,
                length,
                TextMarkerType.SolidBlock,
                bkgColor,
                (
                    Color.FromArgb(
                        160,
                        160,
                        160)));
            this._document.MarkerStrategy.AddMarker(this._region);
        }

        private TextRange FindNextIn(int offset1, int offset2, bool searchBackward)
        {
            Debug.Assert(offset2 >= offset1);
            offset2 -= this._lookFor.Length;

            // Make behavior decisions before starting search loop
            bool matchFirstChar;
            bool matchWord;

            // Search
            char lookForCh = this._lookFor2[0];
            if (searchBackward)
            {
                for (int offset = offset2; offset >= offset1; offset--)
                {
                    if (!this.MatchCase)
                    {
                        matchFirstChar = lookForCh == char.ToUpperInvariant(this._document.GetCharAt(offset));
                    }
                    else
                    {
                        matchFirstChar = lookForCh == this._document.GetCharAt(offset);
                    }

                    if (this.MatchWholeWordOnly)
                    {
                        matchWord = this.IsWholeWordMatch(offset);
                    }
                    else
                    {
                        matchWord = this.IsPartWordMatch(offset);
                    }

                    if (matchFirstChar && matchWord)
                    {
                        return new TextRange(this._document, offset, this._lookFor.Length);
                    }
                }
            }
            else
            {
                for (int offset = offset1; offset <= offset2; offset++)
                {
                    if (!this.MatchCase)
                    {
                        matchFirstChar = lookForCh == char.ToUpperInvariant(this._document.GetCharAt(offset));
                    }
                    else
                    {
                        matchFirstChar = lookForCh == this._document.GetCharAt(offset);
                    }

                    if (this.MatchWholeWordOnly)
                    {
                        matchWord = this.IsWholeWordMatch(offset);
                    }
                    else
                    {
                        matchWord = this.IsPartWordMatch(offset);
                    }

                    if (matchFirstChar && matchWord)
                    {
                        return new TextRange(this._document, offset, this._lookFor.Length);
                    }
                }
            }

            return null;
        }

        private bool IsAlphaNumeric(int offset)
        {
            char c = this._document.GetCharAt(offset);
            return char.IsLetterOrDigit(c) || c == '_';
        }

        private bool IsPartWordMatch(int offset)
        {
            string substr = this._document.GetText(offset, this._lookFor.Length);
            if (!this.MatchCase)
            {
                substr = substr.ToUpperInvariant();
            }

            return substr == this._lookFor2;
        }

        private bool IsWholeWordMatch(int offset)
        {
            if (this.IsWordBoundary(offset) && this.IsWordBoundary(offset + this._lookFor.Length))
            {
                return this.IsPartWordMatch(offset);
            }
            else
            {
                return false;
            }
        }

        private bool IsWordBoundary(int offset)
        {
            return offset <= 0 || offset >= this._document.TextLength ||
                   !this.IsAlphaNumeric(offset - 1) || !this.IsAlphaNumeric(offset);
        }
    }

    public class TextRange : AbstractSegment
    {
        private IDocument _document;

        public TextRange(IDocument document, int offset, int length)
        {
            this._document = document;
            this.offset = offset;
            this.length = length;
        }
    }
}