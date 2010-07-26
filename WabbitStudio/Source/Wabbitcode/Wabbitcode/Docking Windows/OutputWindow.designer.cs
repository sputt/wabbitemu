namespace Revsoft.Wabbitcode.Docking_Windows
{
    partial class OutputWindow
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(OutputWindow));
            this.outputWindowBox = new System.Windows.Forms.RichTextBox();
            this.contextMenu1 = new System.Windows.Forms.ContextMenu();
            this.copyOutputButton = new System.Windows.Forms.MenuItem();
            this.selectAllOuputButton = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();
            // 
            // outputWindowBox
            // 
            this.outputWindowBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.outputWindowBox.Location = new System.Drawing.Point(0, 2);
            this.outputWindowBox.Name = "outputWindowBox";
            this.outputWindowBox.Size = new System.Drawing.Size(526, 190);
            this.outputWindowBox.TabIndex = 2;
            this.outputWindowBox.Text = "";
            this.outputWindowBox.WordWrap = false;
            this.outputWindowBox.DoubleClick += new System.EventHandler(this.outputWindowBox_DoubleClick);
            // 
            // contextMenu1
            // 
            this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.copyOutputButton,
            this.selectAllOuputButton});
            // 
            // copyOutputButton
            // 
            this.copyOutputButton.Index = 0;
            this.copyOutputButton.Shortcut = System.Windows.Forms.Shortcut.CtrlC;
            this.copyOutputButton.Text = "Copy";
            this.copyOutputButton.Click += new System.EventHandler(this.copyOutputButton_Click);
            // 
            // selectAllOuputButton
            // 
            this.selectAllOuputButton.Index = 1;
            this.selectAllOuputButton.Shortcut = System.Windows.Forms.Shortcut.CtrlA;
            this.selectAllOuputButton.Text = "Select All";
            this.selectAllOuputButton.Click += new System.EventHandler(this.selectAllOuputButton_Click);
            // 
            // OutputWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.ClientSize = new System.Drawing.Size(526, 194);
            this.Controls.Add(this.outputWindowBox);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "OutputWindow";
            this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
            this.ShowHint = Revsoft.Docking.DockState.DockBottomAutoHide;
            this.TabText = "Output";
            this.Text = "Output";
            this.ResumeLayout(false);

		}
		#endregion

        private System.Windows.Forms.RichTextBox outputWindowBox;
        private System.Windows.Forms.ContextMenu contextMenu1;
        private System.Windows.Forms.MenuItem copyOutputButton;
        private System.Windows.Forms.MenuItem selectAllOuputButton;

    }
}