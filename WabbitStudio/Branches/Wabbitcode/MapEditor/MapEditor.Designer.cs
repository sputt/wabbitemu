namespace Revsoft.MapEditor
{
    partial class MapEditor
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
            this.mainMenu = new System.Windows.Forms.MainMenu(this.components);
            this.fileMenuItem = new System.Windows.Forms.MenuItem();
            this.newMenuItem = new System.Windows.Forms.MenuItem();
            this.openMenuItem = new System.Windows.Forms.MenuItem();
            this.saveMenuItem = new System.Windows.Forms.MenuItem();
            this.saveAsMenuItem = new System.Windows.Forms.MenuItem();
            this.loadMenuItem = new System.Windows.Forms.MenuItem();
            this.closeMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem8 = new System.Windows.Forms.MenuItem();
            this.exitMenuItem = new System.Windows.Forms.MenuItem();
            this.editMenuItem = new System.Windows.Forms.MenuItem();
            this.undoMenuItem = new System.Windows.Forms.MenuItem();
            this.redoMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem12 = new System.Windows.Forms.MenuItem();
            this.cutMenuItem = new System.Windows.Forms.MenuItem();
            this.copyMenuItem = new System.Windows.Forms.MenuItem();
            this.pasteMenuItem = new System.Windows.Forms.MenuItem();
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.menuItem2 = new System.Windows.Forms.MenuItem();
            this.addNewMenuItem = new System.Windows.Forms.MenuItem();
            this.addExistMenuItem = new System.Windows.Forms.MenuItem();
            this.mapView = new System.Windows.Forms.ListView();
            this.tileManager = new System.Windows.Forms.FlowLayoutPanel();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.map = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.map)).BeginInit();
            this.SuspendLayout();
            // 
            // mainMenu
            // 
            this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.fileMenuItem,
            this.editMenuItem,
            this.menuItem1,
            this.menuItem2});
            // 
            // fileMenuItem
            // 
            this.fileMenuItem.Index = 0;
            this.fileMenuItem.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.newMenuItem,
            this.openMenuItem,
            this.saveMenuItem,
            this.saveAsMenuItem,
            this.loadMenuItem,
            this.closeMenuItem,
            this.menuItem8,
            this.exitMenuItem});
            this.fileMenuItem.Text = "File";
            // 
            // newMenuItem
            // 
            this.newMenuItem.Index = 0;
            this.newMenuItem.Text = "New";
            // 
            // openMenuItem
            // 
            this.openMenuItem.Index = 1;
            this.openMenuItem.Text = "Open";
            // 
            // saveMenuItem
            // 
            this.saveMenuItem.Index = 2;
            this.saveMenuItem.Text = "Save";
            // 
            // saveAsMenuItem
            // 
            this.saveAsMenuItem.Index = 3;
            this.saveAsMenuItem.Text = "Save As...";
            // 
            // loadMenuItem
            // 
            this.loadMenuItem.Index = 4;
            this.loadMenuItem.Text = "Load Tile Set";
            // 
            // closeMenuItem
            // 
            this.closeMenuItem.Index = 5;
            this.closeMenuItem.Text = "Close";
            // 
            // menuItem8
            // 
            this.menuItem8.Index = 6;
            this.menuItem8.Text = "-";
            // 
            // exitMenuItem
            // 
            this.exitMenuItem.Index = 7;
            this.exitMenuItem.Text = "Exit";
            // 
            // editMenuItem
            // 
            this.editMenuItem.Index = 1;
            this.editMenuItem.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.undoMenuItem,
            this.redoMenuItem,
            this.menuItem12,
            this.cutMenuItem,
            this.copyMenuItem,
            this.pasteMenuItem});
            this.editMenuItem.Text = "Edit";
            // 
            // undoMenuItem
            // 
            this.undoMenuItem.Index = 0;
            this.undoMenuItem.Text = "Undo";
            // 
            // redoMenuItem
            // 
            this.redoMenuItem.Index = 1;
            this.redoMenuItem.Text = "Redo";
            // 
            // menuItem12
            // 
            this.menuItem12.Index = 2;
            this.menuItem12.Text = "-";
            // 
            // cutMenuItem
            // 
            this.cutMenuItem.Index = 3;
            this.cutMenuItem.Text = "Cut";
            // 
            // copyMenuItem
            // 
            this.copyMenuItem.Index = 4;
            this.copyMenuItem.Text = "Copy";
            // 
            // pasteMenuItem
            // 
            this.pasteMenuItem.Index = 5;
            this.pasteMenuItem.Text = "Paste";
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 2;
            this.menuItem1.Text = "View";
            // 
            // menuItem2
            // 
            this.menuItem2.Index = 3;
            this.menuItem2.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.addNewMenuItem,
            this.addExistMenuItem});
            this.menuItem2.Text = "Tiles";
            // 
            // addNewMenuItem
            // 
            this.addNewMenuItem.Index = 0;
            this.addNewMenuItem.Text = "Add New...";
            // 
            // addExistMenuItem
            // 
            this.addExistMenuItem.Index = 1;
            this.addExistMenuItem.Text = "Add Existing...";
            this.addExistMenuItem.Click += new System.EventHandler(this.addExistMenuItem_Click);
            // 
            // mapView
            // 
            this.mapView.Dock = System.Windows.Forms.DockStyle.Top;
            this.mapView.Location = new System.Drawing.Point(0, 0);
            this.mapView.Name = "mapView";
            this.mapView.Size = new System.Drawing.Size(574, 85);
            this.mapView.TabIndex = 0;
            this.mapView.UseCompatibleStateImageBehavior = false;
            // 
            // tileManager
            // 
            this.tileManager.AllowDrop = true;
            this.tileManager.Dock = System.Windows.Forms.DockStyle.Right;
            this.tileManager.FlowDirection = System.Windows.Forms.FlowDirection.TopDown;
            this.tileManager.Location = new System.Drawing.Point(423, 85);
            this.tileManager.Name = "tileManager";
            this.tileManager.Size = new System.Drawing.Size(151, 121);
            this.tileManager.TabIndex = 1;
            // 
            // openFileDialog
            // 
            this.openFileDialog.Filter = "Bitmaps|*.bmp";
            this.openFileDialog.Multiselect = true;
            this.openFileDialog.RestoreDirectory = true;
            // 
            // map
            // 
            this.map.Dock = System.Windows.Forms.DockStyle.Fill;
            this.map.Location = new System.Drawing.Point(0, 85);
            this.map.Name = "map";
            this.map.Size = new System.Drawing.Size(423, 121);
            this.map.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.map.TabIndex = 2;
            this.map.TabStop = false;
            // 
            // MapEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(574, 206);
            this.Controls.Add(this.map);
            this.Controls.Add(this.tileManager);
            this.Controls.Add(this.mapView);
            this.Menu = this.mainMenu;
            this.Name = "MapEditor";
            this.Text = "Wabbitcode Map Editor";
            ((System.ComponentModel.ISupportInitialize)(this.map)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.MainMenu mainMenu;
        private System.Windows.Forms.MenuItem fileMenuItem;
        private System.Windows.Forms.MenuItem newMenuItem;
        private System.Windows.Forms.MenuItem openMenuItem;
        private System.Windows.Forms.MenuItem saveMenuItem;
        private System.Windows.Forms.MenuItem saveAsMenuItem;
        private System.Windows.Forms.MenuItem closeMenuItem;
        private System.Windows.Forms.MenuItem menuItem8;
        private System.Windows.Forms.MenuItem exitMenuItem;
        private System.Windows.Forms.MenuItem editMenuItem;
        private System.Windows.Forms.MenuItem loadMenuItem;
        private System.Windows.Forms.MenuItem undoMenuItem;
        private System.Windows.Forms.MenuItem redoMenuItem;
        private System.Windows.Forms.MenuItem menuItem12;
        private System.Windows.Forms.MenuItem cutMenuItem;
        private System.Windows.Forms.MenuItem copyMenuItem;
        private System.Windows.Forms.MenuItem pasteMenuItem;
        private System.Windows.Forms.ListView mapView;
        private System.Windows.Forms.FlowLayoutPanel tileManager;
        private System.Windows.Forms.PictureBox map;
        private System.Windows.Forms.MenuItem menuItem1;
        private System.Windows.Forms.MenuItem menuItem2;
        private System.Windows.Forms.MenuItem addNewMenuItem;
        private System.Windows.Forms.MenuItem addExistMenuItem;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
    }
}