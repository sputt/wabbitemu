namespace Revsoft.Wabbitcode.Docking_Windows
{
    partial class DirectoryViewer
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DirectoryViewer));
            this.dirViewer = new System.Windows.Forms.TreeView();
            this.projectIcons = new System.Windows.Forms.ImageList(this.components);
            this.fileContextMenu = new System.Windows.Forms.ContextMenu();
            this.openMenuItem = new System.Windows.Forms.MenuItem();
            this.openWithMenuItem = new System.Windows.Forms.MenuItem();
            this.renMenuItem = new System.Windows.Forms.MenuItem();
            this.delMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem5 = new System.Windows.Forms.MenuItem();
            this.cutMenuItem = new System.Windows.Forms.MenuItem();
            this.copyMenuItem = new System.Windows.Forms.MenuItem();
            this.pasteMenuItem = new System.Windows.Forms.MenuItem();
            this.folderContextMenu = new System.Windows.Forms.ContextMenu();
            this.openIEMenuItem = new System.Windows.Forms.MenuItem();
            this.renFMenuItem = new System.Windows.Forms.MenuItem();
            this.delFMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem6 = new System.Windows.Forms.MenuItem();
            this.cutFMenuItem = new System.Windows.Forms.MenuItem();
            this.copyFMenuItem = new System.Windows.Forms.MenuItem();
            this.pasteFMenuItem = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();
            // 
            // dirViewer
            // 
            this.dirViewer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dirViewer.ImageIndex = 0;
            this.dirViewer.ImageList = this.projectIcons;
            this.dirViewer.LabelEdit = true;
            this.dirViewer.Location = new System.Drawing.Point(0, 2);
            this.dirViewer.Name = "dirViewer";
            this.dirViewer.SelectedImageIndex = 0;
            this.dirViewer.Size = new System.Drawing.Size(234, 393);
            this.dirViewer.TabIndex = 0;
            this.dirViewer.AfterLabelEdit += new System.Windows.Forms.NodeLabelEditEventHandler(this.dirViewer_AfterLabelEdit);
            this.dirViewer.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.directoryViewer_BeforeExpand);
            this.dirViewer.DoubleClick += new System.EventHandler(this.directoryViewer_DoubleClick);
            this.dirViewer.BeforeCollapse += new System.Windows.Forms.TreeViewCancelEventHandler(this.directoryViewer_BeforeCollapse);
            this.dirViewer.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.dirViewer_NodeMouseClick);
            this.dirViewer.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.directoryViewer_ItemDrag);
            // 
            // projectIcons
            // 
            this.projectIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("projectIcons.ImageStream")));
            this.projectIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.projectIcons.Images.SetKeyName(0, "folder_open_16.gif");
            this.projectIcons.Images.SetKeyName(1, "folder_open_16_h.gif");
            this.projectIcons.Images.SetKeyName(2, "folder_closed_16.gif");
            this.projectIcons.Images.SetKeyName(3, "folder_closed_16_h.gif");
            this.projectIcons.Images.SetKeyName(4, "new_document_16.gif");
            this.projectIcons.Images.SetKeyName(5, "new_document_16_h.gif");
            // 
            // fileContextMenu
            // 
            this.fileContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.openMenuItem,
            this.openWithMenuItem,
            this.renMenuItem,
            this.delMenuItem,
            this.menuItem5,
            this.cutMenuItem,
            this.copyMenuItem,
            this.pasteMenuItem});
            // 
            // openMenuItem
            // 
            this.openMenuItem.Index = 0;
            this.openMenuItem.Text = "Open";
            this.openMenuItem.Click += new System.EventHandler(this.openMenuItem_Click);
            // 
            // openWithMenuItem
            // 
            this.openWithMenuItem.Index = 1;
            this.openWithMenuItem.Text = "Open With...";
            this.openWithMenuItem.Click += new System.EventHandler(this.openWithMenuItem_Click);
            // 
            // renMenuItem
            // 
            this.renMenuItem.Index = 2;
            this.renMenuItem.Text = "Rename";
            this.renMenuItem.Click += new System.EventHandler(this.renMenuItem_Click);
            // 
            // delMenuItem
            // 
            this.delMenuItem.Index = 3;
            this.delMenuItem.Text = "Delete";
            this.delMenuItem.Click += new System.EventHandler(this.delMenuItem_Click);
            // 
            // menuItem5
            // 
            this.menuItem5.Index = 4;
            this.menuItem5.Text = "-";
            // 
            // cutMenuItem
            // 
            this.cutMenuItem.Index = 5;
            this.cutMenuItem.Text = "Cut";
            this.cutMenuItem.Click += new System.EventHandler(this.cutMenuItem_Click);
            // 
            // copyMenuItem
            // 
            this.copyMenuItem.Index = 6;
            this.copyMenuItem.Text = "Copy";
            this.copyMenuItem.Click += new System.EventHandler(this.copyMenuItem_Click);
            // 
            // pasteMenuItem
            // 
            this.pasteMenuItem.Index = 7;
            this.pasteMenuItem.Text = "Paste";
            this.pasteMenuItem.Click += new System.EventHandler(this.pasteMenuItem_Click);
            // 
            // folderContextMenu
            // 
            this.folderContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.openIEMenuItem,
            this.renFMenuItem,
            this.delFMenuItem,
            this.menuItem6,
            this.cutFMenuItem,
            this.copyFMenuItem,
            this.pasteFMenuItem});
            // 
            // openIEMenuItem
            // 
            this.openIEMenuItem.Index = 0;
            this.openIEMenuItem.Text = "Open in Explorer";
            this.openIEMenuItem.Click += new System.EventHandler(this.openIEMenuItem_Click);
            // 
            // renFMenuItem
            // 
            this.renFMenuItem.Index = 1;
            this.renFMenuItem.Text = "Rename";
            this.renFMenuItem.Click += new System.EventHandler(this.renFMenuItem_Click);
            // 
            // delFMenuItem
            // 
            this.delFMenuItem.Index = 2;
            this.delFMenuItem.Text = "Delete";
            this.delFMenuItem.Click += new System.EventHandler(this.delFMenuItem_Click);
            // 
            // menuItem6
            // 
            this.menuItem6.Index = 3;
            this.menuItem6.Text = "-";
            // 
            // cutFMenuItem
            // 
            this.cutFMenuItem.Index = 4;
            this.cutFMenuItem.Text = "Cut";
            this.cutFMenuItem.Click += new System.EventHandler(this.cutFMenuItem_Click);
            // 
            // copyFMenuItem
            // 
            this.copyFMenuItem.Index = 5;
            this.copyFMenuItem.Text = "Copy";
            this.copyFMenuItem.Click += new System.EventHandler(this.copyFMenuItem_Click);
            // 
            // pasteFMenuItem
            // 
            this.pasteFMenuItem.Index = 6;
            this.pasteFMenuItem.Text = "Paste";
            this.pasteFMenuItem.Click += new System.EventHandler(this.pasteFMenuItem_Click);
            // 
            // DirectoryViewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.ClientSize = new System.Drawing.Size(234, 397);
            this.Controls.Add(this.dirViewer);
            this.HideOnClose = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "DirectoryViewer";
            this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
            this.ShowHint = Revsoft.Docking.DockState.DockRightAutoHide;
            this.TabText = "Directory Viewer";
            this.Text = "Directory Viewer";
            this.VisibleChanged += new System.EventHandler(this.DirectoryViewer_VisibleChanged);
            this.ResumeLayout(false);

		}
		#endregion

        private System.Windows.Forms.ContextMenu fileContextMenu;
        private System.Windows.Forms.MenuItem openMenuItem;
        private System.Windows.Forms.MenuItem openWithMenuItem;
        private System.Windows.Forms.MenuItem renMenuItem;
        private System.Windows.Forms.MenuItem delMenuItem;
        private System.Windows.Forms.MenuItem menuItem5;
        private System.Windows.Forms.MenuItem cutMenuItem;
        private System.Windows.Forms.MenuItem copyMenuItem;
        private System.Windows.Forms.MenuItem pasteMenuItem;
        private System.Windows.Forms.ImageList projectIcons;
        private System.Windows.Forms.ContextMenu folderContextMenu;
        private System.Windows.Forms.MenuItem openIEMenuItem;
        private System.Windows.Forms.MenuItem renFMenuItem;
        private System.Windows.Forms.MenuItem delFMenuItem;
        private System.Windows.Forms.MenuItem menuItem6;
        private System.Windows.Forms.MenuItem cutFMenuItem;
        private System.Windows.Forms.MenuItem copyFMenuItem;
        private System.Windows.Forms.MenuItem pasteFMenuItem;
        public System.Windows.Forms.TreeView dirViewer;


    }
}