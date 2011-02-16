namespace Revsoft.Wabbitcode.Docking_Windows
{
    partial class EmulatorWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;

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
            this.components = new System.ComponentModel.Container();
            this.contextMenu = new System.Windows.Forms.ContextMenu();
            this.autoHideItem = new System.Windows.Forms.MenuItem();
            this.floatingItem = new System.Windows.Forms.MenuItem();
            this.hideItem = new System.Windows.Forms.MenuItem();
            this.screenBox = new System.Windows.Forms.PictureBox();
            this.doubleBox = new System.Windows.Forms.CheckBox();
            this.updateTimer = new System.Windows.Forms.Timer(this.components);
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            ((System.ComponentModel.ISupportInitialize)(this.screenBox)).BeginInit();
            this.SuspendLayout();
            // 
            // autoHideItem
            // 
            this.autoHideItem.Index = -1;
            this.autoHideItem.Text = "Autohide";
            // 
            // floatingItem
            // 
            this.floatingItem.Index = -1;
            this.floatingItem.Text = "Floating";
            // 
            // hideItem
            // 
            this.hideItem.Index = -1;
            this.hideItem.Text = "Hide";
            // 
            // screenBox
            // 
            this.screenBox.Location = new System.Drawing.Point(12, 12);
            this.screenBox.Name = "screenBox";
            this.screenBox.Size = new System.Drawing.Size(192, 128);
            this.screenBox.TabIndex = 0;
            this.screenBox.TabStop = false;
            // 
            // doubleBox
            // 
            this.doubleBox.AutoSize = true;
            this.doubleBox.Checked = true;
            this.doubleBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.doubleBox.Location = new System.Drawing.Point(12, 157);
            this.doubleBox.Name = "doubleBox";
            this.doubleBox.Size = new System.Drawing.Size(83, 17);
            this.doubleBox.TabIndex = 1;
            this.doubleBox.Text = "Double Size";
            this.doubleBox.UseVisualStyleBackColor = true;
            // 
            // updateTimer
            // 
            this.updateTimer.Interval = 25;
            this.updateTimer.Tick += new System.EventHandler(this.updateTimer_Tick);
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            // 
            // EmulatorWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(214, 186);
            this.Controls.Add(this.doubleBox);
            this.Controls.Add(this.screenBox);
            this.KeyPreview = true;
            this.Name = "EmulatorWindow";
            this.ShowHint = Revsoft.Docking.DockState.Float;
            this.ShowInTaskbar = false;
            this.TabPageContextMenu = this.contextMenu;
            ((System.ComponentModel.ISupportInitialize)(this.screenBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ContextMenu contextMenu;
        private System.Windows.Forms.MenuItem autoHideItem;
        private System.Windows.Forms.MenuItem floatingItem;
        private System.Windows.Forms.MenuItem hideItem;
        private System.Windows.Forms.PictureBox screenBox;
        private System.Windows.Forms.CheckBox doubleBox;
        private System.Windows.Forms.Timer updateTimer;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;

    }
}