// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 3272 $</version>
// </file>

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Drawing;
using Revsoft.TextEditor.Util;

namespace Revsoft.TextEditor.Document
{
	public interface IBreakpointFactory
	{
		Breakpoint CreateBreakpoint(IDocument document, TextLocation location);
	}
	
	/// <summary>
	/// This class handles the breakpoints for a buffer
	/// </summary>
	public class BreakpointManager
	{
		IDocument      document;
		#if DEBUG
		IList<Breakpoint> breakpoint = new CheckedList<Breakpoint>();
		#else
		List<Breakpoint> breakpoint = new List<Breakpoint>();
		#endif
		
		/// <value>
		/// Contains all breakpoints
		/// </value>
		public ReadOnlyCollection<Breakpoint> Marks {
			get {
				return new ReadOnlyCollection<Breakpoint>(breakpoint);
			}
		}
		
		public IDocument Document {
			get {
				return document;
			}
		}
		
		/// <summary>
		/// Creates a new instance of <see cref="BreakpointManager"/>
		/// </summary>
		internal BreakpointManager(IDocument document)
		{
			this.document = document;
		}
		
		/// <summary>
		/// Gets/Sets the breakpoint factory used to create breakpoints for "ToggleMarkAt".
		/// </summary>
		public IBreakpointFactory Factory { get; set;}
		
		/// <summary>
		/// Sets the mark at the line <code>location.Line</code> if it is not set, if the
		/// line is already marked the mark is cleared.
		/// </summary>
		public void ToggleMarkAt(TextLocation location)
		{
			Breakpoint newMark;
			newMark = Factory != null ? Factory.CreateBreakpoint(document, location) :
                                                new Breakpoint(document, location);

			Type newMarkType = newMark.GetType();
			
			for (int i = 0; i < breakpoint.Count; ++i) {
				Breakpoint mark = breakpoint[i];

			    if (mark.LineNumber != location.Line || !mark.CanToggle || mark.GetType() != newMarkType) 
                    continue;
			    RemoveMark(mark);
                //breakpoint.RemoveAt(i);
			    //OnRemoved(new BreakpointEventArgs(mark));
			    return;
			}
		    AddMark(newMark);
			//breakpoint.Add(newMark);
			//OnAdded(new BreakpointEventArgs(newMark));
		}
		
		public void AddMark(Breakpoint mark)
		{
            //Adds the marker
            if (addMarker(mark) == false)
                return;
            breakpoint.Add(mark);
			OnAdded(new BreakpointEventArgs(mark));
		}

        public void RemoveMark(Breakpoint mark)
		{
            removeMarker(mark);
			OnRemoved(new BreakpointEventArgs(mark));
            breakpoint.Remove(mark);
		}
		
		public void RemoveMarks(Predicate<Breakpoint> predicate)
		{
			for (int i = 0; i < breakpoint.Count; ++i) {
				Breakpoint bm = breakpoint[i];
			    if (!predicate(bm)) 
                    continue;
			    breakpoint.RemoveAt(i--);
			    OnRemoved(new BreakpointEventArgs(bm));
			}
		}
		
		/// <returns>
		/// true, if a mark at mark exists, otherwise false
		/// </returns>
		public bool IsMarked(int lineNr)
		{
			for (int i = 0; i < breakpoint.Count; ++i) {
				if (breakpoint[i].LineNumber == lineNr) {
					return true;
				}
			}
			return false;
		}

        public bool addMarker(Breakpoint mark)
        {
            TextMarker marker = getMarker(mark.LineNumber);
            if (marker == null)
                return false;
            document.MarkerStrategy.AddMarker(marker);
            return true;
        }

