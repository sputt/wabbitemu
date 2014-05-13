using System;
using System.Drawing;

namespace Revsoft.TextEditor.Document
{
    public abstract class AbstractMarginIcon
    {
        protected IDocument document;
        protected TextAnchor anchor;
        protected TextLocation location;
        protected bool isEnabled = true;

        protected AbstractMarginIcon(IDocument document, TextLocation location, bool isEnabled = true)
        {
            this.document = document;
            this.isEnabled = isEnabled;
            Location = location;
        }

        public IDocument Document {
            get {
                return document;
            }
            set {
                if (document == value) return;
                if (anchor != null) {
                    location = anchor.Location;
                    anchor = null;
                }
                document = value;
                CreateAnchor();
                OnDocumentChanged(EventArgs.Empty);
            }
        }

        /// <summary>
        /// Gets the TextAnchor used for this breakpoint.
        /// Is null if the breakpoint is not connected to a document.
        /// </summary>
        public TextAnchor Anchor {
            get { return anchor; }
        }

        public TextLocation Location {
            get
            {
                if (anchor.IsDeleted)
                    return location;
                return anchor != null ? anchor.Location : location;
            }
            set {
                location = value;
                CreateAnchor();
            }
        }

        public bool IsEnabled {
            get {
                return isEnabled;
            }
            set {
                if (isEnabled == value) 
                    return;
                isEnabled = value;
                if (document != null) {
                    document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, LineNumber));
                    document.CommitUpdate();
                }

                OnIsEnabledChanged(EventArgs.Empty);
            }
        }

        public int LineNumber {
            get {
                if (anchor.IsDeleted)
                    return location.Line;
                return anchor != null ? anchor.LineNumber : location.Line;
            }
        }

        public int ColumnNumber {
            get {
                return anchor != null ? anchor.ColumnNumber : location.Column;
            }
        }

        /// <summary>
        /// Gets if the breakpoint can be toggled off using the 'set/unset breakpoint' command.
        /// </summary>
        public bool CanToggle {
            get {
                return true;
            }
        }

        public event EventHandler DocumentChanged;
        public event EventHandler IsEnabledChanged;

        private void CreateAnchor()
        {
            if (document == null) return;
            LineSegment line = document.GetLineSegment(Math.Max(0, Math.Min(location.Line, document.TotalNumberOfLines - 1)));
            anchor = line.CreateAnchor(Math.Max(0, Math.Min(location.Column, line.Length)));
            // after insertion: keep breakpoints after the initial whitespace (see DefaultFormattingStrategy.SmartReplaceLine)
            anchor.MovementType = AnchorMovementType.AfterInsertion;
            anchor.Deleted += AnchorDeleted;
        }

        private void OnDocumentChanged(EventArgs e)
        {
            if (DocumentChanged != null)
            {
                DocumentChanged(this, e);
            }
        }

        private void OnIsEnabledChanged(EventArgs e)
        {
            if (IsEnabledChanged != null)
            {
                IsEnabledChanged(this, e);
            }
        }

        protected abstract void AnchorDeleted(object sender, EventArgs e);

        public abstract bool Click(System.Windows.Forms.Control parent, System.Windows.Forms.MouseEventArgs e);

        public abstract void Draw(IconBarMargin margin, Graphics g, Point p);
    }
}