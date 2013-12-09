namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class DebugPanel
    {
        public System.Windows.Forms.TextBox pcBox;

        private System.Windows.Forms.TextBox afBox;
        private System.Windows.Forms.Label afLabel;
        private System.Windows.Forms.TextBox afpBox;
        private System.Windows.Forms.Label afpLabel;
        private System.Windows.Forms.TextBox bcBox;
        private System.Windows.Forms.Label bcLabel;
        private System.Windows.Forms.TextBox bcpBox;
        private System.Windows.Forms.Label bcpLabel;
        private System.Windows.Forms.CheckBox cflagBox;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.ContextMenu contextMenu1;
        private System.Windows.Forms.MenuItem copyDebugMenuItem;
        private System.Windows.Forms.TextBox deBox;
        private System.Windows.Forms.Label deLabel;
        private System.Windows.Forms.TextBox depBox;
        private System.Windows.Forms.Label depLabel;
        private System.Windows.Forms.GroupBox flagsBox;
        private System.Windows.Forms.MenuItem gotoSourceMenuItem;
        private System.Windows.Forms.CheckBox hcflagBox;
        private System.Windows.Forms.TextBox hlBox;
        private System.Windows.Forms.Label hlLabel;
        private System.Windows.Forms.TextBox hlpBox;
        private System.Windows.Forms.Label hlpLabel;
        private System.Windows.Forms.TextBox ixBox;
        private System.Windows.Forms.Label ixLabel;
        private System.Windows.Forms.TextBox iyBox;
        private System.Windows.Forms.Label iyLabel;
        private System.Windows.Forms.CheckBox nflagBox;
        private System.Windows.Forms.Panel panel;
        private System.Windows.Forms.MenuItem pasteDebugMenuItem;
        private System.Windows.Forms.Label pcLabel;
        private System.Windows.Forms.CheckBox pvflagBox;
        private System.Windows.Forms.GroupBox registersBox;
        private System.Windows.Forms.GroupBox screenBox;
        private System.Windows.Forms.PictureBox screenPicBox;
        private System.Windows.Forms.CheckBox sflagBox;
        private System.Windows.Forms.TextBox spBox;
        private System.Windows.Forms.Label spLabel;
        private System.Windows.Forms.CheckBox zflagBox;

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
            this.afLabel = new System.Windows.Forms.Label();
            this.bcLabel = new System.Windows.Forms.Label();
            this.hlLabel = new System.Windows.Forms.Label();
            this.deLabel = new System.Windows.Forms.Label();
            this.ixLabel = new System.Windows.Forms.Label();
            this.iyLabel = new System.Windows.Forms.Label();
            this.depLabel = new System.Windows.Forms.Label();
            this.hlpLabel = new System.Windows.Forms.Label();
            this.bcpLabel = new System.Windows.Forms.Label();
            this.afpLabel = new System.Windows.Forms.Label();
            this.spLabel = new System.Windows.Forms.Label();
            this.pcLabel = new System.Windows.Forms.Label();
            this.afBox = new System.Windows.Forms.TextBox();
            this.bcBox = new System.Windows.Forms.TextBox();
            this.deBox = new System.Windows.Forms.TextBox();
            this.hlBox = new System.Windows.Forms.TextBox();
            this.ixBox = new System.Windows.Forms.TextBox();
            this.iyBox = new System.Windows.Forms.TextBox();
            this.afpBox = new System.Windows.Forms.TextBox();
            this.bcpBox = new System.Windows.Forms.TextBox();
            this.depBox = new System.Windows.Forms.TextBox();
            this.hlpBox = new System.Windows.Forms.TextBox();
            this.spBox = new System.Windows.Forms.TextBox();
            this.pcBox = new System.Windows.Forms.TextBox();
            this.zflagBox = new System.Windows.Forms.CheckBox();
            this.sflagBox = new System.Windows.Forms.CheckBox();
            this.cflagBox = new System.Windows.Forms.CheckBox();
            this.pvflagBox = new System.Windows.Forms.CheckBox();
            this.nflagBox = new System.Windows.Forms.CheckBox();
            this.hcflagBox = new System.Windows.Forms.CheckBox();
            this.panel = new System.Windows.Forms.Panel();
            this.screenBox = new System.Windows.Forms.GroupBox();
            this.screenPicBox = new System.Windows.Forms.PictureBox();
            this.flagsBox = new System.Windows.Forms.GroupBox();
            this.registersBox = new System.Windows.Forms.GroupBox();
            this.contextMenu1 = new System.Windows.Forms.ContextMenu();
            this.copyDebugMenuItem = new System.Windows.Forms.MenuItem();
            this.pasteDebugMenuItem = new System.Windows.Forms.MenuItem();
            this.gotoSourceMenuItem = new System.Windows.Forms.MenuItem();
            this.panel.SuspendLayout();
            this.screenBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)this.screenPicBox).BeginInit();
            this.flagsBox.SuspendLayout();
            this.registersBox.SuspendLayout();
            this.SuspendLayout();

            // afLabel

            this.afLabel.AutoSize = true;
            this.afLabel.Location = new System.Drawing.Point(15, 21);
            this.afLabel.Name = "afLabel";
            this.afLabel.Size = new System.Drawing.Size(20, 13);
            this.afLabel.TabIndex = 4;
            this.afLabel.Text = "AF";

            // bcLabel

            this.bcLabel.AutoSize = true;
            this.bcLabel.Location = new System.Drawing.Point(15, 45);
            this.bcLabel.Name = "bcLabel";
            this.bcLabel.Size = new System.Drawing.Size(21, 13);
            this.bcLabel.TabIndex = 5;
            this.bcLabel.Text = "BC";

            // hlLabel

            this.hlLabel.AutoSize = true;
            this.hlLabel.Location = new System.Drawing.Point(15, 91);
            this.hlLabel.Name = "hlLabel";
            this.hlLabel.Size = new System.Drawing.Size(21, 13);
            this.hlLabel.TabIndex = 2;
            this.hlLabel.Text = "HL";

            // deLabel

            this.deLabel.AutoSize = true;
            this.deLabel.Location = new System.Drawing.Point(15, 68);
            this.deLabel.Name = "deLabel";
            this.deLabel.Size = new System.Drawing.Size(22, 13);
            this.deLabel.TabIndex = 3;
            this.deLabel.Text = "DE";

            // ixLabel

            this.ixLabel.AutoSize = true;
            this.ixLabel.Location = new System.Drawing.Point(15, 115);
            this.ixLabel.Name = "ixLabel";
            this.ixLabel.Size = new System.Drawing.Size(17, 13);
            this.ixLabel.TabIndex = 6;
            this.ixLabel.Text = "IX";

            // iyLabel

            this.iyLabel.AutoSize = true;
            this.iyLabel.Location = new System.Drawing.Point(15, 139);
            this.iyLabel.Name = "iyLabel";
            this.iyLabel.Size = new System.Drawing.Size(17, 13);
            this.iyLabel.TabIndex = 6;
            this.iyLabel.Text = "IY";

            // depLabel

            this.depLabel.AutoSize = true;
            this.depLabel.Location = new System.Drawing.Point(116, 69);
            this.depLabel.Name = "depLabel";
            this.depLabel.Size = new System.Drawing.Size(24, 13);
            this.depLabel.TabIndex = 3;
            this.depLabel.Text = "DE\'";

            // hlpLabel

            this.hlpLabel.AutoSize = true;
            this.hlpLabel.Location = new System.Drawing.Point(119, 91);
            this.hlpLabel.Name = "hlpLabel";
            this.hlpLabel.Size = new System.Drawing.Size(23, 13);
            this.hlpLabel.TabIndex = 2;
            this.hlpLabel.Text = "HL\'";

            // bcpLabel

            this.bcpLabel.AutoSize = true;
            this.bcpLabel.Location = new System.Drawing.Point(119, 47);
            this.bcpLabel.Name = "bcpLabel";
            this.bcpLabel.Size = new System.Drawing.Size(23, 13);
            this.bcpLabel.TabIndex = 5;
            this.bcpLabel.Text = "BC\'";

            // afpLabel

            this.afpLabel.AutoSize = true;
            this.afpLabel.Location = new System.Drawing.Point(120, 26);
            this.afpLabel.Name = "afpLabel";
            this.afpLabel.Size = new System.Drawing.Size(22, 13);
            this.afpLabel.TabIndex = 4;
            this.afpLabel.Text = "AF\'";

            // spLabel

            this.spLabel.AutoSize = true;
            this.spLabel.Location = new System.Drawing.Point(120, 116);
            this.spLabel.Name = "spLabel";
            this.spLabel.Size = new System.Drawing.Size(21, 13);
            this.spLabel.TabIndex = 6;
            this.spLabel.Text = "SP";

            // pcLabel

            this.pcLabel.AutoSize = true;
            this.pcLabel.Location = new System.Drawing.Point(119, 139);
            this.pcLabel.Name = "pcLabel";
            this.pcLabel.Size = new System.Drawing.Size(21, 13);
            this.pcLabel.TabIndex = 6;
            this.pcLabel.Text = "PC";

            // afBox

            this.afBox.Location = new System.Drawing.Point(41, 18);
            this.afBox.MaxLength = 4;
            this.afBox.Name = "afBox";
            this.afBox.Size = new System.Drawing.Size(61, 20);
            this.afBox.TabIndex = 7;
            this.afBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.afBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // bcBox

            this.bcBox.Location = new System.Drawing.Point(41, 42);
            this.bcBox.MaxLength = 4;
            this.bcBox.Name = "bcBox";
            this.bcBox.Size = new System.Drawing.Size(61, 20);
            this.bcBox.TabIndex = 7;
            this.bcBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.bcBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // deBox

            this.deBox.Location = new System.Drawing.Point(41, 65);
            this.deBox.MaxLength = 4;
            this.deBox.Name = "deBox";
            this.deBox.Size = new System.Drawing.Size(61, 20);
            this.deBox.TabIndex = 7;
            this.deBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.deBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // hlBox

            this.hlBox.Location = new System.Drawing.Point(41, 88);
            this.hlBox.MaxLength = 4;
            this.hlBox.Name = "hlBox";
            this.hlBox.Size = new System.Drawing.Size(61, 20);
            this.hlBox.TabIndex = 7;
            this.hlBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.hlBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // ixBox

            this.ixBox.Location = new System.Drawing.Point(41, 112);
            this.ixBox.MaxLength = 4;
            this.ixBox.Name = "ixBox";
            this.ixBox.Size = new System.Drawing.Size(61, 20);
            this.ixBox.TabIndex = 7;
            this.ixBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.ixBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // iyBox

            this.iyBox.Location = new System.Drawing.Point(41, 136);
            this.iyBox.MaxLength = 4;
            this.iyBox.Name = "iyBox";
            this.iyBox.Size = new System.Drawing.Size(61, 20);
            this.iyBox.TabIndex = 7;
            this.iyBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.iyBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // afpBox

            this.afpBox.Location = new System.Drawing.Point(148, 19);
            this.afpBox.MaxLength = 4;
            this.afpBox.Name = "afpBox";
            this.afpBox.Size = new System.Drawing.Size(61, 20);
            this.afpBox.TabIndex = 7;
            this.afpBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.afpBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // bcpBox

            this.bcpBox.Location = new System.Drawing.Point(148, 43);
            this.bcpBox.MaxLength = 4;
            this.bcpBox.Name = "bcpBox";
            this.bcpBox.Size = new System.Drawing.Size(61, 20);
            this.bcpBox.TabIndex = 7;
            this.bcpBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.bcpBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // depBox

            this.depBox.Location = new System.Drawing.Point(148, 66);
            this.depBox.MaxLength = 4;
            this.depBox.Name = "depBox";
            this.depBox.Size = new System.Drawing.Size(61, 20);
            this.depBox.TabIndex = 7;
            this.depBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.depBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // hlpBox

            this.hlpBox.Location = new System.Drawing.Point(148, 89);
            this.hlpBox.MaxLength = 4;
            this.hlpBox.Name = "hlpBox";
            this.hlpBox.Size = new System.Drawing.Size(61, 20);
            this.hlpBox.TabIndex = 7;
            this.hlpBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.hlpBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // spBox

            this.spBox.Location = new System.Drawing.Point(148, 113);
            this.spBox.MaxLength = 4;
            this.spBox.Name = "spBox";
            this.spBox.Size = new System.Drawing.Size(61, 20);
            this.spBox.TabIndex = 7;
            this.spBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.spBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // pcBox

            this.pcBox.Location = new System.Drawing.Point(148, 137);
            this.pcBox.MaxLength = 4;
            this.pcBox.Name = "pcBox";
            this.pcBox.Size = new System.Drawing.Size(61, 20);
            this.pcBox.TabIndex = 7;
            this.pcBox.TextChanged += new System.EventHandler(this.registersBox_TextChanged);
            this.pcBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.RegisterBox_KeyPress);

            // zflagBox

            this.zflagBox.AutoSize = true;
            this.zflagBox.Location = new System.Drawing.Point(18, 19);
            this.zflagBox.Name = "zflagBox";
            this.zflagBox.Size = new System.Drawing.Size(31, 17);
            this.zflagBox.TabIndex = 0;
            this.zflagBox.Text = "z";
            this.zflagBox.UseVisualStyleBackColor = true;
            this.zflagBox.CheckedChanged += new System.EventHandler(this.zflagBox_CheckedChanged);

            // sflagBox

            this.sflagBox.AutoSize = true;
            this.sflagBox.Location = new System.Drawing.Point(169, 19);
            this.sflagBox.Name = "sflagBox";
            this.sflagBox.Size = new System.Drawing.Size(31, 17);
            this.sflagBox.TabIndex = 0;
            this.sflagBox.Text = "s";
            this.sflagBox.UseVisualStyleBackColor = true;
            this.sflagBox.CheckedChanged += new System.EventHandler(this.zflagBox_CheckedChanged);

            // cflagBox

            this.cflagBox.AutoSize = true;
            this.cflagBox.Location = new System.Drawing.Point(94, 19);
            this.cflagBox.Name = "cflagBox";
            this.cflagBox.Size = new System.Drawing.Size(32, 17);
            this.cflagBox.TabIndex = 0;
            this.cflagBox.Text = "c";
            this.cflagBox.UseVisualStyleBackColor = true;
            this.cflagBox.CheckedChanged += new System.EventHandler(this.zflagBox_CheckedChanged);

            // pvflagBox

            this.pvflagBox.AutoSize = true;
            this.pvflagBox.Location = new System.Drawing.Point(18, 42);
            this.pvflagBox.Name = "pvflagBox";
            this.pvflagBox.Size = new System.Drawing.Size(43, 17);
            this.pvflagBox.TabIndex = 0;
            this.pvflagBox.Text = "p/v";
            this.pvflagBox.UseVisualStyleBackColor = true;
            this.pvflagBox.CheckedChanged += new System.EventHandler(this.zflagBox_CheckedChanged);

            // nflagBox

            this.nflagBox.AutoSize = true;
            this.nflagBox.Location = new System.Drawing.Point(169, 42);
            this.nflagBox.Name = "nflagBox";
            this.nflagBox.Size = new System.Drawing.Size(32, 17);
            this.nflagBox.TabIndex = 0;
            this.nflagBox.Text = "n";
            this.nflagBox.UseVisualStyleBackColor = true;
            this.nflagBox.CheckedChanged += new System.EventHandler(this.zflagBox_CheckedChanged);

            // hcflagBox

            this.hcflagBox.AutoSize = true;
            this.hcflagBox.Location = new System.Drawing.Point(94, 42);
            this.hcflagBox.Name = "hcflagBox";
            this.hcflagBox.Size = new System.Drawing.Size(38, 17);
            this.hcflagBox.TabIndex = 0;
            this.hcflagBox.Text = "hc";
            this.hcflagBox.UseVisualStyleBackColor = true;
            this.hcflagBox.CheckedChanged += new System.EventHandler(this.zflagBox_CheckedChanged);

            // panel

            this.panel.AutoScroll = true;
            this.panel.Controls.Add(this.screenBox);
            this.panel.Controls.Add(this.flagsBox);
            this.panel.Controls.Add(this.registersBox);
            this.panel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel.Location = new System.Drawing.Point(0, 0);
            this.panel.Name = "panel";
            this.panel.Size = new System.Drawing.Size(258, 433);
            this.panel.TabIndex = 0;

            // screenBox

            this.screenBox.Controls.Add(this.screenPicBox);
            this.screenBox.Location = new System.Drawing.Point(12, 261);
            this.screenBox.Name = "screenBox";
            this.screenBox.Size = new System.Drawing.Size(227, 158);
            this.screenBox.TabIndex = 2;
            this.screenBox.TabStop = false;
            this.screenBox.Text = "Screen";

            // screenPicBox

            this.screenPicBox.Location = new System.Drawing.Point(17, 17);
            this.screenPicBox.Name = "screenPicBox";
            this.screenPicBox.Size = new System.Drawing.Size(192, 128);
            this.screenPicBox.TabIndex = 0;
            this.screenPicBox.TabStop = false;

            // flagsBox

            this.flagsBox.Controls.Add(this.zflagBox);
            this.flagsBox.Controls.Add(this.cflagBox);
            this.flagsBox.Controls.Add(this.sflagBox);
            this.flagsBox.Controls.Add(this.pvflagBox);
            this.flagsBox.Controls.Add(this.hcflagBox);
            this.flagsBox.Controls.Add(this.nflagBox);
            this.flagsBox.Location = new System.Drawing.Point(12, 186);
            this.flagsBox.Name = "flagsBox";
            this.flagsBox.Size = new System.Drawing.Size(227, 69);
            this.flagsBox.TabIndex = 1;
            this.flagsBox.TabStop = false;
            this.flagsBox.Text = "Flags";

            // registersBox

            this.registersBox.Controls.Add(this.afLabel);
            this.registersBox.Controls.Add(this.afBox);
            this.registersBox.Controls.Add(this.afpLabel);
            this.registersBox.Controls.Add(this.afpBox);
            this.registersBox.Controls.Add(this.bcLabel);
            this.registersBox.Controls.Add(this.bcBox);
            this.registersBox.Controls.Add(this.bcpLabel);
            this.registersBox.Controls.Add(this.bcpBox);
            this.registersBox.Controls.Add(this.deLabel);
            this.registersBox.Controls.Add(this.deBox);
            this.registersBox.Controls.Add(this.depLabel);
            this.registersBox.Controls.Add(this.depBox);
            this.registersBox.Controls.Add(this.hlLabel);
            this.registersBox.Controls.Add(this.hlBox);
            this.registersBox.Controls.Add(this.hlpLabel);
            this.registersBox.Controls.Add(this.hlpBox);
            this.registersBox.Controls.Add(this.ixBox);
            this.registersBox.Controls.Add(this.ixLabel);
            this.registersBox.Controls.Add(this.iyLabel);
            this.registersBox.Controls.Add(this.iyBox);
            this.registersBox.Controls.Add(this.spLabel);
            this.registersBox.Controls.Add(this.spBox);
            this.registersBox.Controls.Add(this.pcLabel);
            this.registersBox.Controls.Add(this.pcBox);
            this.registersBox.Location = new System.Drawing.Point(12, 12);
            this.registersBox.Name = "registersBox";
            this.registersBox.Size = new System.Drawing.Size(227, 168);
            this.registersBox.TabIndex = 0;
            this.registersBox.TabStop = false;
            this.registersBox.Text = "Registers";

            // contextMenu1

            this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
            {
                this.copyDebugMenuItem,
                this.pasteDebugMenuItem,
                this.gotoSourceMenuItem
            });

            // copyDebugMenuItem

            this.copyDebugMenuItem.Index = 0;
            this.copyDebugMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlC;
            this.copyDebugMenuItem.Text = "Copy";
            this.copyDebugMenuItem.Click += new System.EventHandler(this.Copy);

            // pasteDebugMenuItem

            this.pasteDebugMenuItem.Index = 1;
            this.pasteDebugMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlV;
            this.pasteDebugMenuItem.Text = "Paste";
            this.pasteDebugMenuItem.Click += new System.EventHandler(this.Paste);

            // menuItem3

            this.gotoSourceMenuItem.Index = 2;
            this.gotoSourceMenuItem.Text = "Goto Source Line";
            this.gotoSourceMenuItem.Click += new System.EventHandler(gotoSourceMenuItem_Click);

            // DebugPanel

            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(258, 433);
            this.Controls.Add(this.panel);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (byte)0);
            this.HideOnClose = true;
            this.MinimumSize = new System.Drawing.Size(266, 38);
            this.Name = "DebugPanel";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockRight;
            this.TabText = "Debug";
            this.Text = "Debug";
            this.panel.ResumeLayout(false);
            this.screenBox.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)this.screenPicBox).EndInit();
            this.flagsBox.ResumeLayout(false);
            this.flagsBox.PerformLayout();
            this.registersBox.ResumeLayout(false);
            this.registersBox.PerformLayout();
            this.ResumeLayout(false);
        }
    }
}