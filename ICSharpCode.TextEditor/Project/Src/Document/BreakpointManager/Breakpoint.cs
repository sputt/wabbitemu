// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 3272 $</version>
// </file>

using System;
using System.Drawing;
using SWF = System.Windows.Forms;

namespace Revsoft.TextEditor.Document
{
	/// <summary>
	/// Description of Breakpoint.
	/// </summary>
	public class Breakpoint
	{
		IDocument document;
		TextAnchor anchor;
		TextLocation location;
		bool isEnabled = true;
		
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
		
		void CreateAnchor()
		{
		    if (document == null) return;
		    LineSegment line = document.GetLineSegment(Math.Max(0, Math.Min(location.Line, document.TotalNumberOfLines-1)));
		    anchor = line.CreateAnchor(Math.Max(0, Math.Min(location.Column, line.Length)));
		    // after insertion: keep breakpoints after the initial whitespace (see DefaultFormattingStrategy.SmartReplaceLine)
		    anchor.MovementType = AnchorMovementType.AfterInsertion;
		    anchor.Deleted += AnchorDeleted;
		}
		
		void AnchorDeleted(object sender, EventArgs e)
		{
			document.BreakpointManager.RemoveMark(this);
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
		
		public event EventHandler DocumentChanged;
		
		protected virtual void OnDocumentChanged(EventArgs e)
		{
			if (DocumentChanged != null) {
				DocumentChanged(this, e);
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
		
		public event EventHandler IsEnabledChanged;
		
		protected virtual void OnIsEnabledChanged(EventArgs e)
		{
			if (IsEnabledChanged != null) {
				IsEnabledChanged(this, e);
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
		public virtual bool CanToggle {
			get {
				return true;
			}
		}
		
		public Breakpoint(IDocument document, TextLocation location) : this(document, location, true)
		{
		}
		
		public Breakpoint(IDocument document, TextLocation location, bool isEnabled)
		{
			this.document = document;
			this.isEnabled = isEnabled;
			Location = location;
		}
		
		public virtual bool Click(SWF.Control parent, SWF.MouseEventArgs e)
		{
			if (e.Button == SWF.MouseButtons.Left && CanToggle) {
                Actions.IEditAction newBreakpoint = new Actions.ToggleBreakpoint();
                newBreakpoint.Execute((TextArea)parent);
                //document.BreakpointManager.ToggleMarkAt(this.location);
				//document.BreakpointManager.RemoveMark(this);
				return true;
			}
			return false;
		}
		
		public virtual void Draw(IconBarMargin margin, Graphics g, Point p)
		{
            //WTF ISHEALTHY? DAMN STRAIGHT FUCKER OWNED. WHO WANTS A GREEN BREAKPOINT ANYWAY?
			margin.DrawBreakpoint(g, p.Y, isEnabled);
		}
	}
}
