namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class LabelList
    {
        private System.Windows.Forms.CheckBox alphaBox;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.CheckBox includeEquatesBox;
        private System.Windows.Forms.ListBox labelsBox;

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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LabelList));
			this.includeEquatesBox = new System.Windows.Forms.CheckBox();
			this.labelsBox = new System.Windows.Forms.ListBox();
			this.alphaBox = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// includeEquatesBox
			// 
			this.includeEquatesBox.AutoSize = true;
			this.includeEquatesBox.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.includeEquatesBox.Location = new System.Drawing.Point(0, 308);
			this.includeEquatesBox.Name = "includeEquatesBox";
			this.includeEquatesBox.Padding = new System.Windows.Forms.Padding(5, 0, 0, 0);
			this.includeEquatesBox.Size = new System.Drawing.Size(225, 17);
			this.includeEquatesBox.TabIndex = 0;
			this.includeEquatesBox.Text = "Show Equates";
			this.includeEquatesBox.UseVisualStyleBackColor = true;
			this.includeEquatesBox.CheckedChanged += new System.EventHandler(this.includeEquatesBox_CheckedChanged);
			// 
			// labelsBox
			// 
			this.labelsBox.Dock = System.Windows.Forms.DockStyle.Fill;
			this.labelsBox.FormattingEnabled = false;
	        this.labelsBox.IntegralHeight = false;
			this.labelsBox.Location = new System.Drawing.Point(0, 2);
			this.labelsBox.Name = "labelsBox";
			this.labelsBox.Size = new System.Drawing.Size(225, 306);
			this.labelsBox.TabIndex = 1;
			this.labelsBox.DoubleClick += new System.EventHandler(this.labelsBox_DoubleClick);
			this.labelsBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.labelsBox_KeyPress);
			// 
			// alphaBox
			// 
			this.alphaBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
			this.alphaBox.AutoSize = true;
			this.alphaBox.Location = new System.Drawing.Point(141, 308);
			this.alphaBox.Name = "alphaBox";
			this.alphaBox.Size = new System.Drawing.Size(81, 17);
			this.alphaBox.TabIndex = 2;
			this.alphaBox.Text = "Alphabetize";
			this.alphaBox.UseVisualStyleBackColor = true;
			this.alphaBox.CheckedChanged += new System.EventHandler(this.alphaBox_CheckedChanged);
			// 
			// LabelList
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.ClientSize = new System.Drawing.Size(225, 327);
			this.Controls.Add(this.alphaBox);
			this.Controls.Add(this.labelsBox);
			this.Controls.Add(this.includeEquatesBox);
	        this.DoubleBuffered = true;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "LabelList";
			this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
			this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockLeft;
			this.TabText = "Label List";
			this.Text = "Label List";
			this.VisibleChanged += new System.EventHandler(this.LabelList_VisibleChanged);
			this.ResumeLayout(false);
			this.PerformLayout();

        }
    }
}