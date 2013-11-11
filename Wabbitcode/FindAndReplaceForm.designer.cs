namespace Revsoft.Wabbitcode
{
    public partial class FindAndReplaceForm
    {
        private System.Windows.Forms.Button findNextFindButton;
        private System.Windows.Forms.Button findPrevFindButton;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox findFindBox;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
                _search.Dispose();
            }

            base.Dispose(disposing);
        }

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.findFindBox = new System.Windows.Forms.TextBox();
            this.findNextFindButton = new System.Windows.Forms.Button();
            this.findPrevFindButton = new System.Windows.Forms.Button();
            this.findTabs = new System.Windows.Forms.TabControl();
            this.findPage = new System.Windows.Forms.TabPage();
            this.matchCaseFindCheckbox = new System.Windows.Forms.CheckBox();
            this.matchWholeWordFindCheckbox = new System.Windows.Forms.CheckBox();
            this.replacePage = new System.Windows.Forms.TabPage();
            this.label2 = new System.Windows.Forms.Label();
            this.matchCaseReplaceCheckbox = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.matchWholeWordReplaceCheckbox = new System.Windows.Forms.CheckBox();
            this.replaceFindBox = new System.Windows.Forms.TextBox();
            this.replaceReplaceBox = new System.Windows.Forms.TextBox();
            this.findNextReplaceButton = new System.Windows.Forms.Button();
            this.findPrevReplaceButton = new System.Windows.Forms.Button();
            this.replaceButton = new System.Windows.Forms.Button();
            this.replaceAllButton = new System.Windows.Forms.Button();
            this.findInFilesPage = new System.Windows.Forms.TabPage();
            this.label4 = new System.Windows.Forms.Label();
            this.matchCaseFilesCheckbox = new System.Windows.Forms.CheckBox();
            this.label5 = new System.Windows.Forms.Label();
            this.matchWholeWordFilesCheckbox = new System.Windows.Forms.CheckBox();
            this.findFilesBox = new System.Windows.Forms.TextBox();
            this.replaceFilesBox = new System.Windows.Forms.TextBox();
            this.findInFilesButton = new System.Windows.Forms.Button();
            this.replaceInFilesButton = new System.Windows.Forms.Button();
            this.findTabs.SuspendLayout();
            this.findPage.SuspendLayout();
            this.replacePage.SuspendLayout();
            this.findInFilesPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(56, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Fi&nd what:";
            // 
            // findFindBox
            // 
            this.findFindBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.findFindBox.Location = new System.Drawing.Point(85, 6);
            this.findFindBox.Name = "findFindBox";
            this.findFindBox.Size = new System.Drawing.Size(276, 20);
            this.findFindBox.TabIndex = 1;
            this.findFindBox.TextChanged += new System.EventHandler(this.txtLookFor_TextChanged);
            // 
            // findNextFindButton
            // 
            this.findNextFindButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.findNextFindButton.Location = new System.Drawing.Point(286, 80);
            this.findNextFindButton.Name = "findNextFindButton";
            this.findNextFindButton.Size = new System.Drawing.Size(75, 23);
            this.findNextFindButton.TabIndex = 6;
            this.findNextFindButton.Text = "&Find next";
            this.findNextFindButton.Click += new System.EventHandler(this.findNextFindButton_Click);
            // 
            // findPrevFindButton
            // 
            this.findPrevFindButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.findPrevFindButton.Location = new System.Drawing.Point(196, 80);
            this.findPrevFindButton.Name = "findPrevFindButton";
            this.findPrevFindButton.Size = new System.Drawing.Size(84, 23);
            this.findPrevFindButton.TabIndex = 6;
            this.findPrevFindButton.Text = "Find pre&vious";
            this.findPrevFindButton.Click += new System.EventHandler(this.findPrevFindButton_Click);
            // 
            // findTabs
            // 
            this.findTabs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.findTabs.Controls.Add(this.findPage);
            this.findTabs.Controls.Add(this.replacePage);
            this.findTabs.Controls.Add(this.findInFilesPage);
            this.findTabs.Location = new System.Drawing.Point(9, 9);
            this.findTabs.Margin = new System.Windows.Forms.Padding(0);
            this.findTabs.Name = "findTabs";
            this.findTabs.SelectedIndex = 0;
            this.findTabs.Size = new System.Drawing.Size(375, 135);
            this.findTabs.TabIndex = 10;
            this.findTabs.SelectedIndexChanged += new System.EventHandler(this.findTabs_SelectedIndexChanged);
            // 
            // findPage
            // 
            this.findPage.Controls.Add(this.matchCaseFindCheckbox);
            this.findPage.Controls.Add(this.matchWholeWordFindCheckbox);
            this.findPage.Controls.Add(this.label1);
            this.findPage.Controls.Add(this.findFindBox);
            this.findPage.Controls.Add(this.findNextFindButton);
            this.findPage.Controls.Add(this.findPrevFindButton);
            this.findPage.Location = new System.Drawing.Point(4, 22);
            this.findPage.Name = "findPage";
            this.findPage.Padding = new System.Windows.Forms.Padding(3);
            this.findPage.Size = new System.Drawing.Size(367, 109);
            this.findPage.TabIndex = 0;
            this.findPage.Text = "Find";
            this.findPage.UseVisualStyleBackColor = true;
            // 
            // matchCaseFindCheckbox
            // 
            this.matchCaseFindCheckbox.AutoSize = true;
            this.matchCaseFindCheckbox.Location = new System.Drawing.Point(85, 32);
            this.matchCaseFindCheckbox.Name = "matchCaseFindCheckbox";
            this.matchCaseFindCheckbox.Size = new System.Drawing.Size(82, 17);
            this.matchCaseFindCheckbox.TabIndex = 19;
            this.matchCaseFindCheckbox.Text = "Match &case";
            // 
            // matchWholeWordFindCheckbox
            // 
            this.matchWholeWordFindCheckbox.AutoSize = true;
            this.matchWholeWordFindCheckbox.Location = new System.Drawing.Point(173, 32);
            this.matchWholeWordFindCheckbox.Name = "matchWholeWordFindCheckbox";
            this.matchWholeWordFindCheckbox.Size = new System.Drawing.Size(113, 17);
            this.matchWholeWordFindCheckbox.TabIndex = 20;
            this.matchWholeWordFindCheckbox.Text = "Match &whole word";
            // 
            // replacePage
            // 
            this.replacePage.Controls.Add(this.label2);
            this.replacePage.Controls.Add(this.matchCaseReplaceCheckbox);
            this.replacePage.Controls.Add(this.label3);
            this.replacePage.Controls.Add(this.matchWholeWordReplaceCheckbox);
            this.replacePage.Controls.Add(this.replaceFindBox);
            this.replacePage.Controls.Add(this.replaceReplaceBox);
            this.replacePage.Controls.Add(this.findNextReplaceButton);
            this.replacePage.Controls.Add(this.findPrevReplaceButton);
            this.replacePage.Controls.Add(this.replaceButton);
            this.replacePage.Controls.Add(this.replaceAllButton);
            this.replacePage.Location = new System.Drawing.Point(4, 22);
            this.replacePage.Name = "replacePage";
            this.replacePage.Padding = new System.Windows.Forms.Padding(3);
            this.replacePage.Size = new System.Drawing.Size(367, 109);
            this.replacePage.TabIndex = 1;
            this.replacePage.Text = "Replace";
            this.replacePage.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(56, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Fi&nd what:";
            // 
            // matchCaseReplaceCheckbox
            // 
            this.matchCaseReplaceCheckbox.AutoSize = true;
            this.matchCaseReplaceCheckbox.Location = new System.Drawing.Point(86, 58);
            this.matchCaseReplaceCheckbox.Name = "matchCaseReplaceCheckbox";
            this.matchCaseReplaceCheckbox.Size = new System.Drawing.Size(82, 17);
            this.matchCaseReplaceCheckbox.TabIndex = 17;
            this.matchCaseReplaceCheckbox.Text = "Match &case";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(8, 35);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(72, 13);
            this.label3.TabIndex = 15;
            this.label3.Text = "Re&place with:";
            // 
            // matchWholeWordReplaceCheckbox
            // 
            this.matchWholeWordReplaceCheckbox.AutoSize = true;
            this.matchWholeWordReplaceCheckbox.Location = new System.Drawing.Point(174, 58);
            this.matchWholeWordReplaceCheckbox.Name = "matchWholeWordReplaceCheckbox";
            this.matchWholeWordReplaceCheckbox.Size = new System.Drawing.Size(113, 17);
            this.matchWholeWordReplaceCheckbox.TabIndex = 18;
            this.matchWholeWordReplaceCheckbox.Text = "Match &whole word";
            // 
            // replaceFindBox
            // 
            this.replaceFindBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.replaceFindBox.Location = new System.Drawing.Point(87, 6);
            this.replaceFindBox.Name = "replaceFindBox";
            this.replaceFindBox.Size = new System.Drawing.Size(274, 20);
            this.replaceFindBox.TabIndex = 14;
            // 
            // replaceReplaceBox
            // 
            this.replaceReplaceBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.replaceReplaceBox.Location = new System.Drawing.Point(87, 32);
            this.replaceReplaceBox.Name = "replaceReplaceBox";
            this.replaceReplaceBox.Size = new System.Drawing.Size(274, 20);
            this.replaceReplaceBox.TabIndex = 16;
            // 
            // findNextReplaceButton
            // 
            this.findNextReplaceButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.findNextReplaceButton.Location = new System.Drawing.Point(286, 80);
            this.findNextReplaceButton.Name = "findNextReplaceButton";
            this.findNextReplaceButton.Size = new System.Drawing.Size(75, 23);
            this.findNextReplaceButton.TabIndex = 19;
            this.findNextReplaceButton.Text = "&Find next";
            // 
            // findPrevReplaceButton
            // 
            this.findPrevReplaceButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.findPrevReplaceButton.Location = new System.Drawing.Point(196, 80);
            this.findPrevReplaceButton.Name = "findPrevReplaceButton";
            this.findPrevReplaceButton.Size = new System.Drawing.Size(84, 23);
            this.findPrevReplaceButton.TabIndex = 20;
            this.findPrevReplaceButton.Text = "Find pre&vious";
            // 
            // replaceButton
            // 
            this.replaceButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.replaceButton.Location = new System.Drawing.Point(115, 80);
            this.replaceButton.Name = "replaceButton";
            this.replaceButton.Size = new System.Drawing.Size(75, 23);
            this.replaceButton.TabIndex = 12;
            this.replaceButton.Text = "&Replace";
            this.replaceButton.Click += new System.EventHandler(this.replaceButton_Click);
            // 
            // replaceAllButton
            // 
            this.replaceAllButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.replaceAllButton.Location = new System.Drawing.Point(34, 80);
            this.replaceAllButton.Name = "replaceAllButton";
            this.replaceAllButton.Size = new System.Drawing.Size(75, 23);
            this.replaceAllButton.TabIndex = 11;
            this.replaceAllButton.Text = "Replace &all";
            this.replaceAllButton.Click += new System.EventHandler(this.replaceAllButton_Click);
            // 
            // findInFilesPage
            // 
            this.findInFilesPage.Controls.Add(this.label4);
            this.findInFilesPage.Controls.Add(this.matchCaseFilesCheckbox);
            this.findInFilesPage.Controls.Add(this.label5);
            this.findInFilesPage.Controls.Add(this.matchWholeWordFilesCheckbox);
            this.findInFilesPage.Controls.Add(this.findFilesBox);
            this.findInFilesPage.Controls.Add(this.replaceFilesBox);
            this.findInFilesPage.Controls.Add(this.findInFilesButton);
            this.findInFilesPage.Controls.Add(this.replaceInFilesButton);
            this.findInFilesPage.Location = new System.Drawing.Point(4, 22);
            this.findInFilesPage.Name = "findInFilesPage";
            this.findInFilesPage.Padding = new System.Windows.Forms.Padding(3);
            this.findInFilesPage.Size = new System.Drawing.Size(367, 109);
            this.findInFilesPage.TabIndex = 2;
            this.findInFilesPage.Text = "Find in files";
            this.findInFilesPage.UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(56, 13);
            this.label4.TabIndex = 23;
            this.label4.Text = "Fi&nd what:";
            // 
            // matchCaseFilesCheckbox
            // 
            this.matchCaseFilesCheckbox.AutoSize = true;
            this.matchCaseFilesCheckbox.Location = new System.Drawing.Point(86, 58);
            this.matchCaseFilesCheckbox.Name = "matchCaseFilesCheckbox";
            this.matchCaseFilesCheckbox.Size = new System.Drawing.Size(82, 17);
            this.matchCaseFilesCheckbox.TabIndex = 27;
            this.matchCaseFilesCheckbox.Text = "Match &case";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(8, 35);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(72, 13);
            this.label5.TabIndex = 25;
            this.label5.Text = "Re&place with:";
            // 
            // matchWholeWordFilesCheckbox
            // 
            this.matchWholeWordFilesCheckbox.AutoSize = true;
            this.matchWholeWordFilesCheckbox.Location = new System.Drawing.Point(174, 58);
            this.matchWholeWordFilesCheckbox.Name = "matchWholeWordFilesCheckbox";
            this.matchWholeWordFilesCheckbox.Size = new System.Drawing.Size(113, 17);
            this.matchWholeWordFilesCheckbox.TabIndex = 28;
            this.matchWholeWordFilesCheckbox.Text = "Match &whole word";
            // 
            // findFilesBox
            // 
            this.findFilesBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.findFilesBox.Location = new System.Drawing.Point(87, 6);
            this.findFilesBox.Name = "findFilesBox";
            this.findFilesBox.Size = new System.Drawing.Size(274, 20);
            this.findFilesBox.TabIndex = 24;
            // 
            // replaceFilesBox
            // 
            this.replaceFilesBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.replaceFilesBox.Location = new System.Drawing.Point(87, 32);
            this.replaceFilesBox.Name = "replaceFilesBox";
            this.replaceFilesBox.Size = new System.Drawing.Size(274, 20);
            this.replaceFilesBox.TabIndex = 26;
            // 
            // findInFilesButton
            // 
            this.findInFilesButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.findInFilesButton.Location = new System.Drawing.Point(268, 80);
            this.findInFilesButton.Name = "findInFilesButton";
            this.findInFilesButton.Size = new System.Drawing.Size(93, 23);
            this.findInFilesButton.TabIndex = 29;
            this.findInFilesButton.Text = "&Find in files";
            this.findInFilesButton.Click += new System.EventHandler(this.findInFilesButton_Click);
            // 
            // replaceInFilesButton
            // 
            this.replaceInFilesButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.replaceInFilesButton.Location = new System.Drawing.Point(169, 80);
            this.replaceInFilesButton.Name = "replaceInFilesButton";
            this.replaceInFilesButton.Size = new System.Drawing.Size(93, 23);
            this.replaceInFilesButton.TabIndex = 22;
            this.replaceInFilesButton.Text = "&Replace in files";
            this.replaceInFilesButton.Visible = false;
            // 
            // FindAndReplaceForm
            // 
            this.AcceptButton = this.findNextFindButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(393, 153);
            this.Controls.Add(this.findTabs);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FindAndReplaceForm";
            this.ShowIcon = false;
            this.Text = "Find";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FindAndReplaceForm_FormClosing);
            this.findTabs.ResumeLayout(false);
            this.findPage.ResumeLayout(false);
            this.findPage.PerformLayout();
            this.replacePage.ResumeLayout(false);
            this.replacePage.PerformLayout();
            this.findInFilesPage.ResumeLayout(false);
            this.findInFilesPage.PerformLayout();
            this.ResumeLayout(false);

        }

        private System.Windows.Forms.TabControl findTabs;
        private System.Windows.Forms.TabPage findPage;
        private System.Windows.Forms.TabPage replacePage;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox matchCaseReplaceCheckbox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox matchWholeWordReplaceCheckbox;
        private System.Windows.Forms.TextBox replaceFindBox;
        private System.Windows.Forms.TextBox replaceReplaceBox;
        private System.Windows.Forms.Button findNextReplaceButton;
        private System.Windows.Forms.Button findPrevReplaceButton;
        private System.Windows.Forms.Button replaceButton;
        private System.Windows.Forms.Button replaceAllButton;
        private System.Windows.Forms.TabPage findInFilesPage;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox matchCaseFilesCheckbox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox matchWholeWordFilesCheckbox;
        private System.Windows.Forms.TextBox findFilesBox;
        private System.Windows.Forms.TextBox replaceFilesBox;
        private System.Windows.Forms.Button findInFilesButton;
        private System.Windows.Forms.Button replaceInFilesButton;
        private System.Windows.Forms.CheckBox matchCaseFindCheckbox;
        private System.Windows.Forms.CheckBox matchWholeWordFindCheckbox;
    }
}