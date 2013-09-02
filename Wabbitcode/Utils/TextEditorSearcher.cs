using System;
using System.Diagnostics;
using System.Drawing;
using Revsoft.TextEditor.Document;

namespace Revsoft.Wabbitcode.Utils
{
	/// <summary>This class finds occurrances of a search string in a text
	/// editor's IDocument... it's like Find box without a GUI.
	/// </summary>
	public sealed class TextEditorSearcher : IDisposable
	{
		public bool MatchCase;
		public bool MatchWholeWordOnly;

		private IDocument _document;
		private string _lookFor;
		private string _lookForUppercase; // uppercase in case-insensitive mode

		// I would have used the TextAnchor class to represent the beginning and
		// end of the region to scan while automatically adjusting to changes in
		// the document--but for some reason it is sealed and its constructor is
		// internal. Instead I use a TextMarker, which is perhaps even better as
		// it gives me the opportunity to highlight the region. Note that all the
		// markers and coloring information is associated with the text document,
		// not the editor control, so TextEditorSearcher doesn't need a reference
		// to the TextEditorControl. After adding the marker to the document, we
		// must remember to remove it when it is no longer needed.
		private TextMarker _region;

		~TextEditorSearcher()
		{
			Dispose();
		}

		/// <summary>Begins the start offset for searching</summary>
		public int BeginOffset
		{
			get
			{
				if (_region != null)
				{
					return _region.Offset;
				}
				return 0;
			}
		}

		public IDocument Document
		{
			set
			{
				if (_document != value)
				{
					ClearScanRegion();
					_document = value;
				}
			}
		}

		/// <summary>Begins the end offset for searching</summary>
		public int EndOffset
		{
			get
			{
				if (_region != null)
				{
					return _region.EndOffset;
				}
				return _document.TextLength;
			}
		}

		public bool HasScanRegion
		{
			get { return _region != null; }
		}

		public string LookFor
		{
			set { _lookFor = value; }
		}

		public void ClearScanRegion()
		{
			if (_region != null)
			{
				_document.MarkerStrategy.RemoveMarker(_region);
				_region = null;
			}
		}

		public void Dispose()
		{
			ClearScanRegion();
			GC.SuppressFinalize(this);
		}

		/// <summary>Finds next instance of LookFor, according to the search rules
		/// (MatchCase, MatchWholeWordOnly).</summary>
		/// <param name="beginAtOffset">Offset in Document at which to begin the search</param>
		/// <param name="searchBackward"></param>
		/// <param name="loopedAround"></param>
		/// <remarks>If there is a match at beginAtOffset precisely, it will be returned.</remarks>
		/// <returns>Region of document that matches the search string</returns>
		public TextRange FindNext(int beginAtOffset, bool searchBackward, out bool loopedAround)
		{
			Debug.Assert(!string.IsNullOrEmpty(_lookFor));
			loopedAround = false;

			int startAt = BeginOffset, endAt = EndOffset;
			int curOffs = beginAtOffset; // .InRange(startAt, endAt);

			_lookForUppercase = MatchCase ? _lookFor : _lookFor.ToUpperInvariant();

			TextRange result;
			if (searchBackward)
			{
				result = FindNextIn(startAt, curOffs, true);
				if (result == null)
				{
					loopedAround = true;
					result = FindNextIn(curOffs, endAt, true);
				}
			}
			else
			{
				result = FindNextIn(curOffs, endAt, false);
				if (result == null)
				{
					loopedAround = true;
					result = FindNextIn(startAt, curOffs, false);
				}
			}

			return result;
		}

		/// <summary>Sets the region to search. The region is updated
		/// automatically as the document changes.</summary>
		public void SetScanRegion(ISelection sel)
		{
			SetScanRegion(sel.Offset, sel.Length);
		}

		/// <summary>Sets the region to search. The region is updated
		/// automatically as the document changes.</summary>
		private void SetScanRegion(int offset, int length)
		{
			var bkgColor = _document.HighlightingStrategy.GetColorFor("Default").BackgroundColor;
			_region = new TextMarker(
				offset,
				length,
				TextMarkerType.SolidBlock,
				bkgColor,
				(
					Color.FromArgb(
						160,
						160,
						160)));
			_document.MarkerStrategy.AddMarker(_region);
		}

		private TextRange FindNextIn(int offset1, int offset2, bool searchBackward)
		{
			Debug.Assert(offset2 >= offset1);
			offset2 -= _lookFor.Length;

			// Make behavior decisions before starting search loop
			bool matchFirstChar;
			bool matchWord;

			// Search
			char lookForCh = _lookForUppercase[0];
			if (searchBackward)
			{
				for (int offset = offset2; offset >= offset1; offset--)
				{
					if (!MatchCase)
					{
						matchFirstChar = lookForCh == char.ToUpperInvariant(_document.GetCharAt(offset));
					}
					else
					{
						matchFirstChar = lookForCh == _document.GetCharAt(offset);
					}

					matchWord = MatchWholeWordOnly ? IsWholeWordMatch(offset) : IsPartWordMatch(offset);

					if (matchFirstChar && matchWord)
					{
						return new TextRange(offset, _lookFor.Length);
					}
				}
			}
			else
			{
				for (int offset = offset1; offset <= offset2; offset++)
				{
					if (!MatchCase)
					{
						matchFirstChar = lookForCh == char.ToUpperInvariant(_document.GetCharAt(offset));
					}
					else
					{
						matchFirstChar = lookForCh == _document.GetCharAt(offset);
					}

					matchWord = MatchWholeWordOnly ? IsWholeWordMatch(offset) : IsPartWordMatch(offset);

					if (matchFirstChar && matchWord)
					{
						return new TextRange(offset, _lookFor.Length);
					}
				}
			}

			return null;
		}

		private bool IsAlphaNumeric(int offset)
		{
			char c = _document.GetCharAt(offset);
			return char.IsLetterOrDigit(c) || c == '_';
		}

		private bool IsPartWordMatch(int offset)
		{
			string substr = _document.GetText(offset, _lookFor.Length);
			if (!MatchCase)
			{
				substr = substr.ToUpperInvariant();
			}

			return substr == _lookForUppercase;
		}

		private bool IsWholeWordMatch(int offset)
		{
			if (IsWordBoundary(offset) && IsWordBoundary(offset + _lookFor.Length))
			{
				return IsPartWordMatch(offset);
			}
			return false;
		}

		private bool IsWordBoundary(int offset)
		{
			return offset <= 0 || offset >= _document.TextLength ||
			       !IsAlphaNumeric(offset - 1) || !IsAlphaNumeric(offset);
		}
	}
}