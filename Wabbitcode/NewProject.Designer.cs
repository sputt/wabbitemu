namespace Revsoft.Wabbitcode
{
    partial class NewProjectDialog : System.Windows.Forms.Form
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
            this.okTemplate = new System.Windows.Forms.Button();
            this.cancelTemplate = new System.Windows.Forms.Button();
            this.folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.nameLabel = new System.Windows.Forms.Label();
            this.nameTextBox = new System.Windows.Forms.TextBox();
            this.browseLoc = new System.Windows.Forms.Button();
            this.locTextBox = new System.Windows.Forms.TextBox();
            this.locLabel = new System.Windows.Forms.Label();
            this.projFromDirBox = new System.Windows.Forms.CheckBox();
            this.fileTypesBox = new System.Windows.Forms.TextBox();
            this.fileTypesLabel = new System.Windows.Forms.Label();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.SuspendLayout();
            // 
            // okTemplate
            // 
            this.okTemplate.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.okTemplate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.okTemplate.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okTemplate.Location = new System.Drawing.Point(288, 328);
            this.okTemplate.Name = "okTemplate";
            this.okTemplate.Size = new System.Drawing.Size(75, 23);
            this.okTemplate.TabIndex = 7;
            this.okTemplate.Text = "OK";
            this.okTemplate.Click += new System.EventHandler(this.okTemplate_Click);
            // 
            // cancelTemplate
            // 
            this.cancelTemplate.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.cancelTemplate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.cancelTemplate.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelTemplate.Location = new System.Drawing.Point(369, 328);
            this.cancelTemplate.Name = "cancelTemplate";
            this.cancelTemplate.Size = new System.Drawing.Size(75, 23);
            this.cancelTemplate.TabIndex = 8;
            this.cancelTemplate.Text = "Cancel";
            // 
            // nameLabel
            // 
            this.nameLabel.AutoSize = true;
            this.nameLabel.BackColor = System.Drawing.Color.Transparent;
            this.nameLabel.Location = new System.Drawing.Point(9, 219);
            this.nameLabel.Name = "nameLabel";
            this.nameLabel.Size = new System.Drawing.Size(72, 13);
            this.nameLabel.TabIndex = 19;
            this.nameLabel.Text = "Project name:";
            // 
            // nameTextBox
            // 
            this.nameTextBox.Location = new System.Drawing.Point(89, 216);
            this.nameTextBox.Name = "nameTextBox";
            this.nameTextBox.Size = new System.Drawing.Size(274, 20);
            this.nameTextBox.TabIndex = 2;
            // 
            // browseLoc
            // 
            this.browseLoc.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.browseLoc.Location = new System.Drawing.Point(369, 247);
            this.browseLoc.Name = "browseLoc";
            this.browseLoc.Size = new System.Drawing.Size(75, 23);
            this.browseLoc.TabIndex = 4;
            this.browseLoc.Text = "Browse...";
            this.browseLoc.Click += new System.EventHandler(this.browseLoc_Click);
            // 
            // locTextBox
            // 
            this.locTextBox.Location = new System.Drawing.Point(89, 247);
            this.locTextBox.Name = "locTextBox";
            this.locTextBox.Size = new System.Drawing.Size(274, 20);
            this.locTextBox.TabIndex = 3;
            // 
            // locLabel
            // 
            this.locLabel.AutoSize = true;
            this.locLabel.BackColor = System.Drawing.Color.Transparent;
            this.locLabel.Location = new System.Drawing.Point(32, 252);
            this.locLabel.Name = "locLabel";
            this.locLabel.Size = new System.Drawing.Size(51, 13);
            this.locLabel.TabIndex = 19;
            this.locLabel.Text = "Location:";
            // 
            // projFromDirBox
            // 
            this.projFromDirBox.AutoSize = true;
            this.projFromDirBox.Location = new System.Drawing.Point(89, 303);
            this.projFromDirBox.Name = "projFromDirBox";
            this.projFromDirBox.Size = new System.Drawing.Size(158, 17);
            this.projFromDirBox.TabIndex = 6;
            this.projFromDirBox.Text = "Create project from directory";
            this.projFromDirBox.UseVisualStyleBackColor = true;
            this.projFromDirBox.CheckedChanged += new System.EventHandler(this.projFromDirBox_CheckedChanged);
            // 
            // fileTypesBox
            // 
            this.fileTypesBox.Location = new System.Drawing.Point(89, 277);
            this.fileTypesBox.Name = "fileTypesBox";
            this.fileTypesBox.Size = new System.Drawing.Size(274, 20);
            this.fileTypesBox.TabIndex = 5;
            this.fileTypesBox.Text = "*.asm;*.z80;*.inc;*.bmp";
            this.fileTypesBox.Visible = false;
            // 
            // fileTypesLabel
            // 
            this.fileTypesLabel.AutoSize = true;
            this.fileTypesLabel.BackColor = System.Drawing.Color.Transparent;
            this.fileTypesLabel.Location = new System.Drawing.Point(25, 280);
            this.fileTypesLabel.Name = "fileTypesLabel";
            this.fileTypesLabel.Size = new System.Drawing.Size(54, 13);
            this.fileTypesLabel.TabIndex = 19;
            this.fileTypesLabel.Text = "File types:";
            this.fileTypesLabel.Visible = false;
            // 
            // tabControl
            // 
            this.tabControl.Location = new System.Drawing.Point(12, 12);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(432, 198);
            this.tabControl.TabIndex = 20;
            // 
            // NewProjectDialog
            // 
            this.AcceptButton = this.okTemplate;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelTemplate;
            this.ClientSize = new System.Drawing.Size(456, 363);
            this.Controls.Add(this.tabControl);
            this.Controls.Add(this.projFromDirBox);
            this.Controls.Add(this.fileTypesLabel);
            this.Controls.Add(this.locLabel);
            this.Controls.Add(this.nameLabel);
            this.Controls.Add(this.nameTextBox);
            this.Controls.Add(this.browseLoc);
            this.Controls.Add(this.fileTypesBox);
            this.Controls.Add(this.locTextBox);
            this.Controls.Add(this.cancelTemplate);
            this.Controls.Add(this.okTemplate);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "NewProjectDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = " New Project";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.templateForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button okTemplate;
        private System.Windows.Forms.Button cancelTemplate;
		private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog;
        private System.Windows.Forms.Label nameLabel;
        private System.Windows.Forms.TextBox nameTextBox;
        private System.Windows.Forms.Button browseLoc;
        private System.Windows.Forms.TextBox locTextBox;
        private System.Windows.Forms.Label locLabel;
        private System.Windows.Forms.CheckBox projFromDirBox;
        private System.Windows.Forms.TextBox fileTypesBox;
        private System.Windows.Forms.Label fileTypesLabel;
		private System.Windows.Forms.TabControl tabControl;

    }
}