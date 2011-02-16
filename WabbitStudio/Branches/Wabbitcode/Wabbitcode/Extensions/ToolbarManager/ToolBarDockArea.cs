using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.ToolbarManager
{
    public sealed class ToolBarDockArea : UserControl
    {
        private ToolBarManager _dockManager;
        private int _lastLineCount = 1;

        private Container components;

        public ToolBarDockArea(ToolBarManager dockManager, DockStyle dockStyle)
        {
            InitializeComponent();

            SetStyle(
                ControlStyles.AllPaintingInWmPaint |
                ControlStyles.DoubleBuffer, true);

            _dockManager = dockManager;
            DockManager.DockStation.Controls.Add(this);
            if (dockStyle == DockStyle.Fill || dockStyle == DockStyle.None)
                dockStyle = DockStyle.Top;

            Dock = dockStyle;
            SendToBack();

            FitHolders();

            Layout += LayoutHandler;
        }

        public ToolBarManager DockManager
        {
            get { return _dockManager; }
        }

        public bool Horizontal
        {
            get { return Dock != DockStyle.Left && Dock != DockStyle.Right; }
        }

        public void LayoutHandler(object sender, LayoutEventArgs e)
        {
            SuspendLayout();
            const int lineSzForCalc = 23;

            SortedList lineList = new SortedList();
            foreach (ToolBarDockHolder holder in Controls)
            {
                if (!holder.Visible)
                    continue;
                int prefLine = GetPreferredLine(lineSzForCalc, holder);
                int prefPos = GetPreferredPosition(holder);
                LineHolder line = (LineHolder) lineList[prefLine];
                if (line == null)
                {
                    line = new LineHolder();
                    lineList.Add(prefLine, line);
                }
                int csize = GetHolderWidth(holder);
                int lsize = GetHolderLineSize(holder);
                line.AddColumn(new ColumnHolder(prefPos, holder, csize + 1));
                if (line.Size - 1 < lsize)
                    line.Size = lsize + 1;
            }

            int pos = 0;
            _lastLineCount = lineList.Count;
            if (_lastLineCount == 0)
                _lastLineCount = 1;
            for (int ndx = 0; ndx < lineList.Count; ndx++)
            {
                LineHolder line = (LineHolder) lineList.GetByIndex(ndx);
                if (line == null)
                    continue;
                line.Distribute();
                foreach (ColumnHolder col in line.Columns)
                {
                    if (Horizontal)
                    {
                        col.Holder.Location = new Point(col.Position, pos);
                        col.Holder.PreferredDockedLocation = new Point(col.Holder.PreferredDockedLocation.X,
                                                                       pos + col.Holder.Height/2);
                    }
                    else
                    {
                        col.Holder.Location = new Point(pos, col.Position);
                        col.Holder.PreferredDockedLocation = new Point(pos + col.Holder.Width/2,
                                                                       col.Holder.PreferredDockedLocation.Y);
                    }
                }
                pos += line.Size + 1;
            }
            FitHolders();
            ResumeLayout();
        }

        private int GetPreferredLine(int lineSz, ToolBarDockHolder holder)
        {
            int pos;
            if (Horizontal)
            {
                pos = holder.PreferredDockedLocation.Y;
                if (pos < 0)
                    return Int32.MinValue;
                if (pos > Height)
                    return Int32.MaxValue;
            }
            else
            {
                pos = holder.PreferredDockedLocation.X;
                if (pos < 0)
                    return Int32.MinValue;
                if (pos > Width)
                    return Int32.MaxValue;
            }
            int line = pos/lineSz;
            int posLine = line*lineSz;
            if (posLine + 3 > pos)
                return line*2;
            if (posLine + lineSz - 3 < pos)
                return line*2 + 2;
            return line*2 + 1;
        }

        private int GetPreferredPosition(ToolBarDockHolder holder)
        {
            return Horizontal ? holder.PreferredDockedLocation.X : holder.PreferredDockedLocation.Y;
        }

        private int GetHolderLineSize(Control holder)
        {
            return Horizontal ? holder.Height : holder.Width;
        }

/*
        private int GetMyLineSize()
        {
            return Horizontal ? Height : Width;
        }
*/

        private int GetHolderWidth(Control holder)
        {
            return Horizontal ? holder.Width : holder.Height;
        }

        private void FitHolders()
        {
            Size sz = new Size(0, 0);
            foreach (Control c in Controls)
                if (c.Visible)
                {
                    if (c.Right > sz.Width)
                        sz.Width = c.Right;
                    if (c.Bottom > sz.Height)
                        sz.Height = c.Bottom;
                }
            if (Horizontal)
                Height = sz.Height;
            else
                Width = sz.Width;
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

        private void ToolBarDockArea_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                DockManager.ShowContextMenu(PointToScreen(new Point(e.X, e.Y)));
            }
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            // 
            // ToolBarDockArea
            // 
            this.Name = "ToolBarDockArea";
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.ToolBarDockArea_MouseUp);
        }

        #endregion

        #region Nested type: ColumnHolder

        private class ColumnHolder
        {
            public ToolBarDockHolder Holder;
            public int Position;
            public int Size;

            public ColumnHolder(int pos, ToolBarDockHolder holder, int size)
            {
                Position = pos;
                Holder = holder;
                Size = size;
            }
        }

        #endregion

        #region Nested type: LineHolder

        private class LineHolder
        {
            public ArrayList Columns = new ArrayList();
            public int Size;

            public void AddColumn(ColumnHolder column)
            {
                int indx = 0;
                foreach (ColumnHolder col in Columns)
                {
                    if (col.Position > column.Position)
                    {
                        Columns.Insert(indx, column);
                        break;
                    }
                    indx++;
                }
                if (indx == Columns.Count)
                    Columns.Add(column);
            }

            public void Distribute()
            {
                int pos = 0;
                foreach (ColumnHolder col in Columns)
                {
                    if (col.Position < pos)
                        col.Position = pos;
                    pos = col.Position + col.Size;
                }
            }
        }

        #endregion
    }
}