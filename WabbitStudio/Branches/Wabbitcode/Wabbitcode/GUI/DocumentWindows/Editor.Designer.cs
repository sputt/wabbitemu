using System.ComponentModel;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.GUI.DocumentWindows
{
	partial class Editor : AbstractFileEditor
	{
		private IContainer components;
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Editor));
            this.editorBox = new Revsoft.Wabbitcode.EditorExtensions.WabbitcodeTextEditor();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.contextMenu = new System.Windows.Forms.ContextMenu();
			this.cutContext = new System.Windows.Forms.MenuItem();
			this.copyContext = new System.Windows.Forms.MenuItem();
			this.pasteContext = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.selectAllContext = new System.Windows.Forms.MenuItem();
            this.setNextStateMenuItem = new System.Windows.Forms.MenuItem();
			this.fixCaseContext = new System.Windows.Forms.MenuItem();
            this.renameContext = new System.Windows.Forms.MenuItem();
            this.extractMethodContext = new System.Windows.Forms.MenuItem();
            this.refactorContext = new System.Windows.Forms.MenuItem();
			this.findRefContext = new System.Windows.Forms.MenuItem();
			this.bgotoButton = new System.Windows.Forms.MenuItem();
			this.contextMenu1 = new System.Windows.Forms.ContextMenu();
			this.saveMenuItem = new System.Windows.Forms.MenuItem();
			this.closeMenuItem = new System.Windows.Forms.MenuItem();
			this.closeAllOtherMenuItem = new System.Windows.Forms.MenuItem();
			this.closeAllMenuItem = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.copyPathMenuItem = new System.Windows.Forms.MenuItem();
			this.openFolderMenuItem = new System.Windows.Forms.MenuItem();
			this.SuspendLayout();
            //
            // editorBox
            //
            this.editorBox.AllowDrop = true;
            this.editorBox.Dock = DockStyle.Fill;
            this.editorBox.DragDrop += this.editor_DragDrop;
            this.editorBox.DragEnter += this.editor_DragEnter;
            this.editorBox.ActiveTextAreaControl.TextArea.DragDrop += editor_DragDrop;
            this.editorBox.ActiveTextAreaControl.TextArea.DragEnter += editor_DragEnter;
            
			// 
			// imageList1
			// 
			this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			this.imageList1.Images.SetKeyName(0, "PreProcessorAutocomplete.png");
			this.imageList1.Images.SetKeyName(1, "DirectivesAutocomplete.png");
			this.imageList1.Images.SetKeyName(2, "ConditionalRegistersAutocomplete.png");
			this.imageList1.Images.SetKeyName(3, "RegistersAutocomplete.png");
			this.imageList1.Images.SetKeyName(4, "LabelAutocomplete.png");
			this.imageList1.Images.SetKeyName(5, "EquatesAutocomplete.png");
			this.imageList1.Images.SetKeyName(6, "HexadecimalNumbersAutocomplete.png");
			this.imageList1.Images.SetKeyName(7, "OpCodeAutocomplete.png");
			// 
			// contextMenu
			// 
			this.contextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.cutContext,
			this.copyContext,
			this.pasteContext,
			this.menuItem4,
            this.refactorContext,
			this.findRefContext,
			this.menuItem5,
			this.selectAllContext,
            this.setNextStateMenuItem,
			this.fixCaseContext,
			this.bgotoButton});
			// 
			// cutContext
			// 
			this.cutContext.Index = 0;
			this.cutContext.Text = "Cut";
			this.cutContext.Click += new System.EventHandler(this.cutContext_Click);
			// 
			// copyContext
			// 
			this.copyContext.Index = 1;
			this.copyContext.Text = "Copy";
			this.copyContext.Click += new System.EventHandler(this.copyContext_Click);
			// 
			// pasteContext
			// 
			this.pasteContext.Index = 2;
			this.pasteContext.Text = "Paste";
			this.pasteContext.Click += new System.EventHandler(this.pasteContext_Click);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 3;
			this.menuItem4.Text = "-";
            // 
            // refactorContext
            // 
            this.refactorContext.Index = 4;
            this.refactorContext.Text = "Refactor";
            this.refactorContext.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
				this.renameContext,
				this.extractMethodContext,
			});

            // 
            // renameContext
            // 
            this.renameContext.Index = 0;
            this.renameContext.Text = "Rename";
            this.renameContext.Click += new System.EventHandler(this.renameContext_Click);

            // 
            // extractMethodContext
            // 
            this.extractMethodContext.Index = 1;
            this.extractMethodContext.Text = "Extract Method";
            this.extractMethodContext.Click += new System.EventHandler(this.extractMethodContext_Click);
			// 
			// findRefContext
			// 
			this.findRefContext.Index = 5;
			this.findRefContext.Text = "Find References";
			this.findRefContext.Click += new System.EventHandler(this.findRefContext_Click);
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 6;
			this.menuItem5.Text = "-";
			// 
			// selectAllContext
			// 
			this.selectAllContext.Index = 7;
			this.selectAllContext.Text = "Select All";
			this.selectAllContext.Click += new System.EventHandler(this.selectAllContext_Click);
            // 
            // setNextStateMenuItem
            // 
            this.setNextStateMenuItem.Index = 8;
            this.setNextStateMenuItem.Text = "Set Next Statement";
            this.setNextStateMenuItem.Visible = false;
            this.setNextStateMenuItem.Click += new System.EventHandler(this.setNextStateMenuItem_Click);
			// 
			// fixCaseContext
			// 
			this.fixCaseContext.Index = 9;
			this.fixCaseContext.Text = "Fix Case";
			this.fixCaseContext.Visible = false;
			this.fixCaseContext.Click += new System.EventHandler(this.fixCaseContext_Click);
			// 
			// bgotoButton
			// 
			this.bgotoButton.Enabled = false;
			this.bgotoButton.Index = 10;
			this.bgotoButton.Text = "Goto";
			this.bgotoButton.Click += new System.EventHandler(this.bgotoButton_Click);
			// 
			// contextMenu1
			// 
			this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
			this.closeMenuItem,
			this.closeAllOtherMenuItem,
			this.closeAllMenuItem,
			this.saveMenuItem,
			this.menuItem6,
			this.copyPathMenuItem,
			this.openFolderMenuItem});
			// 
			// saveMenuItem
			// 
			this.saveMenuItem.Index = 2;
			this.saveMenuItem.Text = "Save";
			this.saveMenuItem.Click += new System.EventHandler(this.saveMenuItem_Click);
			// 
			// closeMenuItem
			// 
			this.closeMenuItem.Index = 0;
			this.closeMenuItem.Text = "Close";
			this.closeMenuItem.Click += new System.EventHandler(this.closeMenuItem_Click);
			// 
			// closeAllOtherMenuItem
			// 
			this.closeAllOtherMenuItem.Index = 1;
			this.closeAllOtherMenuItem.Text = "Close All Other";
			this.closeAllOtherMenuItem.Click += new System.EventHandler(this.closeAllOtherMenuItem_Click);
			// 
			// closeAllMenuItem
			// 
			this.closeAllMenuItem.Index = 1;
			this.closeAllMenuItem.Text = "Close All";
			this.closeAllMenuItem.Click += new System.EventHandler(this.closeAllMenuItem_Click);
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 3;
			this.menuItem6.Text = "-";
			// 
			// copyPathMenuItem
			// 
			this.copyPathMenuItem.Index = 4;
			this.copyPathMenuItem.Text = "Copy Path";
			this.copyPathMenuItem.Click += new System.EventHandler(this.copyPathMenuItem_Click);
			// 
			// openFolderMenuItem
			// 
			this.openFolderMenuItem.Index = 5;
			this.openFolderMenuItem.Text = "Open Containing Folder";
			this.openFolderMenuItem.Click += new System.EventHandler(this.openFolderMenuItem_Click);
			// 
			// newEditor
			// 
			this.AllowDrop = true;
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.ClientSize = new System.Drawing.Size(564, 340);
			this.Controls.Add(this.editorBox);
			this.DockAreas = WeifenLuo.WinFormsUI.Docking.DockAreas.Document | WeifenLuo.WinFormsUI.Docking.DockAreas.Float;
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "newEditor";
			this.Padding = new System.Windows.Forms.Padding(2);
			this.TabPageContextMenu = this.contextMenu1;
			this.TabText = "";
			this.Text = "frmDocument";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.editor_FormClosing);
			this.DragDrop += new System.Windows.Forms.DragEventHandler(this.editor_DragDrop);
			this.DragEnter += new System.Windows.Forms.DragEventHandler(this.editor_DragEnter);
			this.ResumeLayout(false);

		}

		#endregion
		private Revsoft.Wabbitcode.EditorExtensions.WabbitcodeTextEditor editorBox;
		internal ImageList imageList1;
		private ContextMenu contextMenu;
		private MenuItem cutContext;
		private MenuItem copyContext;
		private MenuItem pasteContext;
        private MenuItem refactorContext;
        private MenuItem renameContext;
        private MenuItem extractMethodContext;
		private MenuItem findRefContext;
		private MenuItem fixCaseContext;
		private MenuItem menuItem4;
		private MenuItem selectAllContext;
		private ContextMenu contextMenu1;
		private MenuItem saveMenuItem;
		private MenuItem closeMenuItem;
		private MenuItem closeAllOtherMenuItem;
		private MenuItem closeAllMenuItem;
		private MenuItem menuItem5;
		private MenuItem menuItem6;
		private MenuItem copyPathMenuItem;
		private MenuItem openFolderMenuItem;
        private MenuItem setNextStateMenuItem;
		private MenuItem bgotoButton;
	}
}
