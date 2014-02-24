namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class MacroManager
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.ToolStripButton deleteMacroButton;
        private System.Windows.Forms.ListBox macroBox;
        private System.Windows.Forms.ToolStripButton newMacroButton;
        private System.Windows.Forms.ToolStripButton runMacroButton;
        private System.Windows.Forms.ToolStrip toolStrip1;

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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MacroManager));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.newMacroButton = new System.Windows.Forms.ToolStripButton();
            this.deleteMacroButton = new System.Windows.Forms.ToolStripButton();
            this.runMacroButton = new System.Windows.Forms.ToolStripButton();
            this.macroBox = new System.Windows.Forms.ListBox();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();

            // toolStrip1

            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[]
            {
                this.newMacroButton,
                this.deleteMacroButton,
                this.runMacroButton
            });
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(292, 25);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";

            // newMacroButton

            this.newMacroButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.newMacroButton.Image = (System.Drawing.Image)resources.GetObject("newMacroButton.Image");
            this.newMacroButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newMacroButton.Name = "newMacroButton";
            this.newMacroButton.Size = new System.Drawing.Size(35, 22);
            this.newMacroButton.Text = "New";
            this.newMacroButton.Click += new System.EventHandler(this.newMacroButton_Click);

            // deleteMacroButton

            this.deleteMacroButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.deleteMacroButton.Image = (System.Drawing.Image)resources.GetObject("deleteMacroButton.Image");
            this.deleteMacroButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.deleteMacroButton.Name = "deleteMacroButton";
            this.deleteMacroButton.Size = new System.Drawing.Size(44, 22);
            this.deleteMacroButton.Text = "Delete";
            this.deleteMacroButton.Click += new System.EventHandler(this.deleteMacroButton_Click);

            // runMacroButton

            this.runMacroButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.runMacroButton.Image = (System.Drawing.Image)resources.GetObject("runMacroButton.Image");
            this.runMacroButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.runMacroButton.Name = "runMacroButton";
            this.runMacroButton.Size = new System.Drawing.Size(32, 22);
            this.runMacroButton.Text = "Run";
            this.runMacroButton.Click += new System.EventHandler(this.runMacroButton_Click);

            // macroBox

            this.macroBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.macroBox.FormattingEnabled = true;
            this.macroBox.Location = new System.Drawing.Point(0, 25);
            this.macroBox.Name = "macroBox";
            this.macroBox.Size = new System.Drawing.Size(292, 241);
            this.macroBox.TabIndex = 1;

            // MacroManager

            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.macroBox);
            this.Controls.Add(this.toolStrip1);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (byte)0);
            this.HideOnClose = true;
            this.Name = "MacroManager";
            this.Text = WindowIdentifier;
            this.TabText = WindowIdentifier;
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();
        }
    }
}