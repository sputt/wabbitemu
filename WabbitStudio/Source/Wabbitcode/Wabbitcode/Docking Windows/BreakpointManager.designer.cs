namespace Revsoft.Wabbitcode.Docking_Windows
{
    partial class BreakpointManager
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BreakpointManager));
			this.contextMenu = new System.Windows.Forms.ContextMenu();
			this.autoHideItem = new System.Windows.Forms.MenuItem();
			this.floatingItem = new System.Windows.Forms.MenuItem();
			this.hideItem = new System.Windows.Forms.MenuItem();
			this.toolStrip1 = new System.Windows.Forms.ToolStrip();
			this.newBreakToolStripButton = new System.Windows.Forms.ToolStripSplitButton();
			this.breakpointToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.dataBreakpointToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
			this.delBreakToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.delAllBreakToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.disableAllToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
			this.gotoToolStripButton = new System.Windows.Forms.ToolStripButton();
			this.colToolStripDropDown = new System.Windows.Forms.ToolStripDropDownButton();
			this.enabledToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.nameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.conditionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.hitCountToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.addressToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.pageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.isInRamToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.breakpointGridView = new System.Windows.Forms.DataGridView();
			this.EnabledCol = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.NameCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.ConditionCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.HitCountCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.AddressCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.PageCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.IsRamCol = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.toolStrip1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.breakpointGridView)).BeginInit();
			this.SuspendLayout();
			// 
			// autoHideItem
			// 
			this.autoHideItem.Index = -1;
			this.autoHideItem.Text = "Autohide";
			// 
			// floatingItem
			// 
			this.floatingItem.Index = -1;
			this.floatingItem.Text = "Floating";
			// 
			// hideItem
			// 
			this.hideItem.Index = -1;
			this.hideItem.Text = "Hide";
			// 
			// toolStrip1
			// 
			this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
			this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newBreakToolStripButton,
            this.toolStripSeparator1,
            this.delBreakToolStripButton,
            this.delAllBreakToolStripButton,
            this.disableAllToolStripButton,
            this.toolStripSeparator2,
            this.gotoToolStripButton,
            this.colToolStripDropDown});
			this.toolStrip1.Location = new System.Drawing.Point(0, 0);
			this.toolStrip1.Name = "toolStrip1";
			this.toolStrip1.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
			this.toolStrip1.Size = new System.Drawing.Size(535, 25);
			this.toolStrip1.TabIndex = 0;
			this.toolStrip1.Text = "toolStrip1";
			// 
			// newBreakToolStripButton
			// 
			this.newBreakToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.newBreakToolStripButton.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.breakpointToolStripMenuItem,
            this.dataBreakpointToolStripMenuItem});
			this.newBreakToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("newBreakToolStripButton.Image")));
			this.newBreakToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.newBreakToolStripButton.Name = "newBreakToolStripButton";
			this.newBreakToolStripButton.Size = new System.Drawing.Size(47, 22);
			this.newBreakToolStripButton.Text = "New";
			this.newBreakToolStripButton.ButtonClick += new System.EventHandler(this.breakpointToolStripMenuItem_Click);
			// 
			// breakpointToolStripMenuItem
			// 
			this.breakpointToolStripMenuItem.Name = "breakpointToolStripMenuItem";
			this.breakpointToolStripMenuItem.Size = new System.Drawing.Size(158, 22);
			this.breakpointToolStripMenuItem.Text = "Break at Line...";
			this.breakpointToolStripMenuItem.Click += new System.EventHandler(this.breakpointToolStripMenuItem_Click);
			// 
			// dataBreakpointToolStripMenuItem
			// 
			this.dataBreakpointToolStripMenuItem.Name = "dataBreakpointToolStripMenuItem";
			this.dataBreakpointToolStripMenuItem.Size = new System.Drawing.Size(158, 22);
			this.dataBreakpointToolStripMenuItem.Text = "Data Breakpoint";
			// 
			// toolStripSeparator1
			// 
			this.toolStripSeparator1.Name = "toolStripSeparator1";
			this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
			// 
			// delBreakToolStripButton
			// 
			this.delBreakToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.delBreakToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.delBreakToolStripButton.Name = "delBreakToolStripButton";
			this.delBreakToolStripButton.Size = new System.Drawing.Size(44, 22);
			this.delBreakToolStripButton.Text = "Delete";
			this.delBreakToolStripButton.Click += new System.EventHandler(this.delBreakToolStripButton_Click);
			// 
			// delAllBreakToolStripButton
			// 
			this.delAllBreakToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.delAllBreakToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("delAllBreakToolStripButton.Image")));
			this.delAllBreakToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.delAllBreakToolStripButton.Name = "delAllBreakToolStripButton";
			this.delAllBreakToolStripButton.Size = new System.Drawing.Size(61, 22);
			this.delAllBreakToolStripButton.Text = "Delete All";
			this.delAllBreakToolStripButton.Click += new System.EventHandler(this.delAllBreakToolStripButton_Click);
			// 
			// disableAllToolStripButton
			// 
			this.disableAllToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.disableAllToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("disableAllToolStripButton.Image")));
			this.disableAllToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.disableAllToolStripButton.Name = "disableAllToolStripButton";
			this.disableAllToolStripButton.Size = new System.Drawing.Size(66, 22);
			this.disableAllToolStripButton.Text = "Disable All";
			this.disableAllToolStripButton.Click += new System.EventHandler(this.disableAllToolStripButton_Click);
			// 
			// toolStripSeparator2
			// 
			this.toolStripSeparator2.Name = "toolStripSeparator2";
			this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
			// 
			// gotoToolStripButton
			// 
			this.gotoToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.gotoToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("gotoToolStripButton.Image")));
			this.gotoToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.gotoToolStripButton.Name = "gotoToolStripButton";
			this.gotoToolStripButton.Size = new System.Drawing.Size(37, 22);
			this.gotoToolStripButton.Text = "Goto";
			this.gotoToolStripButton.Click += new System.EventHandler(this.gotoToolStripButton_Click);
			// 
			// colToolStripDropDown
			// 
			this.colToolStripDropDown.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
			this.colToolStripDropDown.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.enabledToolStripMenuItem,
            this.nameToolStripMenuItem,
            this.conditionToolStripMenuItem,
            this.hitCountToolStripMenuItem,
            this.addressToolStripMenuItem,
            this.pageToolStripMenuItem,
            this.isInRamToolStripMenuItem});
			this.colToolStripDropDown.Image = ((System.Drawing.Image)(resources.GetObject("colToolStripDropDown.Image")));
			this.colToolStripDropDown.ImageTransparentColor = System.Drawing.Color.Magenta;
			this.colToolStripDropDown.Name = "colToolStripDropDown";
			this.colToolStripDropDown.Size = new System.Drawing.Size(68, 22);
			this.colToolStripDropDown.Text = "Columns";
			// 
			// enabledToolStripMenuItem
			// 
			this.enabledToolStripMenuItem.Checked = true;
			this.enabledToolStripMenuItem.CheckOnClick = true;
			this.enabledToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.enabledToolStripMenuItem.Name = "enabledToolStripMenuItem";
			this.enabledToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
			this.enabledToolStripMenuItem.Text = "Enabled";
			this.enabledToolStripMenuItem.Click += new System.EventHandler(this.colButtonClick);
			// 
			// nameToolStripMenuItem
			// 
			this.nameToolStripMenuItem.Checked = true;
			this.nameToolStripMenuItem.CheckOnClick = true;
			this.nameToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.nameToolStripMenuItem.Name = "nameToolStripMenuItem";
			this.nameToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
			this.nameToolStripMenuItem.Text = "Name";
			this.nameToolStripMenuItem.Click += new System.EventHandler(this.colButtonClick);
			// 
			// conditionToolStripMenuItem
			// 
			this.conditionToolStripMenuItem.Checked = true;
			this.conditionToolStripMenuItem.CheckOnClick = true;
			this.conditionToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.conditionToolStripMenuItem.Name = "conditionToolStripMenuItem";
			this.conditionToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
			this.conditionToolStripMenuItem.Text = "Condition";
			this.conditionToolStripMenuItem.Click += new System.EventHandler(this.colButtonClick);
			// 
			// hitCountToolStripMenuItem
			// 
			this.hitCountToolStripMenuItem.Checked = true;
			this.hitCountToolStripMenuItem.CheckOnClick = true;
			this.hitCountToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.hitCountToolStripMenuItem.Name = "hitCountToolStripMenuItem";
			this.hitCountToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
			this.hitCountToolStripMenuItem.Text = "Hit Count";
			this.hitCountToolStripMenuItem.Click += new System.EventHandler(this.colButtonClick);
			// 
			// addressToolStripMenuItem
			// 
			this.addressToolStripMenuItem.Checked = true;
			this.addressToolStripMenuItem.CheckOnClick = true;
			this.addressToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.addressToolStripMenuItem.Name = "addressToolStripMenuItem";
			this.addressToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
			this.addressToolStripMenuItem.Text = "Address";
			this.addressToolStripMenuItem.Click += new System.EventHandler(this.colButtonClick);
			// 
			// pageToolStripMenuItem
			// 
			this.pageToolStripMenuItem.Checked = true;
			this.pageToolStripMenuItem.CheckOnClick = true;
			this.pageToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.pageToolStripMenuItem.Name = "pageToolStripMenuItem";
			this.pageToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
			this.pageToolStripMenuItem.Text = "Page";
			this.pageToolStripMenuItem.Click += new System.EventHandler(this.colButtonClick);
			// 
			// isInRamToolStripMenuItem
			// 
			this.isInRamToolStripMenuItem.Checked = true;
			this.isInRamToolStripMenuItem.CheckOnClick = true;
			this.isInRamToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
			this.isInRamToolStripMenuItem.Name = "isInRamToolStripMenuItem";
			this.isInRamToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
			this.isInRamToolStripMenuItem.Text = "Is In Ram";
			this.isInRamToolStripMenuItem.Click += new System.EventHandler(this.colButtonClick);
			// 
			// breakpointGridView
			// 
			this.breakpointGridView.AllowUserToAddRows = false;
			this.breakpointGridView.AllowUserToResizeRows = false;
			this.breakpointGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.breakpointGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.EnabledCol,
            this.NameCol,
            this.ConditionCol,
            this.HitCountCol,
            this.AddressCol,
            this.PageCol,
            this.IsRamCol});
			this.breakpointGridView.Dock = System.Windows.Forms.DockStyle.Fill;
			this.breakpointGridView.Location = new System.Drawing.Point(0, 25);
			this.breakpointGridView.Name = "breakpointGridView";
			this.breakpointGridView.RowHeadersVisible = false;
			this.breakpointGridView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
			this.breakpointGridView.ShowEditingIcon = false;
			this.breakpointGridView.ShowRowErrors = false;
			this.breakpointGridView.Size = new System.Drawing.Size(535, 189);
			this.breakpointGridView.TabIndex = 1;
			this.breakpointGridView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.breakpointGridView_CellValueChanged);
			this.breakpointGridView.UserDeletingRow += new System.Windows.Forms.DataGridViewRowCancelEventHandler(this.breakpointGridView_UserDeletingRow);
			// 
			// EnabledCol
			// 
			this.EnabledCol.HeaderText = "Enabled";
			this.EnabledCol.Name = "EnabledCol";
			this.EnabledCol.Width = 50;
			// 
			// NameCol
			// 
			this.NameCol.HeaderText = "Name";
			this.NameCol.Name = "NameCol";
			this.NameCol.ReadOnly = true;
			// 
			// ConditionCol
			// 
			this.ConditionCol.HeaderText = "Condition";
			this.ConditionCol.Name = "ConditionCol";
			// 
			// HitCountCol
			// 
			this.HitCountCol.HeaderText = "Hit Count";
			this.HitCountCol.Name = "HitCountCol";
			// 
			// AddressCol
			// 
			this.AddressCol.HeaderText = "Address";
			this.AddressCol.Name = "AddressCol";
			// 
			// PageCol
			// 
			this.PageCol.HeaderText = "Page";
			this.PageCol.Name = "PageCol";
			// 
			// IsRamCol
			// 
			this.IsRamCol.HeaderText = "In Ram";
			this.IsRamCol.Name = "IsRamCol";
			this.IsRamCol.Width = 50;
			// 
			// BreakpointManager
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(535, 214);
			this.Controls.Add(this.breakpointGridView);
			this.Controls.Add(this.toolStrip1);
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.HideOnClose = true;
			this.Name = "BreakpointManager";
			this.ShowHint = Revsoft.Docking.DockState.DockBottom;
			this.TabPageContextMenu = this.contextMenu;
			this.TabText = "Breakpoints";
			this.Text = "Breakpoints";
			this.toolStrip1.ResumeLayout(false);
			this.toolStrip1.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.breakpointGridView)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ContextMenu contextMenu;
        private System.Windows.Forms.MenuItem autoHideItem;
        private System.Windows.Forms.MenuItem floatingItem;
        private System.Windows.Forms.MenuItem hideItem;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripSplitButton newBreakToolStripButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton delBreakToolStripButton;
        private System.Windows.Forms.ToolStripButton delAllBreakToolStripButton;
        private System.Windows.Forms.ToolStripButton disableAllToolStripButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton gotoToolStripButton;
        private System.Windows.Forms.ToolStripMenuItem breakpointToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem dataBreakpointToolStripMenuItem;
        private System.Windows.Forms.ToolStripDropDownButton colToolStripDropDown;
        private System.Windows.Forms.ToolStripMenuItem nameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hitCountToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem conditionToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addressToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem pageToolStripMenuItem;
        private System.Windows.Forms.DataGridView breakpointGridView;
        private System.Windows.Forms.DataGridViewCheckBoxColumn EnabledCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn NameCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn ConditionCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn HitCountCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn AddressCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn PageCol;
        private System.Windows.Forms.DataGridViewCheckBoxColumn IsRamCol;
        private System.Windows.Forms.ToolStripMenuItem enabledToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem isInRamToolStripMenuItem;

    }
}