namespace Revsoft.Wabbitcode
{
    partial class RefactorForm
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
			this.nameBox = new System.Windows.Forms.TextBox();
			this.newNameLabel = new System.Windows.Forms.Label();
			this.tabControl = new System.Windows.Forms.TabControl();
			this.previewButton = new System.Windows.Forms.Button();
			this.okButton = new System.Windows.Forms.Button();
			this.cancelButton = new System.Windows.Forms.Button();
			this.prevRefButton = new System.Windows.Forms.Button();
			this.nextRefButton = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// nameBox
			// 
			this.nameBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.nameBox.Location = new System.Drawing.Point(12, 33);
			this.nameBox.Name = "nameBox";
			this.nameBox.Size = new System.Drawing.Size(603, 20);
			this.nameBox.TabIndex = 0;
			// 
			// newNameLabel
			// 
			this.newNameLabel.AutoSize = true;
			this.newNameLabel.Location = new System.Drawing.Point(12, 15);
			this.newNameLabel.Name = "newNameLabel";
			this.newNameLabel.Size = new System.Drawing.Size(63, 13);
			this.newNameLabel.TabIndex = 1;
			this.newNameLabel.Text = "New Name:";
			// 
			// tabControl
			// 
			this.tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tabControl.Location = new System.Drawing.Point(12, 90);
			this.tabControl.Name = "tabControl";
			this.tabControl.SelectedIndex = 0;
			this.tabControl.Size = new System.Drawing.Size(603, 0);
			this.tabControl.TabIndex = 3;
			this.tabControl.Visible = false;
			// 
			// previewButton
			// 
			this.previewButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.previewButton.Location = new System.Drawing.Point(378, 59);
			this.previewButton.Name = "previewButton";
			this.previewButton.Size = new System.Drawing.Size(75, 23);
			this.previewButton.TabIndex = 4;
			this.previewButton.Text = "Preview...";
			this.previewButton.UseVisualStyleBackColor = true;
			this.previewButton.Click += new System.EventHandler(this.previewButton_Click);
			// 
			// okButton
			// 
			this.okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.okButton.Location = new System.Drawing.Point(459, 59);
			this.okButton.Name = "okButton";
			this.okButton.Size = new System.Drawing.Size(75, 23);
			this.okButton.TabIndex = 4;
			this.okButton.Text = "OK";
			this.okButton.UseVisualStyleBackColor = true;
			this.okButton.Click += new System.EventHandler(this.okButton_Click);
			// 
			// cancelButton
			// 
			this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(540, 59);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(75, 23);
			this.cancelButton.TabIndex = 5;
			this.cancelButton.Text = "Cancel";
			this.cancelButton.UseVisualStyleBackColor = true;
			this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
			// 
			// prevRefButton
			// 
			this.prevRefButton.Location = new System.Drawing.Point(12, 59);
			this.prevRefButton.Name = "prevRefButton";
			this.prevRefButton.Size = new System.Drawing.Size(31, 23);
			this.prevRefButton.TabIndex = 6;
			this.prevRefButton.Text = "<";
			this.prevRefButton.UseVisualStyleBackColor = true;
			this.prevRefButton.Visible = false;
			this.prevRefButton.Click += new System.EventHandler(this.prevRefButton_Click);
			// 
			// nextRefButton
			// 
			this.nextRefButton.Location = new System.Drawing.Point(49, 59);
			this.nextRefButton.Name = "nextRefButton";
			this.nextRefButton.Size = new System.Drawing.Size(31, 23);
			this.nextRefButton.TabIndex = 7;
			this.nextRefButton.Text = ">";
			this.nextRefButton.UseVisualStyleBackColor = true;
			this.nextRefButton.Visible = false;
			this.nextRefButton.Click += new System.EventHandler(this.nextRefButton_Click);
			// 
			// RefactorForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(627, 94);
			this.Controls.Add(this.nextRefButton);
			this.Controls.Add(this.prevRefButton);
			this.Controls.Add(this.cancelButton);
			this.Controls.Add(this.tabControl);
			this.Controls.Add(this.okButton);
			this.Controls.Add(this.previewButton);
			this.Controls.Add(this.newNameLabel);
			this.Controls.Add(this.nameBox);
			this.Name = "RefactorForm";
			this.Text = "Refactor";
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

		private System.Windows.Forms.TextBox nameBox;
		private System.Windows.Forms.Label newNameLabel;
		private System.Windows.Forms.TabControl tabControl;
		private System.Windows.Forms.Button cancelButton;
		private System.Windows.Forms.Button okButton;
		private System.Windows.Forms.Button previewButton;
		private System.Windows.Forms.Button prevRefButton;
		private System.Windows.Forms.Button nextRefButton;
    }
}