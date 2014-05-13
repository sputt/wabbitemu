// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 3176 $</version>
// </file>

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Reflection;
using System.Windows.Forms;

using Revsoft.TextEditor.Document;

namespace Revsoft.TextEditor
{
	/// <summary>
	/// This class views the line numbers and folding markers.
	/// </summary>
	public class IconBarMargin : AbstractMargin
	{
		const int iconBarWidth = 18;
		
		static readonly Size iconBarSize = new Size(iconBarWidth, -1);
		

		public override Size Size {
			get {
				return iconBarSize;
			}
		}
		
		public override bool IsVisible {
			get {
				return textArea.TextEditorProperties.IsIconBarVisible;
			}
		}
		
		
		public IconBarMargin(TextArea textArea) : base(textArea)
		{
		}
		
		public override void Paint(Graphics g, Rectangle rect)
		{
			if (rect.Width <= 0 || rect.Height <= 0) {
				return;
			}
			// paint background
			g.FillRectangle(SystemBrushes.Control, new Rectangle(drawingPosition.X, rect.Top, drawingPosition.Width - 1, rect.Height));
			g.DrawLine(SystemPens.ControlDark, drawingPosition.Right - 1, rect.Top, drawingPosition.Right - 1, rect.Bottom);
			
			// paint icons
			foreach (Bookmark mark in textArea.Document.BookmarkManager.Marks)
            {
				PaintAbstractMarginIcon(g, rect, mark);
			}

            foreach (Breakpoint mark in textArea.Document.BreakpointManager.Marks)
            {
                PaintAbstractMarginIcon(g, rect, mark);
            }

		    base.Paint(g, rect);
		}

	    private void PaintAbstractMarginIcon(Graphics g, Rectangle rect, AbstractMarginIcon mark)
	    {
	        int lineNumber = textArea.Document.GetVisibleLine(mark.LineNumber);
	        int lineHeight = textArea.TextView.FontHeight;
	        int yPos = lineNumber * lineHeight - textArea.VirtualTop.Y;
	        if (!IsLineInsideRegion(yPos, yPos + lineHeight, rect.Y, rect.Bottom))
	        {
	            return;
	        }
	        if (lineNumber == textArea.Document.GetVisibleLine(mark.LineNumber - 1))
	        {
	            // marker is inside folded region, do not draw it
	            return;
	        }
	        mark.Draw(this, g, new Point(0, yPos));
	    }

	    public override void HandleMouseDown(Point mousePos, MouseButtons mouseButtons)
		{
			int clickedVisibleLine = (mousePos.Y + textArea.VirtualTop.Y) / textArea.TextView.FontHeight;
			int lineNumber = textArea.Document.GetFirstLogicalLine(clickedVisibleLine);
			
			if (textArea.Caret.Line != lineNumber) {
				textArea.Caret.Line = lineNumber;
			}
			
			IList<Bookmark> marks = textArea.Document.BookmarkManager.Marks;
			List<Bookmark> marksInLine = new List<Bookmark>();
			int oldBookCount = marks.Count;
			foreach (Bookmark mark in marks) {
				if (mark.LineNumber == lineNumber) {
					marksInLine.Add(mark);
				}
			}
            IList<Breakpoint> breaks = textArea.Document.BreakpointManager.Marks;
            List<Breakpoint> breaksInLine = new List<Breakpoint>();
            int oldBreakCount = breaks.Count;
            foreach (Breakpoint mark in breaks)
            {
                if (mark.LineNumber == lineNumber) {
                    breaksInLine.Add(mark);
                }
            }
			for (int i = marksInLine.Count - 1; i >= 0; i--) {
				Bookmark mark = marksInLine[i];
			    if (!mark.Click(textArea, new MouseEventArgs(mouseButtons, 1, mousePos.X, mousePos.Y, 0))) continue;
			    if (oldBookCount != marks.Count) {
			        textArea.UpdateLine(lineNumber);
			    }
			    return;
			}
		    for (int i = breaksInLine.Count - 1; i >= 0; i--) {
				Breakpoint mark = breaksInLine[i];
                if (!mark.Click(textArea, new MouseEventArgs(mouseButtons, 1, mousePos.X, mousePos.Y, 0))) continue;
                if (oldBreakCount != marks.Count) {
                    textArea.UpdateLine(lineNumber);
                }
                return;
            }
                Breakpoint newBreak = new Breakpoint(textArea.Document, new TextLocation(0, lineNumber));
                newBreak.Click(textArea, new MouseEventArgs(mouseButtons, 1, mousePos.X, mousePos.Y, 0));
                textArea.UpdateLine(lineNumber);
			base.HandleMouseDown(mousePos, mouseButtons);
		}
		
		#region Drawing functions

	    public void DrawBreakpoint(Graphics g, int y, bool isEnabled)
		{
            
            int diameter = Math.Min(iconBarWidth - 2, textArea.TextView.FontHeight);
            Rectangle rect = new Rectangle(1,
                                           y + (textArea.TextView.FontHeight - diameter) / 2,
                                           diameter,
                                           diameter);
            var icon = Assembly.GetExecutingAssembly().GetManifestResourceStream("Revsoft.TextEditor.Resources.brk.bmp");
	        if (icon == null)
	        {
	            return;
	        }

	        Bitmap breakpoint = new Bitmap(icon);
	        breakpoint.MakeTransparent();
	        g.DrawImage(breakpoint, rect);
		}
		
