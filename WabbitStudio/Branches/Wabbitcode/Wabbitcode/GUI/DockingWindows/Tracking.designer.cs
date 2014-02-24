namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class TrackingWindow
    {
        private System.Windows.Forms.MenuItem autoHideItem;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.ContextMenu contextMenu;
        private System.Windows.Forms.DataGridViewComboBoxColumn DisplayMethod;
        private System.Windows.Forms.MenuItem floatingItem;
        private System.Windows.Forms.MenuItem hideItem;
        private System.Windows.Forms.DataGridViewTextBoxColumn NumberOfBytesCol;
        private System.Windows.Forms.DataGridViewComboBoxColumn ValueType;
        private System.Windows.Forms.DataGridViewTextBoxColumn VariableAddressCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn VariableNameCol;
        private System.Windows.Forms.DataGridView variablesDataView;
        private System.Windows.Forms.DataGridViewTextBoxColumn VariableValueCol;

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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TrackingWindow));
            this.contextMenu = new System.Windows.Forms.ContextMenu();
            this.autoHideItem = new System.Windows.Forms.MenuItem();
            this.floatingItem = new System.Windows.Forms.MenuItem();
            this.hideItem = new System.Windows.Forms.MenuItem();
            this.variablesDataView = new System.Windows.Forms.DataGridView();
            this.VariableNameCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.VariableAddressCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.NumberOfBytesCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ValueType = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.DisplayMethod = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.VariableValueCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.variablesDataView)).BeginInit();
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
            // variablesDataView
            // 
            this.variablesDataView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.variablesDataView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.VariableNameCol,
            this.VariableAddressCol,
            this.NumberOfBytesCol,
            this.ValueType,
            this.DisplayMethod,
            this.VariableValueCol});
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.variablesDataView.DefaultCellStyle = dataGridViewCellStyle2;
            this.variablesDataView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.variablesDataView.GridColor = System.Drawing.SystemColors.Window;
            this.variablesDataView.Location = new System.Drawing.Point(0, 0);
            this.variablesDataView.MultiSelect = false;
            this.variablesDataView.Name = "variablesDataView";
            this.variablesDataView.RowHeadersVisible = false;
            this.variablesDataView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.variablesDataView.Size = new System.Drawing.Size(1107, 266);
            this.variablesDataView.TabIndex = 1;
            this.variablesDataView.CellContentDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.variablesDataView_CellContentDoubleClick);
            this.variablesDataView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.variablesDataView_CellEndEdit);
            this.variablesDataView.CellEnter += new System.Windows.Forms.DataGridViewCellEventHandler(this.variablesDataView_CellEnter);
            this.variablesDataView.RowPostPaint += new System.Windows.Forms.DataGridViewRowPostPaintEventHandler(this.variablesDataView_RowPostPaint);
            // 
            // VariableNameCol
            // 
            this.VariableNameCol.FillWeight = 30F;
            this.VariableNameCol.Frozen = true;
            this.VariableNameCol.HeaderText = "Variable Name";
            this.VariableNameCol.Name = "VariableNameCol";
            this.VariableNameCol.Visible = false;
            // 
            // VariableAddressCol
            // 
            this.VariableAddressCol.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.VariableAddressCol.FillWeight = 151.5464F;
            this.VariableAddressCol.HeaderText = "Address";
            this.VariableAddressCol.Name = "VariableAddressCol";
            this.VariableAddressCol.Width = 70;
            // 
            // NumberOfBytesCol
            // 
            this.NumberOfBytesCol.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.NumberOfBytesCol.FillWeight = 10.96005F;
            this.NumberOfBytesCol.HeaderText = "Bytes";
            this.NumberOfBytesCol.Name = "NumberOfBytesCol";
            this.NumberOfBytesCol.Width = 40;
            // 
            // ValueType
            // 
            this.ValueType.AutoComplete = false;
            this.ValueType.HeaderText = "Type";
            this.ValueType.Items.AddRange(new object[] {
            "Byte",
            "Word",
            "String",
            "Image"});
            this.ValueType.Name = "ValueType";
            this.ValueType.Resizable = System.Windows.Forms.DataGridViewTriState.True;
            this.ValueType.Width = 60;
            // 
            // DisplayMethod
            // 
            this.DisplayMethod.HeaderText = "Display Method";
            this.DisplayMethod.Name = "DisplayMethod";
            this.DisplayMethod.Resizable = System.Windows.Forms.DataGridViewTriState.True;
            this.DisplayMethod.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
            // 
            // VariableValueCol
            // 
            this.VariableValueCol.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.VariableValueCol.DefaultCellStyle = dataGridViewCellStyle1;
            this.VariableValueCol.FillWeight = 47.49356F;
            this.VariableValueCol.HeaderText = "Value";
            this.VariableValueCol.Name = "VariableValueCol";
            this.VariableValueCol.ReadOnly = true;
            // 
            // TrackingWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1107, 266);
            this.Controls.Add(this.variablesDataView);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "TrackingWindow";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
            this.TabPageContextMenu = this.contextMenu;
            this.TabText = WindowIdentifier;
            this.Text = WindowIdentifier;
            ((System.ComponentModel.ISupportInitialize)(this.variablesDataView)).EndInit();
            this.ResumeLayout(false);

        }
    }
}