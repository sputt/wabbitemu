// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 3272 $</version>
// </file>

using System;
using System.Drawing;
using System.Windows.Forms;

namespace Revsoft.TextEditor.Document
{
    /// <summary>
	/// Description of Breakpoint.
	/// </summary>
	public class Breakpoint : AbstractMarginIcon
    {
        public Breakpoint(IDocument document, TextLocation location, bool isEnabled = true)
            : base(document, location, isEnabled)
        {
        }

        protected override void AnchorDeleted(object sender, EventArgs e)
		{
			document.BreakpointManager.RemoveMark(this);
		}

        public override void Draw(IconBarMargin margin, Graphics g, Point p)
        {
            margin.DrawBreakpoint(g, p.Y, isEnabled);
        }

        public override bool Click(Control parent, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left || !CanToggle)
            {
                return false;
            }

            Actions.IEditAction newBreakpoint = new Actions.ToggleBreakpoint();
            newBreakpoint.Execute((TextArea)parent);
            return true;
        }
    }
}
