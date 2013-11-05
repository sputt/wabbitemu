namespace Revsoft.Wabbitcode.DockingWindows
{
    public partial class ExpressionWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.RichTextBox expressionBox;

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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ExpressionWindow));
            this.expressionBox = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // expressionBox
            // 
            this.expressionBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.expressionBox.Location = new System.Drawing.Point(0, 2);
            this.expressionBox.Name = "expressionBox";
            this.expressionBox.Size = new System.Drawing.Size(526, 190);
            this.expressionBox.TabIndex = 2;
            this.expressionBox.Text = "";
            this.expressionBox.WordWrap = false;
            this.expressionBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.expressionBox_KeyPress);
            // 
            // ExpressionWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.ClientSize = new System.Drawing.Size(526, 194);
            this.Controls.Add(this.expressionBox);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ExpressionWindow";
            this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
            this.TabText = "Expression Window";
            this.Text = "Expression Window";
            this.ResumeLayout(false);

        }
    }
}