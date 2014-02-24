namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class CallStack
    {
        private System.Windows.Forms.MenuItem autoHideItem;
        private System.Windows.Forms.DataGridView callStackView;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.ContextMenu contextMenu;
        private System.Windows.Forms.MenuItem floatingItem;
        private System.Windows.Forms.MenuItem hideItem;

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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CallStack));
            this.contextMenu = new System.Windows.Forms.ContextMenu();
            this.autoHideItem = new System.Windows.Forms.MenuItem();
            this.floatingItem = new System.Windows.Forms.MenuItem();
            this.hideItem = new System.Windows.Forms.MenuItem();
            this.callStackView = new System.Windows.Forms.DataGridView();
            this.callTypeColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.nameColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.callStackView)).BeginInit();
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
            // callStackView
            // 
            this.callStackView.AllowUserToAddRows = false;
            this.callStackView.AllowUserToDeleteRows = false;
            this.callStackView.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.callStackView.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
            this.callStackView.BackgroundColor = System.Drawing.SystemColors.Window;
            this.callStackView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.callStackView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.callTypeColumn,
            this.nameColumn});
            this.callStackView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.callStackView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditProgrammatically;
            this.callStackView.Location = new System.Drawing.Point(0, 0);
            this.callStackView.MultiSelect = false;
            this.callStackView.Name = "callStackView";
            this.callStackView.ReadOnly = true;
            this.callStackView.RowHeadersVisible = false;
            this.callStackView.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.callStackView.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.callStackView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.callStackView.Size = new System.Drawing.Size(503, 172);
            this.callStackView.TabIndex = 0;
            this.callStackView.RowPostPaint += new System.Windows.Forms.DataGridViewRowPostPaintEventHandler(this.callStackView_RowPostPaint);
            this.callStackView.DoubleClick += new System.EventHandler(this.callStackView_DoubleClick);
            // 
            // callTypeColumn
            // 
            this.callTypeColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.callTypeColumn.FillWeight = 50F;
            this.callTypeColumn.HeaderText = "Call Type";
            this.callTypeColumn.Name = "callTypeColumn";
            this.callTypeColumn.ReadOnly = true;
            this.callTypeColumn.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // nameColumn
            // 
            this.nameColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.nameColumn.HeaderText = "Name";
            this.nameColumn.Name = "nameColumn";
            this.nameColumn.ReadOnly = true;
            this.nameColumn.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // CallStack
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(503, 172);
            this.Controls.Add(this.callStackView);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "CallStack";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
            this.TabPageContextMenu = this.contextMenu;
            this.TabText = WindowIdentifier;
            this.Text = WindowIdentifier;
            ((System.ComponentModel.ISupportInitialize)(this.callStackView)).EndInit();
            this.ResumeLayout(false);

        }

        private System.Windows.Forms.DataGridViewTextBoxColumn callTypeColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn nameColumn;
    }
}