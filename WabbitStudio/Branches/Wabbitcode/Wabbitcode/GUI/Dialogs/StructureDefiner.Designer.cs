using Revsoft.Wabbitcode.GUI.DockingWindows;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    partial class StructureDefiner
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

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
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.labelStructure = new System.Windows.Forms.Label();
            this.sturctureGrid = new System.Windows.Forms.DataGridView();
            this.defineName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.numBits = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Type = new System.Windows.Forms.DataGridViewComboBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.sturctureGrid)).BeginInit();
            this.SuspendLayout();
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(76, 6);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(183, 21);
            this.comboBox1.TabIndex = 1;
            // 
            // labelStructure
            // 
            this.labelStructure.AutoSize = true;
            this.labelStructure.Location = new System.Drawing.Point(12, 9);
            this.labelStructure.Name = "labelStructure";
            this.labelStructure.Size = new System.Drawing.Size(58, 13);
            this.labelStructure.TabIndex = 2;
            this.labelStructure.Text = "Structures:";
            // 
            // sturctureGrid
            // 
            this.sturctureGrid.AllowUserToOrderColumns = true;
            this.sturctureGrid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.sturctureGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.sturctureGrid.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.defineName,
            this.numBits,
            this.Type});
            this.sturctureGrid.Location = new System.Drawing.Point(12, 33);
            this.sturctureGrid.Name = "sturctureGrid";
            this.sturctureGrid.RowHeadersVisible = false;
            this.sturctureGrid.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.sturctureGrid.Size = new System.Drawing.Size(483, 142);
            this.sturctureGrid.TabIndex = 3;
            this.sturctureGrid.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.sturctureGrid_CellEndEdit);
            this.sturctureGrid.RowsAdded += new System.Windows.Forms.DataGridViewRowsAddedEventHandler(this.sturctureGrid_RowsAdded);
            // 
            // defineName
            // 
            this.defineName.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.defineName.HeaderText = "Name";
            this.defineName.Name = "defineName";
            // 
            // numBits
            // 
            this.numBits.FillWeight = 300F;
            this.numBits.HeaderText = "Num of bits";
            this.numBits.Name = "numBits";
            // 
            // Type
            // 
            this.Type.HeaderText = "Type";
            this.Type.Items.AddRange(new object[] {
            "Hexadecimal",
            "Decimal",
            "Binary",
            "Octal"});
            this.Type.Name = "Type";
            this.Type.Width = 200;
            // 
            // StructureDefiner
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(507, 187);
            this.Controls.Add(this.sturctureGrid);
            this.Controls.Add(this.labelStructure);
            this.Controls.Add(this.comboBox1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "StructureDefiner";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Structure";
            this.TopMost = true;
            ((System.ComponentModel.ISupportInitialize)(this.sturctureGrid)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Label labelStructure;
        private System.Windows.Forms.DataGridView sturctureGrid;
        private System.Windows.Forms.DataGridViewTextBoxColumn defineName;
        private System.Windows.Forms.DataGridViewTextBoxColumn numBits;
        private System.Windows.Forms.DataGridViewComboBoxColumn Type;

    }
}