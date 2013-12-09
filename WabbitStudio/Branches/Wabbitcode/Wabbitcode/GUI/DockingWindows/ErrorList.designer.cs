namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class ErrorList
    {
        private System.Windows.Forms.DataGridViewImageColumn CategoryColumn;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.ContextMenu contextMenu1;
        private System.Windows.Forms.MenuItem copyMenuItem;
        private System.Windows.Forms.DataGridViewTextBoxColumn DescriptionColumn;
        private System.Windows.Forms.DataGridView errorGridView;
        private System.Windows.Forms.ToolStrip errorOptionsBar;
        private System.Windows.Forms.ToolStripButton errorToolButton;
        private System.Windows.Forms.DataGridViewTextBoxColumn FileColumn;
        private System.Windows.Forms.MenuItem fixMenuItem;
        private System.Windows.Forms.MenuItem gotoMenuItem;
        private System.Windows.Forms.MenuItem helpMenuItem;
        private System.Windows.Forms.ImageList imageListIcons;
        private System.Windows.Forms.DataGridViewTextBoxColumn LineColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn NumberColumn;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton warnToolButton;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }

            base.Dispose(disposing);
        }

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ErrorList));
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.contextMenu1 = new System.Windows.Forms.ContextMenu();
            this.helpMenuItem = new System.Windows.Forms.MenuItem();
            this.gotoMenuItem = new System.Windows.Forms.MenuItem();
            this.fixMenuItem = new System.Windows.Forms.MenuItem();
            this.copyMenuItem = new System.Windows.Forms.MenuItem();
            this.imageListIcons = new System.Windows.Forms.ImageList(this.components);
            this.errorGridView = new System.Windows.Forms.DataGridView();
            this.CategoryColumn = new System.Windows.Forms.DataGridViewImageColumn();
            this.NumberColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DescriptionColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.FileColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.LineColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.errorOptionsBar = new System.Windows.Forms.ToolStrip();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.errorToolButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.warnToolButton = new System.Windows.Forms.ToolStripButton();
            ((System.ComponentModel.ISupportInitialize)this.errorGridView).BeginInit();
            this.errorOptionsBar.SuspendLayout();
            this.SuspendLayout();

            // contextMenu1

            this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
            {
                this.helpMenuItem,
                this.gotoMenuItem,
                this.fixMenuItem,
                this.copyMenuItem
            });

            // helpMenuItem

            this.helpMenuItem.Index = 0;
            this.helpMenuItem.Text = "Show Error Help";

            // gotoMenuItem

            this.gotoMenuItem.Index = 1;
            this.gotoMenuItem.Text = "Goto Error";
            this.gotoMenuItem.Click += new System.EventHandler(this.gotoMenuItem_Click);

            // fixMenuItem

            this.fixMenuItem.Index = 2;
            this.fixMenuItem.Text = "Fix";
            this.fixMenuItem.Click += new System.EventHandler(this.fixMenuItem_Click);

            // copyMenuItem

            this.copyMenuItem.Index = 3;
            this.copyMenuItem.Text = "Copy";
            this.copyMenuItem.Click += new System.EventHandler(this.Copy);

            // imageListIcons

            this.imageListIcons.ImageStream = (System.Windows.Forms.ImageListStreamer)resources.GetObject("imageListIcons.ImageStream");
            this.imageListIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.imageListIcons.Images.SetKeyName(0, "stop_16.png");
            this.imageListIcons.Images.SetKeyName(1, "Warning16.png");

            // errorGridView

            this.errorGridView.AllowUserToAddRows = false;
            this.errorGridView.AllowUserToDeleteRows = false;
            this.errorGridView.AllowUserToResizeRows = false;
            this.errorGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.errorGridView.BackgroundColor = System.Drawing.SystemColors.Window;
            this.errorGridView.ClipboardCopyMode = System.Windows.Forms.DataGridViewClipboardCopyMode.EnableWithoutHeaderText;
            this.errorGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.errorGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[]
            {
                this.CategoryColumn,
                this.NumberColumn,
                this.DescriptionColumn,
                this.FileColumn,
                this.LineColumn
            });
            this.errorGridView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.errorGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.errorGridView.Location = new System.Drawing.Point(0, 27);
            this.errorGridView.Name = "errorGridView";
            this.errorGridView.RowHeadersVisible = false;
            this.errorGridView.RowTemplate.Height = 20;
            this.errorGridView.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.errorGridView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.errorGridView.Size = new System.Drawing.Size(424, 203);
            this.errorGridView.TabIndex = 2;
            this.errorGridView.CellMouseDoubleClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.errorGridView_CellMouseDoubleClick);
            this.errorGridView.MouseClick += new System.Windows.Forms.MouseEventHandler(this.errorGridView_MouseClick);

            // CategoryColumn

            this.CategoryColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.ColumnHeader;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.NullValue = "System.Drawing.Bitmap";
            this.CategoryColumn.DefaultCellStyle = dataGridViewCellStyle1;
            this.CategoryColumn.FillWeight = 15F;
            this.CategoryColumn.HeaderText = "";
            this.CategoryColumn.Image = (System.Drawing.Image)resources.GetObject("CategoryColumn.Image");
            this.CategoryColumn.Name = "CategoryColumn";
            this.CategoryColumn.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.CategoryColumn.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
            this.CategoryColumn.ToolTipText = "Category";
            this.CategoryColumn.Width = 19;

            // NumberColumn

            this.NumberColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.NumberColumn.FillWeight = 25F;
            this.NumberColumn.HeaderText = "";
            this.NumberColumn.Name = "NumberColumn";
            this.NumberColumn.Resizable = System.Windows.Forms.DataGridViewTriState.True;
            this.NumberColumn.ToolTipText = "Number";
            this.NumberColumn.Width = 20;

            // DescriptionColumn

            this.DescriptionColumn.FillWeight = 220.5529F;
            this.DescriptionColumn.HeaderText = "Description";
            this.DescriptionColumn.Name = "DescriptionColumn";

            // FileColumn

            this.FileColumn.HeaderText = "File";
            this.FileColumn.Name = "FileColumn";

            // LineColumn

            this.LineColumn.HeaderText = "Line";
            this.LineColumn.Name = "LineColumn";

            // errorOptionsBar

            this.errorOptionsBar.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.errorOptionsBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[]
            {
                this.toolStripSeparator2,
                this.errorToolButton,
                this.toolStripSeparator1,
                this.warnToolButton
            });
            this.errorOptionsBar.Location = new System.Drawing.Point(0, 2);
            this.errorOptionsBar.Name = "errorOptionsBar";
            this.errorOptionsBar.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.errorOptionsBar.Size = new System.Drawing.Size(424, 25);
            this.errorOptionsBar.TabIndex = 1;

            // toolStripSeparator2

            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);

            // errorToolButton

            this.errorToolButton.Checked = true;
            this.errorToolButton.CheckOnClick = true;
            this.errorToolButton.CheckState = System.Windows.Forms.CheckState.Checked;
            this.errorToolButton.Image = (System.Drawing.Image)resources.GetObject("errorToolButton.Image");
            this.errorToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.errorToolButton.Name = "errorToolButton";
            this.errorToolButton.Size = new System.Drawing.Size(57, 22);
            this.errorToolButton.Text = "Errors";
            this.errorToolButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.errorToolButton.CheckedChanged += new System.EventHandler(this.errorToolButton_CheckedChanged);

            // toolStripSeparator1

            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);

            // warnToolButton

            this.warnToolButton.Checked = true;
            this.warnToolButton.CheckOnClick = true;
            this.warnToolButton.CheckState = System.Windows.Forms.CheckState.Checked;
            this.warnToolButton.Image = (System.Drawing.Image)resources.GetObject("warnToolButton.Image");
            this.warnToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.warnToolButton.Name = "warnToolButton";
            this.warnToolButton.Size = new System.Drawing.Size(77, 22);
            this.warnToolButton.Text = "Warnings";
            this.warnToolButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.warnToolButton.CheckedChanged += new System.EventHandler(this.warnToolButton_CheckedChanged);

            // ErrorList

            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.ClientSize = new System.Drawing.Size(424, 232);
            this.Controls.Add(this.errorGridView);
            this.Controls.Add(this.errorOptionsBar);
            this.Icon = (System.Drawing.Icon)resources.GetObject("$this.Icon");
            this.Name = "ErrorList";
            this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottomAutoHide;
            this.TabText = "Error List";
            this.Text = "Error List";
            ((System.ComponentModel.ISupportInitialize)this.errorGridView).EndInit();
            this.errorOptionsBar.ResumeLayout(false);
            this.errorOptionsBar.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();
        }
    }
}