using System;
using System.Windows.Forms;
using Aga.Controls.Tree;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class ProjectViewer
    {
        private System.Windows.Forms.MenuItem addContextItem;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;
        private System.Windows.Forms.MenuItem copyFMenuItem;
        private System.Windows.Forms.MenuItem copyMenuItem;
        private System.Windows.Forms.MenuItem cutFMenuItem;
        private System.Windows.Forms.MenuItem cutMenuItem;
        private System.Windows.Forms.MenuItem delFMenuItem;
        private System.Windows.Forms.MenuItem delMenuItem;
        private System.Windows.Forms.MenuItem excFromProj;
        private System.Windows.Forms.MenuItem existingFileMenuItem;
        private System.Windows.Forms.ContextMenu fileContextMenu;
        private System.Windows.Forms.ContextMenu folderContextMenu;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.MenuItem menuItem5;
        private System.Windows.Forms.MenuItem menuItem6;
        private System.Windows.Forms.MenuItem newFileContextItem;
        private System.Windows.Forms.MenuItem newFolderContextItem;
        private System.Windows.Forms.MenuItem openExplorerMenuItem;
        private System.Windows.Forms.MenuItem openMenuItem;
        private System.Windows.Forms.MenuItem openWithMenuItem;
        private System.Windows.Forms.MenuItem pasteFMenuItem;
        private System.Windows.Forms.MenuItem pasteMenuItem;
        private Aga.Controls.Tree.TreeViewAdv projViewer;
        private Aga.Controls.Tree.NodeControls.NodeStateIcon icon;
        private Aga.Controls.Tree.NodeControls.NodeTextBox name;
        private System.Windows.Forms.MenuItem renFMenuItem;
        private System.Windows.Forms.MenuItem renMenuItem;

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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProjectViewer));
            this.projViewer = new TreeViewAdv();
            this.icon = new Aga.Controls.Tree.NodeControls.NodeStateIcon();
            this.name = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.folderContextMenu = new System.Windows.Forms.ContextMenu();
            this.addContextItem = new System.Windows.Forms.MenuItem();
            this.newFileContextItem = new System.Windows.Forms.MenuItem();
            this.existingFileMenuItem = new System.Windows.Forms.MenuItem();
            this.newFolderContextItem = new System.Windows.Forms.MenuItem();
            this.renFMenuItem = new System.Windows.Forms.MenuItem();
            this.delFMenuItem = new System.Windows.Forms.MenuItem();
            this.openExplorerMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem6 = new System.Windows.Forms.MenuItem();
            this.cutFMenuItem = new System.Windows.Forms.MenuItem();
            this.copyFMenuItem = new System.Windows.Forms.MenuItem();
            this.pasteFMenuItem = new System.Windows.Forms.MenuItem();
            this.fileContextMenu = new System.Windows.Forms.ContextMenu();
            this.openMenuItem = new System.Windows.Forms.MenuItem();
            this.openWithMenuItem = new System.Windows.Forms.MenuItem();
            this.renMenuItem = new System.Windows.Forms.MenuItem();
            this.delMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.excFromProj = new System.Windows.Forms.MenuItem();
            this.menuItem5 = new System.Windows.Forms.MenuItem();
            this.cutMenuItem = new System.Windows.Forms.MenuItem();
            this.copyMenuItem = new System.Windows.Forms.MenuItem();
            this.pasteMenuItem = new System.Windows.Forms.MenuItem();
            this.SuspendLayout();

            // projViewer

            this.projViewer.AllowDrop = true;
            this.projViewer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.projViewer.Location = new System.Drawing.Point(0, 2);
            this.projViewer.Name = "projViewer";
            this.projViewer.Size = new System.Drawing.Size(234, 393);
            this.projViewer.TabIndex = 0;
            this.projViewer.SelectionMode = Aga.Controls.Tree.TreeSelectionMode.MultiSameParent;
            this.projViewer.BorderStyle = BorderStyle.FixedSingle;
            this.projViewer.NodeControls.Add(this.icon);
            this.projViewer.NodeControls.Add(this.name);
            this.projViewer.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.projViewer_ItemDrag);
            this.projViewer.NodeMouseClick += new System.EventHandler<TreeNodeAdvMouseEventArgs>(this.projViewer_NodeMouseClick);
            this.projViewer.DragDrop += new System.Windows.Forms.DragEventHandler(this.projViewer_DragDrop);
            this.projViewer.DragOver += new System.Windows.Forms.DragEventHandler(this.projViewer_DragOver);
            this.projViewer.DoubleClick += new System.EventHandler(this.projectViewer_DoubleClick);
            this.projViewer.KeyDown += new System.Windows.Forms.KeyEventHandler(projectViewer_KeyDown);

            // name

            this.name.DataPropertyName = "Name";
            this.name.EditEnabled = true;
            this.name.IncrementalSearchEnabled = true;
            this.name.LeftMargin = 3;
            this.name.ParentColumn = null;
            this.name.EditorShowing += NodeEditorShowing;
            this.name.LabelChanged += projViewer_AfterLabelEdit;

            // icon

            this.icon.DataPropertyName = "Icon";
            this.icon.LeftMargin = 1;
            this.icon.ParentColumn = null;

            // folderContextMenu

            this.folderContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
            {
                this.addContextItem,
                this.renFMenuItem,
                this.delFMenuItem,
                this.openExplorerMenuItem,
                this.menuItem6,
                this.cutFMenuItem,
                this.copyFMenuItem,
                this.pasteFMenuItem
            });

            // addContextItem

            this.addContextItem.Index = 0;
            this.addContextItem.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
            {
                this.newFileContextItem,
                this.existingFileMenuItem,
                this.newFolderContextItem
            });
            this.addContextItem.Text = "Add";

            // newFileContextItem

            this.newFileContextItem.Index = 0;
            this.newFileContextItem.Text = "New File";
            this.newFileContextItem.Click += new System.EventHandler(newFileContextItem_Click);

            // existingFileMenuItem

            this.existingFileMenuItem.Index = 1;
            this.existingFileMenuItem.Text = "Existing File";
            this.existingFileMenuItem.Click += new System.EventHandler(this.existingFileMenuItem_Click);

            // newFolderContextItem

            this.newFolderContextItem.Index = 2;
            this.newFolderContextItem.Text = "New Folder";
            this.newFolderContextItem.Click += new System.EventHandler(this.newFolderContextItem_Click);

            // renFMenuItem

            this.renFMenuItem.Index = 1;
            this.renFMenuItem.Text = "Rename";
            this.renFMenuItem.Click += new System.EventHandler(this.renFMenuItem_Click);

            // delFMenuItem

            this.delFMenuItem.Index = 2;
            this.delFMenuItem.Text = "Delete";
            this.delFMenuItem.Click += new System.EventHandler(this.delFMenuItem_Click);

            // openExplorerMenuItem

            this.openExplorerMenuItem.Index = 3;
            this.openExplorerMenuItem.Text = "Open in Explorer";
            this.openExplorerMenuItem.Click += new System.EventHandler(this.openExplorerMenuItem_Click);

            // menuItem6

            this.menuItem6.Index = 4;
            this.menuItem6.Text = "-";

            // cutFMenuItem

            this.cutFMenuItem.Index = 5;
            this.cutFMenuItem.Text = "Cut";

            // copyFMenuItem

            this.copyFMenuItem.Index = 6;
            this.copyFMenuItem.Text = "Copy";

            // pasteFMenuItem

            this.pasteFMenuItem.Index = 7;
            this.pasteFMenuItem.Text = "Paste";

            // fileContextMenu

            this.fileContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[]
            {
                this.openMenuItem,
                this.openWithMenuItem,
                this.renMenuItem,
                this.delMenuItem,
                this.menuItem1,
                this.excFromProj,
                this.menuItem5,
                this.cutMenuItem,
                this.copyMenuItem,
                this.pasteMenuItem
            });

            // openMenuItem

            this.openMenuItem.Index = 0;
            this.openMenuItem.Text = "Open";
            this.openMenuItem.Click += new System.EventHandler(this.openMenuItem_Click);

            // openWithMenuItem

            this.openWithMenuItem.Index = 1;
            this.openWithMenuItem.Text = "Open With...";
            this.openWithMenuItem.Click += new System.EventHandler(this.openWithMenuItem_Click);

            // renMenuItem

            this.renMenuItem.Index = 2;
            this.renMenuItem.Text = "Rename";
            this.renMenuItem.Click += new System.EventHandler(this.renMenuItem_Click);

            // delMenuItem

            this.delMenuItem.Index = 3;
            this.delMenuItem.Text = "Delete";
            this.delMenuItem.Click += new System.EventHandler(this.delMenuItem_Click);

            // menuItem1

            this.menuItem1.Index = 4;
            this.menuItem1.Text = "-";

            // excFromProj

            this.excFromProj.Index = 5;
            this.excFromProj.Text = "Exclude From Project";
            this.excFromProj.Click += new System.EventHandler(this.excFromProj_Click);

            // menuItem5

            this.menuItem5.Index = 6;
            this.menuItem5.Text = "-";

            // cutMenuItem

            this.cutMenuItem.Index = 7;
            this.cutMenuItem.Text = "Cut";
            this.cutMenuItem.Click += new System.EventHandler(this.cutMenuItem_Click);

            // copyMenuItem

            this.copyMenuItem.Index = 8;
            this.copyMenuItem.Text = "Copy";
            this.copyMenuItem.Click += new System.EventHandler(this.copyMenuItem_Click);

            // pasteMenuItem

            this.pasteMenuItem.Index = 9;
            this.pasteMenuItem.Text = "Paste";
            this.pasteMenuItem.Click += new System.EventHandler(this.pasteMenuItem_Click);

            // ProjectViewer

            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.ClientSize = new System.Drawing.Size(234, 397);
            this.Controls.Add(this.projViewer);
            this.Icon = (System.Drawing.Icon)resources.GetObject("$this.Icon");
            this.Name = "ProjectViewer";
            this.Padding = new System.Windows.Forms.Padding(0, 2, 0, 2);
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockRightAutoHide;
            this.TabText = "Project Viewer";
            this.Text = "Project Viewer";
            this.ResumeLayout(false);
        }
    }
}