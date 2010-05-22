namespace Revsoft.Wabbitcode
{
    partial class BuildForm
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
            this.actionLabel = new System.Windows.Forms.Label();
            this.buildGroupBox = new System.Windows.Forms.GroupBox();
            this.outputBox = new System.Windows.Forms.ComboBox();
            this.fileLocBox = new System.Windows.Forms.TextBox();
            this.commandBox = new System.Windows.Forms.TextBox();
            this.browseButton = new System.Windows.Forms.Button();
            this.commandLabel = new System.Windows.Forms.Label();
            this.fileNameLabel = new System.Windows.Forms.Label();
            this.fileLabel = new System.Windows.Forms.Label();
            this.outputLabel = new System.Windows.Forms.Label();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.actionBox = new System.Windows.Forms.ComboBox();
            this.buildGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // actionLabel
            // 
            this.actionLabel.AutoSize = true;
            this.actionLabel.Location = new System.Drawing.Point(34, 9);
            this.actionLabel.Name = "actionLabel";
            this.actionLabel.Size = new System.Drawing.Size(40, 13);
            this.actionLabel.TabIndex = 0;
            this.actionLabel.Text = "Action:";
            // 
            // buildGroupBox
            // 
            this.buildGroupBox.Controls.Add(this.outputBox);
            this.buildGroupBox.Controls.Add(this.fileLocBox);
            this.buildGroupBox.Controls.Add(this.commandBox);
            this.buildGroupBox.Controls.Add(this.browseButton);
            this.buildGroupBox.Controls.Add(this.commandLabel);
            this.buildGroupBox.Controls.Add(this.fileNameLabel);
            this.buildGroupBox.Controls.Add(this.fileLabel);
            this.buildGroupBox.Controls.Add(this.outputLabel);
            this.buildGroupBox.Location = new System.Drawing.Point(12, 33);
            this.buildGroupBox.Name = "buildGroupBox";
            this.buildGroupBox.Size = new System.Drawing.Size(239, 114);
            this.buildGroupBox.TabIndex = 4;
            this.buildGroupBox.TabStop = false;
            this.buildGroupBox.Text = "Build File";
            // 
            // outputBox
            // 
            this.outputBox.FormattingEnabled = true;
            this.outputBox.Items.AddRange(new object[] {
            "Binary",
            "TI-73 Program",
            "TI-82 Program",
            "TI-83 Program",
            "TI-83 Plus Program",
            "TI-83 Plus Application",
            "TI-85 Program",
            "TI-85 String",
            "TI-86 Program",
            "TI-86 String"});
            this.outputBox.Location = new System.Drawing.Point(100, 24);
            this.outputBox.Name = "outputBox";
            this.outputBox.Size = new System.Drawing.Size(121, 21);
            this.outputBox.TabIndex = 7;
            // 
            // fileLocBox
            // 
            this.fileLocBox.Location = new System.Drawing.Point(78, 85);
            this.fileLocBox.Name = "fileLocBox";
            this.fileLocBox.Size = new System.Drawing.Size(151, 20);
            this.fileLocBox.TabIndex = 8;
            this.fileLocBox.Visible = false;
            // 
            // commandBox
            // 
            this.commandBox.Location = new System.Drawing.Point(90, 44);
            this.commandBox.Name = "commandBox";
            this.commandBox.Size = new System.Drawing.Size(139, 20);
            this.commandBox.TabIndex = 8;
            this.commandBox.Visible = false;
            // 
            // browseButton
            // 
            this.browseButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.browseButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.browseButton.Location = new System.Drawing.Point(125, 57);
            this.browseButton.Name = "browseButton";
            this.browseButton.Size = new System.Drawing.Size(75, 23);
            this.browseButton.TabIndex = 7;
            this.browseButton.Text = "Browse...";
            this.browseButton.Click += new System.EventHandler(this.browseButton_Click);
            // 
            // commandLabel
            // 
            this.commandLabel.AutoSize = true;
            this.commandLabel.Location = new System.Drawing.Point(27, 46);
            this.commandLabel.Name = "commandLabel";
            this.commandLabel.Size = new System.Drawing.Size(57, 13);
            this.commandLabel.TabIndex = 4;
            this.commandLabel.Text = "Command:";
            this.commandLabel.Visible = false;
            // 
            // fileNameLabel
            // 
            this.fileNameLabel.AutoSize = true;
            this.fileNameLabel.Location = new System.Drawing.Point(15, 87);
            this.fileNameLabel.Name = "fileNameLabel";
            this.fileNameLabel.Size = new System.Drawing.Size(57, 13);
            this.fileNameLabel.TabIndex = 4;
            this.fileNameLabel.Text = "File Name:";
            // 
            // fileLabel
            // 
            this.fileLabel.AutoSize = true;
            this.fileLabel.Location = new System.Drawing.Point(58, 62);
            this.fileLabel.Name = "fileLabel";
            this.fileLabel.Size = new System.Drawing.Size(26, 13);
            this.fileLabel.TabIndex = 4;
            this.fileLabel.Text = "File:";
            // 
            // outputLabel
            // 
            this.outputLabel.AutoSize = true;
            this.outputLabel.Location = new System.Drawing.Point(15, 27);
            this.outputLabel.Name = "outputLabel";
            this.outputLabel.Size = new System.Drawing.Size(69, 13);
            this.outputLabel.TabIndex = 5;
            this.outputLabel.Text = "Output Type:";
            // 
            // okButton
            // 
            this.okButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(98, 153);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 5;
            this.okButton.Text = "OK";
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(181, 153);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 5;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // actionBox
            // 
            this.actionBox.FormattingEnabled = true;
            this.actionBox.Items.AddRange(new object[] {
            "Assemble File",
            "Build Symbol Table",
            "Build Listing File",
            "Run External Program"});
            this.actionBox.Location = new System.Drawing.Point(98, 6);
            this.actionBox.Name = "actionBox";
            this.actionBox.Size = new System.Drawing.Size(143, 21);
            this.actionBox.TabIndex = 6;
            this.actionBox.SelectedIndexChanged += new System.EventHandler(this.actionBox_SelectedIndexChanged);
            // 
            // BuildForm
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(268, 188);
            this.Controls.Add(this.actionBox);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.buildGroupBox);
            this.Controls.Add(this.actionLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "BuildForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Edit Build Step";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.BuildForm_FormClosing);
            this.buildGroupBox.ResumeLayout(false);
            this.buildGroupBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label actionLabel;
        private System.Windows.Forms.GroupBox buildGroupBox;
        private System.Windows.Forms.Button browseButton;
        private System.Windows.Forms.Label fileLabel;
        private System.Windows.Forms.Label outputLabel;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.Label commandLabel;
        public System.Windows.Forms.TextBox commandBox;
        public System.Windows.Forms.TextBox fileLocBox;
        private System.Windows.Forms.Label fileNameLabel;
        public System.Windows.Forms.ComboBox actionBox;
        public System.Windows.Forms.ComboBox outputBox;
    }
}