using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.ToolbarManager
{
    public class ToolBarDockHolder : UserControl
    {
        // Added by mav
        private static int _mininumStrSize;
        private AllowedBorders _allowedBorders = AllowedBorders.All;

        private Control _control;
        private ToolBarManager _dockManager;
        private Form _form = new Form();
        private Panel _panel;
        private ToolBarDockArea _preferredDockedArea;

        private Point _preferredDockedLocation = new Point(0, 0);
        private DockStyle _style = DockStyle.Top;
        private string _toolbarTitle = string.Empty;
        private Container components;

        public ToolBarDockHolder(ToolBarManager dm, Control c, DockStyle style)
        {
            InitializeComponent();
            SetStyle(
                ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint |
                ControlStyles.DoubleBuffer, true);

            _panel.Controls.AddRange(new[] {c});
            DockManager = dm;
            switch (style)
            {
                case DockStyle.Left:
                    _preferredDockedArea = dm.Left;
                    break;
                case DockStyle.Right:
                    _preferredDockedArea = dm.Right;
                    break;
                case DockStyle.Bottom:
                    _preferredDockedArea = dm.Bottom;
                    break;
                default:
                    _preferredDockedArea = dm.Top;
                    break;
            }
            _control = c;
            FloatForm.Visible = false;
            FloatForm.FormBorderStyle = FormBorderStyle.None;
            FloatForm.MaximizeBox = false;
            FloatForm.MinimizeBox = false;
            FloatForm.ShowInTaskbar = false;
            FloatForm.ClientSize = new Size(10, 10);
            // Added by mav
            DockManager.MainForm.AddOwnedForm(FloatForm);
            DockStyle = style;
            ToolbarTitle = c.Text;
//			_control.TextChanged += new EventHandler(control_TextChanged);
        }

        public AllowedBorders AllowedBorders
        {
            get { return _allowedBorders; }
            set { _allowedBorders = value; }
        }

        public Control Control
        {
            get { return _control; }
        }

        public Point PreferredDockedLocation
        {
            get { return _preferredDockedLocation; }
            set { _preferredDockedLocation = value; }
        }

        public ToolBarDockArea PreferredDockedArea
        {
            get { return _preferredDockedArea; }
            set { _preferredDockedArea = value; }
        }

        public Form FloatForm
        {
            get { return _form; }
        }

        // Added by mav

        public string ToolbarTitle
        {
            get { return _toolbarTitle; }
            set
            {
                if (_toolbarTitle != value)
                {
                    _toolbarTitle = value;
                    TitleTextChanged();
                }
            }
        }

        public DockStyle DockStyle
        {
            get { return _style; }
            set
            {
                _style = value;
                Create();
            }
        }

        public ToolBarManager DockManager
        {
            get { return _dockManager; }
            set { _dockManager = value; }
        }

        public bool IsAllowed(DockStyle dock)
        {
            switch (dock)
            {
                case DockStyle.Fill:
                    return false;
                case DockStyle.Top:
                    return (AllowedBorders & AllowedBorders.Top) == AllowedBorders.Top;
                case DockStyle.Left:
                    return (AllowedBorders & AllowedBorders.Left) == AllowedBorders.Left;
                case DockStyle.Bottom:
                    return (AllowedBorders & AllowedBorders.Bottom) == AllowedBorders.Bottom;
                case DockStyle.Right:
                    return (AllowedBorders & AllowedBorders.Right) == AllowedBorders.Right;
                case DockStyle.None:
                    return true;
            }
            return false;
        }

        // Added by mav
//		private void control_TextChanged(object sender, EventArgs e)
//		{
//			ToolbarTitle = _control.Text;
//		}

        private void TitleTextChanged()
        {
            if (FloatForm.Visible)
                Invalidate(false);
        }

        private void Create()
        {
            Control c = _control;

            Size sz;
            if (typeof (ToolBar).IsAssignableFrom(c.GetType()))
            {
                ToolBar tb = (ToolBar) c;
                int w = 0;
                int h = 0;
                if (DockStyle != DockStyle.Right && DockStyle != DockStyle.Left)
                {
                    c.Dock = DockStyle.Top;
                    foreach (ToolBarButton but in tb.Buttons)
                        w += but.Rectangle.Width;
                    h = tb.ButtonSize.Height;
                    sz = new Size(w, h);
                }
                else
                {
                    c.Dock = DockStyle.Left;
                    foreach (ToolBarButton but in tb.Buttons)
                        // Added by mav
                        if (but.Style == ToolBarButtonStyle.Separator)
                            h += 2*but.Rectangle.Width;
                        else
                            h += but.Rectangle.Height;
                    w = tb.ButtonSize.Width + 2;
                    sz = new Size(w, h);
                }
            }
            else
            {
                sz = c.Size;
                c.Dock = DockStyle.Fill;
            }

            DockPadding.All = 0;
            if (DockStyle == DockStyle.None)
            {
                DockPadding.Left = 2;
                DockPadding.Bottom = 2;
                DockPadding.Right = 2;
                DockPadding.Top = 15;
                sz = new Size(sz.Width + 4, sz.Height + 18);
            }
            else if (DockStyle != DockStyle.Right && DockStyle != DockStyle.Left)
            {
                DockPadding.Left = 8;
                sz = new Size(sz.Width + 8, sz.Height);
            }
            else
            {
                DockPadding.Top = 8;
                sz = new Size(sz.Width, sz.Height + 8);
            }
            Size = sz;
        }


        public bool CanDrag(Point p)
        {
            if (DockStyle == DockStyle.None)
            {
                return p.Y < 16 && p.X < Width - 16;
            }
            if (DockStyle != DockStyle.Right && DockStyle != DockStyle.Left)
                return p.X < 8 && ClientRectangle.Contains(p);
            return p.Y < 8 && ClientRectangle.Contains(p);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        private void ToolBarDockHolder_Paint(object sender, PaintEventArgs e)
        {
            if (DockStyle == DockStyle.None)
            {
                e.Graphics.FillRectangle(SystemBrushes.ControlDark, ClientRectangle);
                // Added by mav
                DrawString(e.Graphics, ToolbarTitle, new Rectangle(0, 0, Width - 16, 14), SystemBrushes.ControlText);
                Rectangle closeRect = new Rectangle(Width - 15, 2, 10, 10);
                Pen pen = new Pen(SystemColors.ControlText);
                DrawCloseButton(e.Graphics, closeRect, pen);
                if (closeRect.Contains(PointToClient(MousePosition)))
                    e.Graphics.DrawRectangle(pen, closeRect);
                Rectangle r = ClientRectangle;
                r.Width--;
                r.Height--;
                e.Graphics.DrawRectangle(new Pen(SystemColors.ControlDarkDark), r);
            }
            else
            {
                e.Graphics.FillRectangle(SystemBrushes.ControlLight, ClientRectangle);
                int off = 2;
                Pen pen = new Pen(SystemColors.ControlDark);
                if (DockStyle != DockStyle.Right && DockStyle != DockStyle.Left)
                {
                    for (int i = 3; i < Size.Height - 3; i += off)
                        e.Graphics.DrawLine(pen, new Point(off, i), new Point(off + off, i));
                }
                else
                {
                    for (int i = 3; i < Size.Width - 3; i += off)
                        e.Graphics.DrawLine(pen, new Point(i, off), new Point(i, off + off));
                }
            }
        }

        private void ToolBarDockHolder_MouseEnter(object sender, EventArgs e)
        {
            if (DockStyle != DockStyle.None && CanDrag(PointToClient(MousePosition)))
                Cursor = Cursors.SizeAll;
            else
                Cursor = Cursors.Default;
            Invalidate(false);
        }

        private void ToolBarDockHolder_MouseLeave(object sender, EventArgs e)
        {
            Cursor = Cursors.Default;
            Invalidate(false);
        }

        private void ToolBarDockHolder_MouseMove(object sender, MouseEventArgs e)
        {
            if (DockStyle != DockStyle.None && CanDrag(new Point(e.X, e.Y)))
                Cursor = Cursors.SizeAll;
            else
                Cursor = Cursors.Default;
            Invalidate(false);
        }

        private void ToolBarDockHolder_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right && CanDrag(new Point(e.X, e.Y)))
            {
                DockManager.ShowContextMenu(PointToScreen(new Point(e.X, e.Y)));
            }
            // Floating Form Close Button Clicked
            if (e.Button == MouseButtons.Left
                && DockStyle == DockStyle.None
                && e.Y < 16 && e.X > Width - 16)
            {
                FloatForm.Visible = false;
            }
        }

        private void DrawString(Graphics g, string s, Rectangle area, Brush brush)
        {
            if (_mininumStrSize == 0)
            {
                _mininumStrSize = (int) g.MeasureString("....", Font).Width;
            }
            if (area.Width < _mininumStrSize)
                return;
            StringFormat drawFormat = new StringFormat();
            drawFormat.FormatFlags = StringFormatFlags.NoWrap | StringFormatFlags.FitBlackBox;
            drawFormat.Trimming = StringTrimming.EllipsisCharacter;
            SizeF ss = g.MeasureString(s, Font);
            if (ss.Height < area.Height)
            {
                int offset = (int) (area.Height - ss.Height)/2;
                area.Y += offset;
                area.Height -= offset;
            }
            g.DrawString(s, Font, brush, area, drawFormat);
        }

        private void DrawCloseButton(Graphics g, Rectangle cross, Pen pen)
        {
            cross.Inflate(-2, -2);

            g.DrawLine(pen, cross.X, cross.Y, cross.Right, cross.Bottom);
            g.DrawLine(pen, cross.X + 1, cross.Y, cross.Right, cross.Bottom - 1);
            g.DrawLine(pen, cross.X, cross.Y + 1, cross.Right - 1, cross.Bottom);
            g.DrawLine(pen, cross.Right, cross.Y, cross.Left, cross.Bottom);
            g.DrawLine(pen, cross.Right - 1, cross.Y, cross.Left, cross.Bottom - 1);
            g.DrawLine(pen, cross.Right, cross.Y + 1, cross.Left + 1, cross.Bottom);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this._panel = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // _panel
            // 
            this._panel.Dock = System.Windows.Forms.DockStyle.Fill;
            this._panel.Name = "_panel";
            this._panel.Size = new System.Drawing.Size(384, 25);
            this._panel.TabIndex = 0;
            // 
            // ToolBarDockHolder
            // 
            this.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Controls.AddRange(new System.Windows.Forms.Control[]
                                       {
                                           this._panel
                                       });
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 6.75F, System.Drawing.FontStyle.Regular,
                                                System.Drawing.GraphicsUnit.Point, ((System.Byte) (0)));
            this.Name = "ToolBarDockHolder";
            this.Size = new System.Drawing.Size(384, 25);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.ToolBarDockHolder_MouseUp);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.ToolBarDockHolder_Paint);
            this.MouseEnter += new System.EventHandler(this.ToolBarDockHolder_MouseEnter);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ToolBarDockHolder_MouseMove);
            this.MouseLeave += new System.EventHandler(this.ToolBarDockHolder_MouseLeave);
            this.ResumeLayout(false);
        }

        #endregion
    }
}