		public void DrawBookmark(Graphics g, int y, bool isEnabled)
		{
			int delta = textArea.TextView.FontHeight / 8;
			Rectangle rect = new Rectangle(1, y + delta, drawingPosition.Width - 4, textArea.TextView.FontHeight - delta * 2);
			
			if (isEnabled) {
				using (Brush brush = new LinearGradientBrush(new Point(rect.Left, rect.Top),
				                                             new Point(rect.Right, rect.Bottom),
				                                             Color.SkyBlue,
				                                             Color.White)) {
					FillRoundRect(g, brush, rect);
				}
			} else {
				FillRoundRect(g, Brushes.White, rect);
			}
			using (Brush brush = new LinearGradientBrush(new Point(rect.Left, rect.Top),
			                                             new Point(rect.Right, rect.Bottom),
			                                             Color.SkyBlue,
			                                             Color.Blue)) {
				using (Pen pen = new Pen(brush)) {
					DrawRoundRect(g, pen, rect);
				}
			}
		}

		public void DrawArrow(Graphics g, int y)
		{
			int delta = textArea.TextView.FontHeight / 8;
			Rectangle rect = new Rectangle(1, y + delta, drawingPosition.Width - 4, textArea.TextView.FontHeight - delta * 2);
			using (Brush brush = new LinearGradientBrush(new Point(rect.Left, rect.Top),
			                                             new Point(rect.Right, rect.Bottom),
			                                             Color.LightYellow,
			                                             Color.Yellow)) {
				FillArrow(g, brush, rect);
			}
			
			using (Brush brush = new LinearGradientBrush(new Point(rect.Left, rect.Top),
			                                             new Point(rect.Right, rect.Bottom),
			                                             Color.Yellow,
			                                             Color.Brown)) {
				using (Pen pen = new Pen(brush)) {
					DrawArrow(g, pen, rect);
				}
			}
		}
		
		GraphicsPath CreateArrowGraphicsPath(Rectangle r)
		{
			GraphicsPath gp = new GraphicsPath();
			int halfX = r.Width / 2;
			int halfY = r.Height/ 2;
			gp.AddLine(r.X, r.Y + halfY/2, r.X + halfX, r.Y + halfY/2);
			gp.AddLine(r.X + halfX, r.Y + halfY/2, r.X + halfX, r.Y);
			gp.AddLine(r.X + halfX, r.Y, r.Right, r.Y + halfY);
			gp.AddLine(r.Right, r.Y + halfY, r.X + halfX, r.Bottom);
			gp.AddLine(r.X + halfX, r.Bottom, r.X + halfX, r.Bottom - halfY/2);
			gp.AddLine(r.X + halfX, r.Bottom - halfY/2, r.X, r.Bottom - halfY/2);
			gp.AddLine(r.X, r.Bottom - halfY/2, r.X, r.Y + halfY/2);
			gp.CloseFigure();
			return gp;
		}
		
		GraphicsPath CreateRoundRectGraphicsPath(Rectangle r)
		{
			GraphicsPath gp = new GraphicsPath();
			int radius = r.Width / 2;
			gp.AddLine(r.X + radius, r.Y, r.Right - radius, r.Y);
			gp.AddArc(r.Right - radius, r.Y, radius, radius, 270, 90);
			
			gp.AddLine(r.Right, r.Y + radius, r.Right, r.Bottom - radius);
			gp.AddArc(r.Right - radius, r.Bottom - radius, radius, radius, 0, 90);
			
			gp.AddLine(r.Right - radius, r.Bottom, r.X + radius, r.Bottom);
			gp.AddArc(r.X, r.Bottom - radius, radius, radius, 90, 90);
			
			gp.AddLine(r.X, r.Bottom - radius, r.X, r.Y + radius);
			gp.AddArc(r.X, r.Y, radius, radius, 180, 90);
			
			gp.CloseFigure();
			return gp;
		}
		
		void DrawRoundRect(Graphics g, Pen p , Rectangle r)
		{
			using (GraphicsPath gp = CreateRoundRectGraphicsPath(r)) {
				g.DrawPath(p, gp);
			}
		}
		
		void FillRoundRect(Graphics g, Brush b , Rectangle r)
		{
			using (GraphicsPath gp = CreateRoundRectGraphicsPath(r)) {
				g.FillPath(b, gp);
			}
		}

		void DrawArrow(Graphics g, Pen p , Rectangle r)
		{
			using (GraphicsPath gp = CreateArrowGraphicsPath(r)) {
				g.DrawPath(p, gp);
			}
		}
		
		void FillArrow(Graphics g, Brush b , Rectangle r)
		{
			using (GraphicsPath gp = CreateArrowGraphicsPath(r)) {
				g.FillPath(b, gp);
			}
		}

		#endregion
		
		static bool IsLineInsideRegion(int top, int bottom, int regionTop, int regionBottom)
		{
			if (top >= regionTop && top <= regionBottom) {
				// Region overlaps the line's top edge.
				return true;
			}
		    return regionTop > top && regionTop < bottom;
		}
	}
}
