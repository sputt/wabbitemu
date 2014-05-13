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
	/// Description of Bookmark.
	/// </summary>
	public class Bookmark : AbstractMarginIcon
	{
        public Bookmark(IDocument document, TextLocation location, bool isEnabled = true) 
            : base(document, location, isEnabled)
        {
            this.document = document;
            this.isEnabled = isEnabled;
            Location = location;
        }

	    protected override void AnchorDeleted(object sender, EventArgs e)
		{
			document.BookmarkManager.RemoveMark(this);
		}
		
		public override bool Click(Control parent, MouseEventArgs e)
		{
		    if (e.Button != MouseButtons.Left || !CanToggle)
		    {
		        return false;
		    }

		    document.BookmarkManager.RemoveMark(this);
		    return true;
		}
		
		public override void Draw(IconBarMargin margin, Graphics g, Point p)
		{
			margin.DrawBookmark(g, p.Y, isEnabled);
		}
	}
}
