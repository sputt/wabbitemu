using System.ComponentModel;
using System.Windows.Forms;
using Revsoft.Wabbitcode.TextEditor;

namespace Revsoft.Wabbitcode.GUI.DocumentWindows
{
	partial class TextEditor
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

            if (_debuggerService.CurrentDebugger != null)
            {
                _debuggerService.CurrentDebugger.DebuggerRunningChanged -= Debugger_OnDebuggerRunningChanged;
                _debuggerService.CurrentDebugger.DebuggerStep -= Debugger_OnDebuggerStep;
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TextEditor));
            this.editorBox = new WabbitcodeTextEditor();
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.tabContextMenu = new System.Windows.Forms.ContextMenu();
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
			// tabContextMenu
			// 
			this.tabContextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
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
			this.ClientSize = new System.Drawing.Size(1024, 768);
			this.Controls.Add(this.editorBox);
			this.DockAreas = WeifenLuo.WinFormsUI.Docking.DockAreas.Document | WeifenLuo.WinFormsUI.Docking.DockAreas.Float;
			this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "newEditor";
			this.Padding = new System.Windows.Forms.Padding(2);
			this.TabPageContextMenu = this.tabContextMenu;
			this.TabText = "";
			this.Text = "frmDocument";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.editor_FormClosing);
			this.DragDrop += new System.Windows.Forms.DragEventHandler(this.editor_DragDrop);
			this.DragEnter += new System.Windows.Forms.DragEventHandler(this.editor_DragEnter);
			this.ResumeLayout(false);

		}

		#endregion
		private WabbitcodeTextEditor editorBox;
		internal ImageList imageList1;
        private ContextMenu tabContextMenu;
        private MenuItem saveMenuItem;
        private MenuItem closeMenuItem;
        private MenuItem closeAllOtherMenuItem;
        private MenuItem closeAllMenuItem;
        private MenuItem menuItem5;
        private MenuItem menuItem6;
        private MenuItem copyPathMenuItem;
        private MenuItem openFolderMenuItem;
	}
}
