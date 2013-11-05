namespace Revsoft.Wabbitcode.DockingWindows
{
    public partial class ToolWindow
    {
        private System.Windows.Forms.MenuItem autoHideItem;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.ContextMenu contextMenu;
        private System.Windows.Forms.MenuItem floatingItem;
        private System.Windows.Forms.MenuItem hideItem;

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
            this.contextMenu = new System.Windows.Forms.ContextMenu();
            this.autoHideItem = new System.Windows.Forms.MenuItem();
            this.floatingItem = new System.Windows.Forms.MenuItem();
            this.hideItem = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();

            // contextMenu

            this.contextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
            {
                this.autoHideItem,
                this.floatingItem,
                this.hideItem
            });

            // autoHideItem

            this.autoHideItem.Index = 0;
            this.autoHideItem.Text = "Autohide";
            this.autoHideItem.Click += new System.EventHandler(this.autoHideItem_Click);

            // floatingItem

            this.floatingItem.Index = 1;
            this.floatingItem.Text = "Floating";
            this.floatingItem.Click += new System.EventHandler(this.floatingItem_Click);

            // hideItem

            this.hideItem.Index = 2;
            this.hideItem.Text = "Hide";
            this.hideItem.Click += new System.EventHandler(this.hideItem_Click);

            // ToolWindow

            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft)
                              | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight)
                              | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop)
                              | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, (byte)0);
            this.HideOnClose = true;
            this.Name = "ToolWindow";
            this.TabPageContextMenu = this.contextMenu;
            this.TabText = "ToolWindow";
            this.Text = "ToolWindow";
            this.VisibleChanged += new System.EventHandler(ToolWindow_VisibleChanged);
            this.ResumeLayout(false);
        }
    }
}