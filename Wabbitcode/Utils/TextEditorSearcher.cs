using System.Diagnostics;
using Revsoft.Wabbitcode.GUI.Dialogs;

namespace Revsoft.Wabbitcode.Utils
{
    public sealed class TextEditorSearcher
    {
        public bool MatchCase;
        public bool MatchWholeWordOnly;

        private readonly string _fileText;
        private string _lookFor;
        private string _lookForUppercase; // uppercase in case-insensitive mode

        public TextEditorSearcher(string fileText)
        {
            _fileText = fileText;
        }

        public string LookFor
        {
            set { _lookFor = value; }
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

            const int startAt = 0;
            int endAt = _fileText.Length;
            int curOffs = beginAtOffset; // .InRange(startAt, endAt);

            _lookForUppercase = MatchCase ? _lookFor : _lookFor.ToUpperInvariant();

            TextRange result;
            if (searchBackward)
            {
                result = FindNextIn(startAt, curOffs, true);
                if (result != null)
                {
                    return result;
                }

                loopedAround = true;
                result = FindNextIn(curOffs, endAt, true);
            }
            else
            {
                result = FindNextIn(curOffs, endAt, false);
                if (result != null)
                {
                    return result;
                }

                loopedAround = true;
                result = FindNextIn(startAt, curOffs, false);
            }

            return result;
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
                        matchFirstChar = lookForCh == char.ToUpperInvariant(_fileText[offset]);
                    }
                    else
                    {
                        matchFirstChar = lookForCh == _fileText[offset];
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
                        matchFirstChar = lookForCh == char.ToUpperInvariant(_fileText[offset]);
                    }
                    else
                    {
                        matchFirstChar = lookForCh == _fileText[offset];
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
            char c = _fileText[offset];
            return char.IsLetterOrDigit(c) || c == '_';
        }

        private bool IsPartWordMatch(int offset)
        {
            string substr = _fileText.Substring(offset, _lookFor.Length);
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
            return offset <= 0 || offset >= _fileText.Length ||
                   !IsAlphaNumeric(offset - 1) || !IsAlphaNumeric(offset);
        }
    }
}