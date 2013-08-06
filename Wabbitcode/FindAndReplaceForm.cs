using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode
{
	public partial class FindAndReplaceForm : Form
	{
		private bool _lastSearchLoopedAround;
		private bool _lastSearchWasBackward;

		private TextEditorControl _editor;
		private readonly Dictionary<TextEditorControl, HighlightGroup> _highlightGroups = new Dictionary<TextEditorControl, HighlightGroup>();
		private readonly TextEditorSearcher _search;
		private readonly IDockingService _dockingService;
		private readonly IProject _project;

		public FindAndReplaceForm(IDockingService dockingService, IProject project)
		{
			_dockingService = dockingService;
			_project = project;

			InitializeComponent();
			_search = new TextEditorSearcher();
		}

		private bool BigSearch
		{
			get
			{
				return btnHighlightAll.Text == "Find all" || btnHighlightAll.Text == "Replace All";
			}

			set
			{
				if (value)
				{
					btnHighlightAll.Text = ReplaceMode ? "Replace all" : "Find all";
					btnReplace.Visible = btnReplaceAll.Visible = false;
					btnHighlightAll.Visible = true;
					btnFindNext.Visible = btnFindPrevious.Visible = false;
					AcceptButton = btnHighlightAll;
				}
				else
				{
					btnHighlightAll.Text = "Find && Highlight All";
				}

				lblReplaceWith.Visible = txtReplaceWith.Visible = ReplaceMode;
			}
		}

		private string LookFor
		{
			get
			{
				return txtLookFor.Text;
			}
		}

		private bool ReplaceMode
		{
			get
			{
				return txtReplaceWith.Visible;
			}

			set
			{
				btnReplace.Visible = value;
				btnReplaceAll.Visible = value;
				lblReplaceWith.Visible = value;
				txtReplaceWith.Visible = value;
				btnHighlightAll.Visible = !value;
				AcceptButton = value ? btnReplace : btnFindNext;
			}
		}

		TextEditorControl Editor
		{
			set
			{
				_editor = value;
				_search.Document = _editor.Document;
				UpdateTitleBar();
			}
		}

		private TextRange FindNext(bool viaF3, bool searchBackward, string messageIfNotFound)
		{
			if (string.IsNullOrEmpty(txtLookFor.Text))
			{
				MessageBox.Show("No string specified to look for!");
				return null;
			}

			_lastSearchWasBackward = searchBackward;
			_search.LookFor = txtLookFor.Text;
			_search.MatchCase = chkMatchCase.Checked;
			_search.MatchWholeWordOnly = chkMatchWholeWord.Checked;

			var caret = _editor.ActiveTextAreaControl.Caret;
			if (viaF3 && _search.HasScanRegion && caret.Offset <= _search.BeginOffset && caret.Offset >= _search.EndOffset)
			{
				// user moved outside of the originally selected region
				_search.ClearScanRegion();
				UpdateTitleBar();
			}

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

		public void ShowFor(Form owner, bool replaceMode, bool bigSearch)
		{
			_editor = null;
			_search.ClearScanRegion();

			Owner = owner;
			Show();

			ReplaceMode = replaceMode;
			BigSearch = bigSearch;

			btnFindNext.Focus();

			txtLookFor.SelectAll();
			txtLookFor.Focus();
		}

		public void ShowFor(TextEditorControl editor, bool replaceMode, bool bigSearch)
		{
			Editor = editor;

			_search.ClearScanRegion();
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
					_search.SetScanRegion(sel);
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

			Owner = (Form)editor.TopLevelControl;
			Show();

			ReplaceMode = replaceMode;
			BigSearch = bigSearch;

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
			FindNext(false, false, "Text not found");
		}

		private void btnFindPrevious_Click(object sender, EventArgs e)
		{
			FindNext(false, true, "Text not found");
		}

		private void btnHighlightAll_Click(object sender, EventArgs e)
		{
			if (BigSearch)
			{
				FindResultsWindow results = _dockingService.FindResults;
				if (!_project.IsInternal)
				{
					IEnumerable<ProjectFile> files = _project.GetProjectFiles();
					try
					{
						results.NewFindResults(txtLookFor.Text, _project.ProjectName);
						foreach (ProjectFile file in files)
						{
							if (!File.Exists(file.FileFullPath))
							{
								continue;
							}

							StreamReader reader = new StreamReader(Path.Combine(_project.ProjectDirectory, file.FileFullPath));
							string[] lines = reader.ReadToEnd().Split('\n');
							reader.Close();
							for (int i = 0; i < lines.Length; i++)
							{
								string text = lines[i];
								if (!text.Contains(txtLookFor.Text))
								{
									continue;
								}

								int loc = 0;
								while (text.IndexOf(txtLookFor.Text, loc) != -1)
								{
									int indexOfString = text.IndexOf(txtLookFor.Text, loc);
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
				_dockingService.ShowDockPanel(results);
			}
			else
			{
				if (!_highlightGroups.ContainsKey(_editor))
				{
					_highlightGroups[_editor] = new HighlightGroup(_editor);
				}

				HighlightGroup group = _highlightGroups[_editor];

				if (string.IsNullOrEmpty(LookFor))
				{
					// Clear highlights
					group.ClearMarkers();
				}
				else
				{
					_search.LookFor = txtLookFor.Text;
					_search.MatchCase = chkMatchCase.Checked;
					_search.MatchWholeWordOnly = chkMatchWholeWord.Checked;

					int offset = 0, count = 0;
					for (; ; )
					{
						bool looped;
						TextRange range = _search.FindNext(offset, false, out looped);
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

			_editor.Document.UndoStack.StartUndoGroup();
			try
			{
				while (FindNext(false, false, null) != null)
				{
					if (_lastSearchLoopedAround)
					{
						break;
					}

					// Replace
					count++;
					InsertText(txtReplaceWith.Text);
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

		private void btnReplace_Click(object sender, EventArgs e)
		{
			var sm = _editor.ActiveTextAreaControl.SelectionManager;
			if (string.Equals(sm.SelectedText, txtLookFor.Text, StringComparison.OrdinalIgnoreCase))
			{
				InsertText(txtReplaceWith.Text);
			}

			FindNext(false, _lastSearchWasBackward, "Text not found.");
		}

		private void FindAndReplaceForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			// Prevent dispose, as this form can be re-used
			if (e.CloseReason != CloseReason.FormOwnerClosing)
			{
				if (Owner != null)
				{
					Owner.Select();    // prevent another app from being activated instead
				}

				e.Cancel = true;
				Hide();

				// Discard search region
				_search.ClearScanRegion();
				if (_editor != null)
				{
					_editor.Refresh(); // must repaint manually
				}
			}
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

		private void UpdateTitleBar()
		{
			string text = ReplaceMode ? "Find & replace" : "Find";
			if (_editor != null && _editor.FileName != null)
			{
				text += " - " + Path.GetFileName(_editor.FileName);
			}

			if (_search.HasScanRegion)
			{
				text += " (selection only)";
			}

			Text = text;
		}
	}

	/// <summary>Bundles a group of markers together so that they can be cleared
	/// together.</summary>
	public sealed class HighlightGroup : IDisposable
	{
		private readonly IDocument _document;
		private readonly TextEditorControl _editor;
		private readonly List<TextMarker> _markers = new List<TextMarker>();

		public HighlightGroup(TextEditorControl editor)
		{
			_editor = editor;
			_document = editor.Document;
		}

		~HighlightGroup()
		{
			Dispose();
		}

		public void AddMarker(TextMarker marker)
		{
			_markers.Add(marker);
			_document.MarkerStrategy.AddMarker(marker);
		}

		public void ClearMarkers()
		{
			foreach (TextMarker m in _markers)
			{
				_document.MarkerStrategy.RemoveMarker(m);
			}

			_markers.Clear();
			_editor.Refresh();
		}

		public void Dispose()
		{
			ClearMarkers();
			GC.SuppressFinalize(this);
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