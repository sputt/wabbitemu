namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    partial class BitmapViewer
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
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.button1 = new System.Windows.Forms.Button();
            this.vertRuler = new System.Windows.Forms.PictureBox();
            this.horzRuler = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.vertRuler)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.horzRuler)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox
            // 
            this.pictureBox.BackColor = System.Drawing.SystemColors.Menu;
            this.pictureBox.Location = new System.Drawing.Point(33, 30);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(200, 112);
            this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.pictureBox.TabIndex = 0;
            this.pictureBox.TabStop = false;
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.Location = new System.Drawing.Point(87, 152);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(72, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "OK";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // vertRuler
            // 
            this.vertRuler.Location = new System.Drawing.Point(12, 30);
            this.vertRuler.Name = "vertRuler";
            this.vertRuler.Size = new System.Drawing.Size(15, 112);
            this.vertRuler.TabIndex = 2;
            this.vertRuler.TabStop = false;
            // 
            // horzRuler
            // 
            this.horzRuler.Location = new System.Drawing.Point(33, 12);
            this.horzRuler.Name = "horzRuler";
            this.horzRuler.Size = new System.Drawing.Size(200, 12);
            this.horzRuler.TabIndex = 2;
            this.horzRuler.TabStop = false;
            // 
            // BitmapViewer
            // 
            this.AcceptButton = this.button1;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(245, 187);
            this.Controls.Add(this.horzRuler);
            this.Controls.Add(this.vertRuler);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.pictureBox);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "BitmapViewer";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Screen";
            this.TopMost = true;
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.vertRuler)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.horzRuler)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button button1;
        public System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.PictureBox vertRuler;
        private System.Windows.Forms.PictureBox horzRuler;
    }
}