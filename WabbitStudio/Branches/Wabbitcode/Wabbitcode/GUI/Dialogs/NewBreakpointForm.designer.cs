namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    partial class NewBreakpointForm
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
            this.fileBox = new System.Windows.Forms.TextBox();
            this.fileLabel = new System.Windows.Forms.Label();
            this.lineBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.browseButton = new System.Windows.Forms.Button();
            this.textLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // fileBox
            // 
            this.fileBox.Location = new System.Drawing.Point(60, 27);
            this.fileBox.Name = "fileBox";
            this.fileBox.Size = new System.Drawing.Size(220, 20);
            this.fileBox.TabIndex = 0;
            this.fileBox.TextChanged += new System.EventHandler(this.TextChange);
            // 
            // fileLabel
            // 
            this.fileLabel.AutoSize = true;
            this.fileLabel.Location = new System.Drawing.Point(28, 30);
            this.fileLabel.Name = "fileLabel";
            this.fileLabel.Size = new System.Drawing.Size(26, 13);
            this.fileLabel.TabIndex = 1;
            this.fileLabel.Text = "File:";
            // 
            // lineBox
            // 
            this.lineBox.Location = new System.Drawing.Point(60, 53);
            this.lineBox.Name = "lineBox";
            this.lineBox.Size = new System.Drawing.Size(83, 20);
            this.lineBox.TabIndex = 2;
            this.lineBox.TextChanged += new System.EventHandler(this.TextChange);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(24, 56);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(30, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Line:";
            // 
            // okButton
            // 
            this.okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(204, 119);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 3;
            this.okButton.Text = "OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(285, 119);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 4;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            // 
            // browseButton
            // 
            this.browseButton.Location = new System.Drawing.Point(286, 25);
            this.browseButton.Name = "browseButton";
            this.browseButton.Size = new System.Drawing.Size(75, 23);
            this.browseButton.TabIndex = 1;
            this.browseButton.Text = "Browse...";
            this.browseButton.UseVisualStyleBackColor = true;
            this.browseButton.Click += new System.EventHandler(this.browseButton_Click);
            // 
            // textLabel
            // 
            this.textLabel.AutoSize = true;
            this.textLabel.Location = new System.Drawing.Point(24, 84);
            this.textLabel.Name = "textLabel";
            this.textLabel.Size = new System.Drawing.Size(31, 13);
            this.textLabel.TabIndex = 1;
            this.textLabel.Text = "Text:";
            // 
            // NewBreakpointForm
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(372, 154);
            this.Controls.Add(this.browseButton);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.textLabel);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.fileLabel);
            this.Controls.Add(this.lineBox);
            this.Controls.Add(this.fileBox);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "NewBreakpointForm";
            this.Text = "New Breakpoint";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox fileBox;
        private System.Windows.Forms.Label fileLabel;
        private System.Windows.Forms.TextBox lineBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Button browseButton;
        private System.Windows.Forms.Label textLabel;
    }
}