// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 915 $</version>
// </file>

using System;

namespace Revsoft.TextEditor.Document
{
	public delegate void BreakpointEventHandler(object sender, BreakpointEventArgs e);
	
	/// <summary>
	/// Description of BookmarkEventHandler.
	/// </summary>
	public class BreakpointEventArgs : EventArgs
	{
	    readonly Breakpoint breakpoint;
		
		public Breakpoint Breakpoint {
			get {
				return breakpoint;
			}
		}
		
		public BreakpointEventArgs(Breakpoint breakpoint)
		{
			this.breakpoint = breakpoint;
		}
	}
}
