namespace Revsoft.Wabbitcode
{
    partial class BuildSteps
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
            this.buildSeqList = new System.Windows.Forms.ListBox();
            this.addDirButton = new System.Windows.Forms.Button();
            this.deleteDirButton = new System.Windows.Forms.Button();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.includeDirBrowser = new System.Windows.Forms.FolderBrowserDialog();
            this.editButton = new System.Windows.Forms.Button();
            this.moveUp = new System.Windows.Forms.Button();
            this.moveDown = new System.Windows.Forms.Button();
            this.configBox = new System.Windows.Forms.ComboBox();
            this.configLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // buildSeqList
            // 
            this.buildSeqList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.buildSeqList.FormattingEnabled = true;
            this.buildSeqList.HorizontalScrollbar = true;
            this.buildSeqList.Location = new System.Drawing.Point(13, 39);
            this.buildSeqList.Name = "buildSeqList";
            this.buildSeqList.Size = new System.Drawing.Size(275, 199);
            this.buildSeqList.TabIndex = 0;
            this.buildSeqList.DoubleClick += new System.EventHandler(this.editButton_Click);
            // 
            // addDirButton
            // 
            this.addDirButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.addDirButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.addDirButton.Location = new System.Drawing.Point(294, 39);
            this.addDirButton.Name = "addDirButton";
            this.addDirButton.Size = new System.Drawing.Size(75, 23);
            this.addDirButton.TabIndex = 1;
            this.addDirButton.Text = "Add";
            this.addDirButton.Click += new System.EventHandler(this.addDirButton_Click);
            // 
            // deleteDirButton
            // 
            this.deleteDirButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.deleteDirButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.deleteDirButton.Location = new System.Drawing.Point(294, 97);
            this.deleteDirButton.Name = "deleteDirButton";
            this.deleteDirButton.Size = new System.Drawing.Size(75, 23);
            this.deleteDirButton.TabIndex = 1;
            this.deleteDirButton.Text = "Delete";
            this.deleteDirButton.Click += new System.EventHandler(this.deleteDirButton_Click);
            // 
            // okButton
            // 
            this.okButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(294, 192);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 1;
            this.okButton.Text = "OK";
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(294, 221);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 1;
            this.cancelButton.Text = "Cancel";
            // 
            // editButton
            // 
            this.editButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.editButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.editButton.Location = new System.Drawing.Point(294, 68);
            this.editButton.Name = "editButton";
            this.editButton.Size = new System.Drawing.Size(75, 23);
            this.editButton.TabIndex = 1;
            this.editButton.Text = "Edit";
            this.editButton.Click += new System.EventHandler(this.editButton_Click);
            // 
            // moveUp
            // 
            this.moveUp.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.moveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.moveUp.Font = new System.Drawing.Font("Times New Roman", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.moveUp.Location = new System.Drawing.Point(298, 126);
            this.moveUp.Name = "moveUp";
            this.moveUp.Size = new System.Drawing.Size(28, 23);
            this.moveUp.TabIndex = 1;
            this.moveUp.Text = "↑";
            this.moveUp.Click += new System.EventHandler(this.moveUp_Click);
            // 
            // moveDown
            // 
            this.moveDown.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.moveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.moveDown.Font = new System.Drawing.Font("Times New Roman", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.moveDown.Location = new System.Drawing.Point(338, 126);
            this.moveDown.Name = "moveDown";
            this.moveDown.Size = new System.Drawing.Size(28, 23);
            this.moveDown.TabIndex = 1;
            this.moveDown.Text = "↓";
            this.moveDown.Click += new System.EventHandler(this.moveDown_Click);
            // 
            // configBox
            // 
            this.configBox.FormattingEnabled = true;
            this.configBox.Location = new System.Drawing.Point(116, 10);
            this.configBox.Name = "configBox";
            this.configBox.Size = new System.Drawing.Size(158, 21);
            this.configBox.TabIndex = 2;
            this.configBox.SelectedIndexChanged += new System.EventHandler(this.configBox_SelectedIndexChanged);
            this.configBox.DropDown += new System.EventHandler(this.configBox_DropDown);
            // 
            // configLabel
            // 
            this.configLabel.AutoSize = true;
            this.configLabel.Location = new System.Drawing.Point(12, 13);
            this.configLabel.Name = "configLabel";
            this.configLabel.Size = new System.Drawing.Size(98, 13);
            this.configLabel.TabIndex = 3;
            this.configLabel.Text = "Build Configuration:";
            // 
            // BuildSteps
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(381, 252);
            this.Controls.Add(this.configLabel);
            this.Controls.Add(this.configBox);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.editButton);
            this.Controls.Add(this.moveDown);
            this.Controls.Add(this.moveUp);
            this.Controls.Add(this.deleteDirButton);
            this.Controls.Add(this.addDirButton);
            this.Controls.Add(this.buildSeqList);
            this.MinimumSize = new System.Drawing.Size(200, 230);
            this.Name = "BuildSteps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Build Steps";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox buildSeqList;
        private System.Windows.Forms.Button addDirButton;
        private System.Windows.Forms.Button deleteDirButton;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.FolderBrowserDialog includeDirBrowser;
        private System.Windows.Forms.Button editButton;
        private System.Windows.Forms.Button moveUp;
        private System.Windows.Forms.Button moveDown;
        private System.Windows.Forms.ComboBox configBox;
        private System.Windows.Forms.Label configLabel;
    }
}