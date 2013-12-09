namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    partial class IncludeDir
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
            this.addDirButton = new System.Windows.Forms.Button();
            this.deleteDirButton = new System.Windows.Forms.Button();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.includeDirBrowser = new System.Windows.Forms.FolderBrowserDialog();
            this.includeDirList = new System.Windows.Forms.ListBox();
            this.SuspendLayout();
            // 
            // addDirButton
            // 
            this.addDirButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.addDirButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.addDirButton.Location = new System.Drawing.Point(294, 13);
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
            this.deleteDirButton.Location = new System.Drawing.Point(294, 42);
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
            this.okButton.Location = new System.Drawing.Point(294, 160);
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
            this.cancelButton.Location = new System.Drawing.Point(294, 189);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 1;
            this.cancelButton.Text = "Cancel";
            // 
            // includeDirList
            // 
            this.includeDirList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.includeDirList.FormattingEnabled = true;
            this.includeDirList.HorizontalScrollbar = true;
            this.includeDirList.Location = new System.Drawing.Point(12, 12);
            this.includeDirList.Name = "includeDirList";
            this.includeDirList.Size = new System.Drawing.Size(276, 199);
            this.includeDirList.TabIndex = 2;
            // 
            // IncludeDirs
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(381, 220);
            this.Controls.Add(this.includeDirList);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.deleteDirButton);
            this.Controls.Add(this.addDirButton);
            this.MinimumSize = new System.Drawing.Size(200, 169);
            this.Name = "IncludeDir";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Include Directories";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button addDirButton;
        private System.Windows.Forms.Button deleteDirButton;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.FolderBrowserDialog includeDirBrowser;
        private System.Windows.Forms.ListBox includeDirList;
    }
}