// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 3515 $</version>
// </file>

using System;
using System.Drawing;
using System.Windows.Forms;

using Revsoft.TextEditor.Util;

namespace Revsoft.TextEditor.Gui.CompletionWindow
{
	public interface IDeclarationViewWindow
	{
		string Description {
			get;
			set;
		}
		void ShowDeclarationViewWindow();
		void CloseDeclarationViewWindow();
	}
	
	public class DeclarationViewWindow : Form, IDeclarationViewWindow
	{
		string description = String.Empty;
		bool fixedWidth;
		
		public string Description {
			get {
				return description;
			}
			set {
				description = value;
				if (value == null && Visible) {
					Visible = false;
				} else if (value != null) {
					if (!Visible) ShowDeclarationViewWindow();
					Refresh();
				}
			}
		}
		
		public bool FixedWidth {
			get {
				return fixedWidth;
			}
			set {
				fixedWidth = value;
			}
		}
		
		public int GetRequiredLeftHandSideWidth(Point p) {
			if (description != null && description.Length > 0) {
				using (Graphics g = CreateGraphics()) {
					Size s = TipPainterTools.GetLeftHandSideDrawingSizeHelpTipFromCombinedDescription(this, g, Font, null, description, p);
					return s.Width;
				}
			}
			return 0;
		}
		
		public bool HideOnClick;
		
		public DeclarationViewWindow(Form parent)
		{
			SetStyle(ControlStyles.Selectable, false);
			StartPosition   = FormStartPosition.Manual;
			FormBorderStyle = FormBorderStyle.None;
			Owner           = parent;
			ShowInTaskbar   = false;
			Size            = new Size(0, 0);
			base.CreateHandle();
		}
		
		protected override CreateParams CreateParams {
			get {
				CreateParams p = base.CreateParams;
				//AbstractCompletionWindow.AddShadowToWindow(p);
				return p;
			}
		}
		
		protected override bool ShowWithoutActivation {
			get {
				return true;
			}
		}
		
		protected override void OnClick(EventArgs e)
		{
			base.OnClick(e);
			if (HideOnClick) Hide();
		}
		
		public void ShowDeclarationViewWindow()
		{
			Show();
		}
		
		public void CloseDeclarationViewWindow()
		{
			Close();
			Dispose();
		}
        ToolTip newToolTip;
		protected override void OnPaint(PaintEventArgs pe)
		{
            if (newToolTip != null)
                newToolTip.RemoveAll();
			if (description != null && description.Length > 0) {
				if (fixedWidth) {
                    newToolTip = new ToolTip();
                    newToolTip.SetToolTip(this, description);
                    newToolTip.Show(description, this, pe.ClipRectangle.X, pe.ClipRectangle.Y);
                    //TipPainterTools.DrawFixedWidthHelpTipFromCombinedDescription(this, pe.Graphics, Font, null, description);
				} else {
                    newToolTip = new ToolTip();
                    newToolTip.SetToolTip(this, description);
                    newToolTip.Show(description, this, pe.ClipRectangle.X, pe.ClipRectangle.Y);
					//TipPainterTools.DrawHelpTipFromCombinedDescription(this, pe.Graphics, Font, null, description);
				}
			}
		}
		
		protected override void OnPaintBackground(PaintEventArgs pe)
		{
			//pe.Graphics.FillRectangle(SystemBrushes.Info, pe.ClipRectangle);
		}
	}
}
