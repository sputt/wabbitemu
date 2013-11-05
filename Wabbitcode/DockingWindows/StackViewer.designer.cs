namespace Revsoft.Wabbitcode.DockingWindows
{
    public partial class StackViewer
    {
        private System.Windows.Forms.MenuItem autoHideItem;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.ContextMenu contextMenu;
        private System.Windows.Forms.MenuItem floatingItem;
        private System.Windows.Forms.MenuItem hideItem;
        private System.Windows.Forms.DataGridView stackView;

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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(StackViewer));
            this.contextMenu = new System.Windows.Forms.ContextMenu();
            this.autoHideItem = new System.Windows.Forms.MenuItem();
            this.floatingItem = new System.Windows.Forms.MenuItem();
            this.hideItem = new System.Windows.Forms.MenuItem();
            this.stackView = new System.Windows.Forms.DataGridView();
            this.addressCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.stackData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.stackView)).BeginInit();
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
            // stackView
            // 
            this.stackView.AllowUserToAddRows = false;
            this.stackView.AllowUserToDeleteRows = false;
            this.stackView.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.stackView.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.stackView.BackgroundColor = System.Drawing.SystemColors.Window;
            this.stackView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.stackView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.addressCol,
            this.stackData});
            this.stackView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.stackView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.stackView.Location = new System.Drawing.Point(0, 0);
            this.stackView.MultiSelect = false;
            this.stackView.Name = "stackView";
            this.stackView.ReadOnly = true;
            this.stackView.RowHeadersVisible = false;
            this.stackView.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.stackView.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.stackView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.stackView.Size = new System.Drawing.Size(503, 172);
            this.stackView.TabIndex = 0;
            this.stackView.RowPostPaint += new System.Windows.Forms.DataGridViewRowPostPaintEventHandler(this.stackView_RowPostPaint);
            this.stackView.DoubleClick += new System.EventHandler(this.stackView_DoubleClick);
            // 
            // addressCol
            // 
            this.addressCol.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.addressCol.FillWeight = 20F;
            this.addressCol.HeaderText = "Address";
            this.addressCol.Name = "addressCol";
            this.addressCol.ReadOnly = true;
            this.addressCol.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // stackData
            // 
            this.stackData.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.stackData.HeaderText = "Stack Data";
            this.stackData.Name = "stackData";
            this.stackData.ReadOnly = true;
            this.stackData.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // StackViewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(503, 172);
            this.Controls.Add(this.stackView);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "StackViewer";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
            this.TabPageContextMenu = this.contextMenu;
            this.TabText = "Stack Viewer";
            this.Text = "Stack Viewer";
            ((System.ComponentModel.ISupportInitialize)(this.stackView)).EndInit();
            this.ResumeLayout(false);

        }

        private System.Windows.Forms.DataGridViewTextBoxColumn addressCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn stackData;
    }
}