// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 3272 $</version>
// </file>

using System;
using Revsoft.TextEditor.Document;

namespace Revsoft.TextEditor.Actions 
{
	public class ToggleBreakpoint : AbstractEditAction
	{
		private TextLocation? _location = null;

		public ToggleBreakpoint()
		{
		}

		public ToggleBreakpoint(TextLocation location)
		{
			_location = location;
		}

		public override void Execute(TextArea textArea)
        {
			if (_location == null)
			{
				_location = textArea.Caret.Position;
			}
            textArea.Document.BreakpointManager.ToggleMarkAt(_location.Value);
            textArea.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.SingleLine, _location.Value.Line));
            textArea.Document.CommitUpdate();
			
		}
	}
	
	public class GotoPrevBreakpoint : AbstractEditAction
	{
		Predicate<Breakpoint> predicate;
		
		public GotoPrevBreakpoint(Predicate<Breakpoint> predicate)
		{
			this.predicate = predicate;
		}
		
		public override void Execute(TextArea textArea)
		{
			Breakpoint mark = textArea.Document.BreakpointManager.GetPrevMark(textArea.Caret.Line, predicate);
			if (mark != null) {
				textArea.Caret.Position = mark.Location;
				textArea.SelectionManager.ClearSelection();
				textArea.SetDesiredColumn();
			}
		}
	}
	
	public class GotoNextBreakpoint : AbstractEditAction
	{
		Predicate<Breakpoint> predicate;
		
		public GotoNextBreakpoint(Predicate<Breakpoint> predicate)
		{
			this.predicate = predicate;
		}
		
		public override void Execute(TextArea textArea)
		{
			Breakpoint mark = textArea.Document.BreakpointManager.GetNextMark(textArea.Caret.Line, predicate);
			if (mark != null) {
				textArea.Caret.Position = mark.Location;
				textArea.SelectionManager.ClearSelection();
				textArea.SetDesiredColumn();
			}
		}
	}
	
	public class ClearAllBreakpoints : AbstractEditAction
	{
		Predicate<Breakpoint> predicate;
		
		public ClearAllBreakpoints(Predicate<Breakpoint> predicate)
		{
			this.predicate = predicate;
		}
		
		public override void Execute(TextArea textArea)
		{
			textArea.Document.BreakpointManager.RemoveMarks(predicate);
			textArea.Document.RequestUpdate(new TextAreaUpdate(TextAreaUpdateType.WholeTextArea));
			textArea.Document.CommitUpdate();
		}
	}
}
