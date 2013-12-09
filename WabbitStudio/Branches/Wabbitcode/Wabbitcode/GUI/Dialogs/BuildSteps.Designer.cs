namespace Revsoft.Wabbitcode.GUI.Dialogs
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
            this.moveUp = new System.Windows.Forms.Button();
            this.moveDown = new System.Windows.Forms.Button();
            this.configBox = new System.Windows.Forms.ComboBox();
            this.configLabel = new System.Windows.Forms.Label();
            this.stepOptionsBox = new System.Windows.Forms.GroupBox();
            this.browseOutput = new System.Windows.Forms.Button();
            this.browseInput = new System.Windows.Forms.Button();
            this.actionLabel = new System.Windows.Forms.Label();
            this.actionBox = new System.Windows.Forms.ComboBox();
            this.outputLabel = new System.Windows.Forms.Label();
            this.inputLabel = new System.Windows.Forms.Label();
            this.outputBox = new System.Windows.Forms.TextBox();
            this.inputBox = new System.Windows.Forms.TextBox();
            this.stepTypeLabel = new System.Windows.Forms.Label();
            this.stepTypeBox = new System.Windows.Forms.ComboBox();
            this.configManagerButton = new System.Windows.Forms.Button();
            this.stepOptionsBox.SuspendLayout();
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
            this.buildSeqList.Size = new System.Drawing.Size(337, 212);
            this.buildSeqList.TabIndex = 0;
            this.buildSeqList.SelectedIndexChanged += new System.EventHandler(this.buildSeqList_SelectedIndexChanged);
            this.buildSeqList.DoubleClick += new System.EventHandler(this.editButton_Click);
            // 
            // addDirButton
            // 
            this.addDirButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.addDirButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.addDirButton.Location = new System.Drawing.Point(356, 64);
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
            this.deleteDirButton.Location = new System.Drawing.Point(356, 93);
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
            this.okButton.Location = new System.Drawing.Point(275, 366);
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
            this.cancelButton.Location = new System.Drawing.Point(356, 366);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 1;
            this.cancelButton.Text = "Cancel";
            // 
            // moveUp
            // 
            this.moveUp.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.moveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.moveUp.Font = new System.Drawing.Font("Times New Roman", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.moveUp.Location = new System.Drawing.Point(360, 151);
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
            this.moveDown.Location = new System.Drawing.Point(400, 151);
            this.moveDown.Name = "moveDown";
            this.moveDown.Size = new System.Drawing.Size(28, 23);
            this.moveDown.TabIndex = 1;
            this.moveDown.Text = "↓";
            this.moveDown.Click += new System.EventHandler(this.moveDown_Click);
            // 
            // configBox
            // 
            this.configBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.configBox.FormattingEnabled = true;
            this.configBox.Location = new System.Drawing.Point(116, 10);
            this.configBox.Name = "configBox";
            this.configBox.Size = new System.Drawing.Size(207, 21);
            this.configBox.TabIndex = 2;
            this.configBox.DropDown += new System.EventHandler(this.configBox_DropDown);
            this.configBox.SelectedIndexChanged += new System.EventHandler(this.configBox_SelectedIndexChanged);
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
            // stepOptionsBox
            // 
            this.stepOptionsBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.stepOptionsBox.Controls.Add(this.browseOutput);
            this.stepOptionsBox.Controls.Add(this.browseInput);
            this.stepOptionsBox.Controls.Add(this.actionLabel);
            this.stepOptionsBox.Controls.Add(this.actionBox);
            this.stepOptionsBox.Controls.Add(this.outputLabel);
            this.stepOptionsBox.Controls.Add(this.inputLabel);
            this.stepOptionsBox.Controls.Add(this.outputBox);
            this.stepOptionsBox.Controls.Add(this.inputBox);
            this.stepOptionsBox.Controls.Add(this.stepTypeLabel);
            this.stepOptionsBox.Controls.Add(this.stepTypeBox);
            this.stepOptionsBox.Enabled = false;
            this.stepOptionsBox.Location = new System.Drawing.Point(12, 257);
            this.stepOptionsBox.Name = "stepOptionsBox";
            this.stepOptionsBox.Size = new System.Drawing.Size(416, 103);
            this.stepOptionsBox.TabIndex = 4;
            this.stepOptionsBox.TabStop = false;
            this.stepOptionsBox.Text = "Step Options";
            // 
            // browseOutput
            // 
            this.browseOutput.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.browseOutput.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.browseOutput.Location = new System.Drawing.Point(335, 70);
            this.browseOutput.Name = "browseOutput";
            this.browseOutput.Size = new System.Drawing.Size(75, 23);
            this.browseOutput.TabIndex = 7;
            this.browseOutput.Text = "Browse...";
            this.browseOutput.Click += new System.EventHandler(this.browseOutput_Click);
            // 
            // browseInput
            // 
            this.browseInput.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.browseInput.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.browseInput.Location = new System.Drawing.Point(335, 44);
            this.browseInput.Name = "browseInput";
            this.browseInput.Size = new System.Drawing.Size(75, 23);
            this.browseInput.TabIndex = 7;
            this.browseInput.Text = "Browse...";
            this.browseInput.Click += new System.EventHandler(this.browseInput_Click);
            // 
            // actionLabel
            // 
            this.actionLabel.AutoSize = true;
            this.actionLabel.Location = new System.Drawing.Point(243, 22);
            this.actionLabel.Name = "actionLabel";
            this.actionLabel.Size = new System.Drawing.Size(40, 13);
            this.actionLabel.TabIndex = 6;
            this.actionLabel.Text = "Action:";
            // 
            // actionBox
            // 
            this.actionBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.actionBox.FormattingEnabled = true;
            this.actionBox.Items.AddRange(new object[] {
            "All",
            "Assemble",
            "List",
            "Symbol Table"});
            this.actionBox.Location = new System.Drawing.Point(289, 19);
            this.actionBox.Name = "actionBox";
            this.actionBox.Size = new System.Drawing.Size(104, 21);
            this.actionBox.TabIndex = 5;
            this.actionBox.SelectedIndexChanged += new System.EventHandler(this.actionBox_SelectedIndexChanged);
            // 
            // outputLabel
            // 
            this.outputLabel.AutoSize = true;
            this.outputLabel.Location = new System.Drawing.Point(13, 75);
            this.outputLabel.Name = "outputLabel";
            this.outputLabel.Size = new System.Drawing.Size(61, 13);
            this.outputLabel.TabIndex = 4;
            this.outputLabel.Text = "Output File:";
            this.outputLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // inputLabel
            // 
            this.inputLabel.AutoSize = true;
            this.inputLabel.Location = new System.Drawing.Point(21, 49);
            this.inputLabel.Name = "inputLabel";
            this.inputLabel.Size = new System.Drawing.Size(53, 13);
            this.inputLabel.TabIndex = 4;
            this.inputLabel.Text = "Input File:";
            this.inputLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // outputBox
            // 
            this.outputBox.Location = new System.Drawing.Point(81, 72);
            this.outputBox.Name = "outputBox";
            this.outputBox.Size = new System.Drawing.Size(248, 20);
            this.outputBox.TabIndex = 3;
            this.outputBox.TextChanged += new System.EventHandler(this.outputBox_TextChanged);
            // 
            // inputBox
            // 
            this.inputBox.Location = new System.Drawing.Point(81, 46);
            this.inputBox.Name = "inputBox";
            this.inputBox.Size = new System.Drawing.Size(248, 20);
            this.inputBox.TabIndex = 2;
            this.inputBox.TextChanged += new System.EventHandler(this.inputBox_TextChanged);
            // 
            // stepTypeLabel
            // 
            this.stepTypeLabel.AutoSize = true;
            this.stepTypeLabel.Location = new System.Drawing.Point(16, 22);
            this.stepTypeLabel.Name = "stepTypeLabel";
            this.stepTypeLabel.Size = new System.Drawing.Size(59, 13);
            this.stepTypeLabel.TabIndex = 1;
            this.stepTypeLabel.Text = "Step Type:";
            // 
            // stepTypeBox
            // 
            this.stepTypeBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.stepTypeBox.FormattingEnabled = true;
            this.stepTypeBox.Items.AddRange(new object[] {
            "Internal Build Step",
            "External Build Step"});
            this.stepTypeBox.Location = new System.Drawing.Point(81, 19);
            this.stepTypeBox.Name = "stepTypeBox";
            this.stepTypeBox.Size = new System.Drawing.Size(121, 21);
            this.stepTypeBox.TabIndex = 0;
            this.stepTypeBox.SelectedIndexChanged += new System.EventHandler(this.stepTypeBox_SelectedIndexChanged);
            // 
            // configManagerButton
            // 
            this.configManagerButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.configManagerButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.configManagerButton.Location = new System.Drawing.Point(329, 10);
            this.configManagerButton.Name = "configManagerButton";
            this.configManagerButton.Size = new System.Drawing.Size(102, 23);
            this.configManagerButton.TabIndex = 5;
            this.configManagerButton.Text = "Config Manager";
            // 
            // BuildSteps
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(443, 397);
            this.Controls.Add(this.configManagerButton);
            this.Controls.Add(this.stepOptionsBox);
            this.Controls.Add(this.configLabel);
            this.Controls.Add(this.configBox);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.moveDown);
            this.Controls.Add(this.moveUp);
            this.Controls.Add(this.deleteDirButton);
            this.Controls.Add(this.addDirButton);
            this.Controls.Add(this.buildSeqList);
            this.MinimumSize = new System.Drawing.Size(200, 230);
            this.Name = "BuildSteps";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Build Steps";
            this.stepOptionsBox.ResumeLayout(false);
            this.stepOptionsBox.PerformLayout();
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
        private System.Windows.Forms.Button moveUp;
        private System.Windows.Forms.Button moveDown;
        private System.Windows.Forms.ComboBox configBox;
        private System.Windows.Forms.Label configLabel;
		private System.Windows.Forms.GroupBox stepOptionsBox;
		private System.Windows.Forms.Button configManagerButton;
		private System.Windows.Forms.Label stepTypeLabel;
		private System.Windows.Forms.ComboBox stepTypeBox;
		private System.Windows.Forms.Label outputLabel;
		private System.Windows.Forms.Label inputLabel;
		private System.Windows.Forms.TextBox outputBox;
		private System.Windows.Forms.TextBox inputBox;
		private System.Windows.Forms.Label actionLabel;
		private System.Windows.Forms.ComboBox actionBox;
        private System.Windows.Forms.Button browseOutput;
        private System.Windows.Forms.Button browseInput;
    }
}