	    public void removeMarker(Breakpoint mark)
        {
			
            TextMarker marker = getMarker(mark.LineNumber);
			List<TextMarker> markers = document.MarkerStrategy.GetMarkers(document.GetOffsetForLineNumber(mark.LineNumber) + 
				document.FormattingStrategy.GetIndentation(document, mark.LineNumber).Length);
			if (markers.Count > 0)
			{
				int i = 0;
                while (markers[i].Offset != marker.Offset && i < markers.Count)
                {
                    i++;
                    if (i == markers.Count)
                        return;
                }
				document.MarkerStrategy.RemoveMarker(markers[i]);
			}
			/*else
			{
				markers = document.MarkerStrategy.GetMarkers(document.GetOffsetForLineNumber(mark.LineNumber) + mark.ColumnNumber - 1);
				if (markers.Count > 0)
				{
					int i = 0;
					while (markers[i].Offset != marker.Offset && i < markers.Count)
						i++;
					document.MarkerStrategy.RemoveMarker(markers[i]);
				}
			}*/

            document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, mark.LineNumber));
            document.CommitUpdate();
        }

	    private TextMarker getMarker(int lineNumber)
        {
            int start = document.GetOffsetForLineNumber(lineNumber);
            int length = document.TextContent.Split('\n')[lineNumber].Length;
            while (start == document.TextContent.Length || (start > 0 && document.TextContent[start] != '\n'))
                start--;
            start++;
            length--;
            if (char.IsLetterOrDigit(document.TextContent[start]))
                return null;
            while (document.TextContent[start] == ' ' || document.TextContent[start] == '\t')
            {
                start++;
                length--;
            }
            if (length < 0)
                length = 1;
            if (document.TextContent.IndexOf(';', start, length) != -1)
                length = document.TextContent.IndexOf(';', start, length) - start - 1;
            while (document.TextContent[start + length] == ' ' || document.TextContent[start + length] == '\t')
                length--;
            length++;
            if (length < 0)
                return null;
            List<FoldMarker> foldings = document.FoldingManager.GetFoldingsContainsLineNumber(lineNumber);
            if (foldings.Count > 0)
                foreach (FoldMarker fold in foldings)
                    if (fold.InnerText.ToLower().EndsWith("#endcomment"))
                        return null;
            if (document.TextContent.Substring(start, length).StartsWith("#") || document.TextContent.Substring(start, length).StartsWith(".") || document.TextContent.Substring(start, length).Trim().Length == 0)
                return null;
            return new TextMarker(start, length, TextMarkerType.SolidBlock, Color.Maroon, Color.White) { Tag = "Breakpoint" };
        }

	    /// <remarks>
		/// Clears all breakpoint
		/// </remarks>
		public void Clear()
		{
			foreach (Breakpoint mark in breakpoint) {
                OnRemoved(new BreakpointEventArgs(mark));
                removeMarker(mark);
			}
			breakpoint.Clear();
		}
		
		/// <value>
		/// The lowest mark, if no marks exists it returns -1
		/// </value>
		public Breakpoint GetFirstMark(Predicate<Breakpoint> predicate)
		{
			if (breakpoint.Count < 1) {
				return null;
			}
			Breakpoint first = null;
			for (int i = 0; i < breakpoint.Count; ++i) {
				if (predicate(breakpoint[i]) && breakpoint[i].IsEnabled && (first == null || breakpoint[i].LineNumber < first.LineNumber)) {
					first = breakpoint[i];
				}
			}
			return first;
		}
		
		/// <value>
		/// The highest mark, if no marks exists it returns -1
		/// </value>
		public Breakpoint GetLastMark(Predicate<Breakpoint> predicate)
		{
			if (breakpoint.Count < 1) {
				return null;
			}
			Breakpoint last = null;
			for (int i = 0; i < breakpoint.Count; ++i) {
				if (predicate(breakpoint[i]) && breakpoint[i].IsEnabled && (last == null || breakpoint[i].LineNumber > last.LineNumber)) {
					last = breakpoint[i];
				}
			}
			return last;
		}
		bool AcceptAnyMarkPredicate(Breakpoint mark)
		{
			return true;
		}
		public Breakpoint GetNextMark(int curLineNr)
		{
			return GetNextMark(curLineNr, AcceptAnyMarkPredicate);
		}
		
		/// <remarks>
		/// returns first mark higher than <code>lineNr</code>
		/// </remarks>
		/// <returns>
		/// returns the next mark > cur, if it not exists it returns FirstMark()
		/// </returns>
		public Breakpoint GetNextMark(int curLineNr, Predicate<Breakpoint> predicate)
		{
			if (breakpoint.Count == 0) {
				return null;
			}
			
			Breakpoint next = GetFirstMark(predicate);
			foreach (Breakpoint mark in breakpoint) {
				if (predicate(mark) && mark.IsEnabled && mark.LineNumber > curLineNr) {
					if (mark.LineNumber < next.LineNumber || next.LineNumber <= curLineNr) {
						next = mark;
					}
				}
			}
			return next;
		}
		
		public Breakpoint GetPrevMark(int curLineNr)
		{
			return GetPrevMark(curLineNr, AcceptAnyMarkPredicate);
		}
		/// <remarks>
		/// returns first mark lower than <code>lineNr</code>
		/// </remarks>
		/// <returns>
		/// returns the next mark lower than cur, if it not exists it returns LastMark()
		/// </returns>
		public Breakpoint GetPrevMark(int curLineNr, Predicate<Breakpoint> predicate)
		{
			if (breakpoint.Count == 0) {
				return null;
			}
			
			Breakpoint prev = GetLastMark(predicate);
			
			foreach (Breakpoint mark in breakpoint) {
				if (predicate(mark) && mark.IsEnabled && mark.LineNumber < curLineNr) {
					if (mark.LineNumber > prev.LineNumber || prev.LineNumber >= curLineNr) {
						prev = mark;
					}
				}
			}
			return prev;
		}
		
		protected virtual void OnRemoved(BreakpointEventArgs e)
		{
			if (Removed != null) {
				Removed(this, e);
			}
		}
		
		protected virtual void OnAdded(BreakpointEventArgs e)
		{
			if (Added != null) {
				Added(this, e);
			}
		}
		
		public event BreakpointEventHandler Removed;
		public event BreakpointEventHandler Added;
	}
}
