namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class FindResultsWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.RichTextBox findResultsBox;

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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FindResultsWindow));
            this.findResultsBox = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();

            // findResultsBox

            this.findResultsBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.findResultsBox.Location = new System.Drawing.Point(0, 2);
            this.findResultsBox.Name = "findResultsBox";
            this.findResultsBox.Size = new System.Drawing.Size(526, 190);
            this.findResultsBox.TabIndex = 2;
            this.findResultsBox.Text = "";
            this.findResultsBox.WordWrap = false;
            this.findResultsBox.DoubleClick += new System.EventHandler(this.findResultsBox_DoubleClick);

            // FindResultsWindow

            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.ClientSize = new System.Drawing.Size(526, 194);
            this.Controls.Add(this.findResultsBox);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (byte)0);
            this.HideOnClose = true;
            this.Icon = (System.Drawing.Icon)resources.GetObject("$this.Icon");
            this.Name = "FindResultsWindow";
            this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottomAutoHide;
            this.TabText = "Find Results";
            this.Text = "Find Results";
            this.ResumeLayout(false);
        }
    }
}