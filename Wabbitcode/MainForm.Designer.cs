namespace Revsoft.Wabbitcode
{
	partial class MainForm
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
				if (_debugger != null)
				{
					_debugger.Dispose();
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            WeifenLuo.WinFormsUI.Docking.DockPanelSkin dockPanelSkin1 = new WeifenLuo.WinFormsUI.Docking.DockPanelSkin();
            WeifenLuo.WinFormsUI.Docking.AutoHideStripSkin autoHideStripSkin1 = new WeifenLuo.WinFormsUI.Docking.AutoHideStripSkin();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient1 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient1 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripSkin dockPaneStripSkin1 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripSkin();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripGradient dockPaneStripGradient1 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient2 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient2 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient3 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPaneStripToolWindowGradient dockPaneStripToolWindowGradient1 = new WeifenLuo.WinFormsUI.Docking.DockPaneStripToolWindowGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient4 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient5 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.DockPanelGradient dockPanelGradient3 = new WeifenLuo.WinFormsUI.Docking.DockPanelGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient6 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            WeifenLuo.WinFormsUI.Docking.TabGradient tabGradient7 = new WeifenLuo.WinFormsUI.Docking.TabGradient();
            this.statusBar = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.progressBar = new System.Windows.Forms.ToolStripProgressBar();
            this.lineCodeInfo = new System.Windows.Forms.ToolStripStatusLabel();
            this.lineStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.colStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.toolBarIcons = new System.Windows.Forms.ImageList(this.components);
            this.toolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.findBox = new System.Windows.Forms.ToolStripComboBox();
            this.configBox = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.mainToolBar = new System.Windows.Forms.ToolStrip();
            this.newToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.openToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.saveToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.saveAllToolButton = new System.Windows.Forms.ToolStripButton();
            this.printToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.cutToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.copyToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.pasteToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.runToolButton = new System.Windows.Forms.ToolStripButton();
            this.documentParser = new System.ComponentModel.BackgroundWorker();
            this.wabbitemu = new System.ComponentModel.BackgroundWorker();
            this.mainMenu1 = new System.Windows.Forms.MenuStrip();
            this.fileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newProjectMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openProjectMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAllMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveProjectMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator14 = new System.Windows.Forms.ToolStripSeparator();
            this.printMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.printPrevMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pageSetMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator15 = new System.Windows.Forms.ToolStripSeparator();
            this.recentFilesMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator16 = new System.Windows.Forms.ToolStripSeparator();
            this.exitMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.undoMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.redoMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator10 = new System.Windows.Forms.ToolStripSeparator();
            this.cutMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.copyMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pasteMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectAllMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator11 = new System.Windows.Forms.ToolStripSeparator();
            this.menuItem7 = new System.Windows.Forms.ToolStripMenuItem();
            this.findMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.replaceMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.findInFilesMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.replaceInFilesMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.findAllRefsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator12 = new System.Windows.Forms.ToolStripSeparator();
            this.insertMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.iSpriteMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.iMapMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.formattingMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.makeUpperMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.makeLowerMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.invertCaseMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sentenceCaseMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator21 = new System.Windows.Forms.ToolStripSeparator();
            this.formatDocMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.convertSpacesToTabsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.bookmarkMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toggleBookmarkMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.prevBookmarkMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.nextBookmarkMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.gotoMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.gLineMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.gSymbolMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator13 = new System.Windows.Forms.ToolStripSeparator();
            this.prefsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viewMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolBarMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mainToolMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.debugToolMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editorToolBarMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.labelListMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.projViewMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.macroManagerMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
            this.debugPanelsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.debugPanelMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.callStackMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stackViewerMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.varTrackMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.breakManagerMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator8 = new System.Windows.Forms.ToolStripSeparator();
            this.outWinMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.errListMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.findResultsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.statusBarMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator9 = new System.Windows.Forms.ToolStripSeparator();
            this.lineNumMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.iconBarMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.refactorMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.renameMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.extractMethodMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.projMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addNewFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.existingFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator17 = new System.Windows.Forms.ToolStripSeparator();
            this.buildOrderButton = new System.Windows.Forms.ToolStripMenuItem();
            this.includeDirProjMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.refreshMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator18 = new System.Windows.Forms.ToolStripSeparator();
            this.closeProjMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.asmMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.assembleMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.includeDirButton = new System.Windows.Forms.ToolStripMenuItem();
            this.symTableMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.countCodeMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.projStatsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.listFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hexFileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.macrosMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.runMacroMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.recMacroMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.manMacroMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.debugMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.startDebugMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.startWithoutDebugMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stopDebugMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator19 = new System.Windows.Forms.ToolStripSeparator();
            this.runMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stepMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stepOverMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stepOutMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator20 = new System.Windows.Forms.ToolStripSeparator();
            this.menuItem11 = new System.Windows.Forms.ToolStripMenuItem();
            this.newBreakpointMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newDataBreakpointMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toggleBreakpointMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deleteAllBreakpointsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.disableAllBreakpointsMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.windowMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showHelpMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.updateMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripContainer = new System.Windows.Forms.ToolStripContainer();
            this.debugToolStrip = new System.Windows.Forms.ToolStrip();
            this.runDebuggerToolButton = new System.Windows.Forms.ToolStripButton();
            this.pauseToolButton = new System.Windows.Forms.ToolStripButton();
            this.stopToolButton = new System.Windows.Forms.ToolStripButton();
            this.restartToolStripButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.gotoCurrentToolButton = new System.Windows.Forms.ToolStripButton();
            this.stepToolButton = new System.Windows.Forms.ToolStripButton();
            this.stepOverToolButton = new System.Windows.Forms.ToolStripButton();
            this.stepOutToolButton = new System.Windows.Forms.ToolStripButton();
            this.editorToolStrip = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton2 = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButton3 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton4 = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButton5 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton6 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton7 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton8 = new System.Windows.Forms.ToolStripButton();
            this.dockPanel = new WeifenLuo.WinFormsUI.Docking.DockPanel();
            this.statusBar.SuspendLayout();
            this.mainToolBar.SuspendLayout();
            this.mainMenu1.SuspendLayout();
            this.toolStripContainer.ContentPanel.SuspendLayout();
            this.toolStripContainer.SuspendLayout();
            this.debugToolStrip.SuspendLayout();
            this.editorToolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusBar
            // 
            this.statusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel,
            this.progressBar,
            this.lineCodeInfo,
            this.lineStatusLabel,
            this.colStatusLabel});
            this.statusBar.Location = new System.Drawing.Point(0, 536);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(990, 22);
            this.statusBar.TabIndex = 2;
            this.statusBar.Text = "statusStrip1";
            // 
            // toolStripStatusLabel
            // 
            this.toolStripStatusLabel.Name = "toolStripStatusLabel";
            this.toolStripStatusLabel.Size = new System.Drawing.Size(839, 17);
            this.toolStripStatusLabel.Spring = true;
            this.toolStripStatusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // progressBar
            // 
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(100, 16);
            this.progressBar.Visible = false;
            // 
            // lineCodeInfo
            // 
            this.lineCodeInfo.Name = "lineCodeInfo";
            this.lineCodeInfo.Size = new System.Drawing.Size(85, 17);
            this.lineCodeInfo.Text = "Min: Max: Size:";
            // 
            // lineStatusLabel
            // 
            this.lineStatusLabel.Name = "lineStatusLabel";
            this.lineStatusLabel.Size = new System.Drawing.Size(23, 17);
            this.lineStatusLabel.Text = "Ln:";
            // 
            // colStatusLabel
            // 
            this.colStatusLabel.Name = "colStatusLabel";
            this.colStatusLabel.Size = new System.Drawing.Size(28, 17);
            this.colStatusLabel.Text = "Col:";
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.DefaultExt = "*.asm";
            this.saveFileDialog1.Filter = "Assembly Files (*.asm)|*.asm|Z80 Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*";
            this.saveFileDialog1.Title = "Save File";
            // 
            // toolBarIcons
            // 
            this.toolBarIcons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("toolBarIcons.ImageStream")));
            this.toolBarIcons.TransparentColor = System.Drawing.Color.Transparent;
            this.toolBarIcons.Images.SetKeyName(0, "new_document_16.png");
            this.toolBarIcons.Images.SetKeyName(1, "new_document_16_d.png");
            this.toolBarIcons.Images.SetKeyName(2, "new_document_16_h.png");
            this.toolBarIcons.Images.SetKeyName(3, "open_document_16.png");
            this.toolBarIcons.Images.SetKeyName(4, "open_document_16_d.png");
            this.toolBarIcons.Images.SetKeyName(5, "open_document_16_h.png");
            this.toolBarIcons.Images.SetKeyName(6, "save_16.png");
            this.toolBarIcons.Images.SetKeyName(7, "save_16_d.png");
            this.toolBarIcons.Images.SetKeyName(8, "save_16_h.png");
            // 
            // toolStripSeparator
            // 
            this.toolStripSeparator.Name = "toolStripSeparator";
            this.toolStripSeparator.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 25);
            // 
            // findBox
            // 
            this.findBox.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
            this.findBox.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.findBox.Name = "findBox";
            this.findBox.Size = new System.Drawing.Size(135, 25);
            this.findBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.findBox_KeyPress);
            // 
            // configBox
            // 
            this.configBox.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
            this.configBox.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.configBox.Name = "configBox";
            this.configBox.Size = new System.Drawing.Size(135, 25);
            this.configBox.SelectedIndexChanged += new System.EventHandler(this.configBox_SelectedIndexChanged);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // mainToolBar
            // 
            this.mainToolBar.AllowItemReorder = true;
            this.mainToolBar.AutoSize = false;
            this.mainToolBar.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.mainToolBar.Dock = System.Windows.Forms.DockStyle.None;
            this.mainToolBar.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.mainToolBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripButton,
            this.openToolStripButton,
            this.saveToolStripButton,
            this.saveAllToolButton,
            this.printToolStripButton,
            this.toolStripSeparator,
            this.cutToolStripButton,
            this.copyToolStripButton,
            this.pasteToolStripButton,
            this.toolStripSeparator3,
            this.findBox,
            this.toolStripSeparator2,
            this.runToolButton,
            this.configBox});
            this.mainToolBar.Location = new System.Drawing.Point(0, 24);
            this.mainToolBar.Name = "mainToolBar";
            this.mainToolBar.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.mainToolBar.Size = new System.Drawing.Size(547, 25);
            this.mainToolBar.TabIndex = 0;
            this.mainToolBar.Text = "Main Toolbar";
            // 
            // newToolStripButton
            // 
            this.newToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.newToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("newToolStripButton.Image")));
            this.newToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newToolStripButton.Name = "newToolStripButton";
            this.newToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.newToolStripButton.Text = "&New";
            this.newToolStripButton.Click += new System.EventHandler(this.newToolButton_Click);
            // 
            // openToolStripButton
            // 
            this.openToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.openToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("openToolStripButton.Image")));
            this.openToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.openToolStripButton.Name = "openToolStripButton";
            this.openToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.openToolStripButton.Text = "&Open";
            this.openToolStripButton.Click += new System.EventHandler(this.openToolButton_Click);
            // 
            // saveToolStripButton
            // 
            this.saveToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.saveToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("saveToolStripButton.Image")));
            this.saveToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.saveToolStripButton.Name = "saveToolStripButton";
            this.saveToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.saveToolStripButton.Text = "&Save";
            this.saveToolStripButton.Click += new System.EventHandler(this.saveToolButton_Click);
            // 
            // saveAllToolButton
            // 
            this.saveAllToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.saveAllToolButton.Image = ((System.Drawing.Image)(resources.GetObject("saveAllToolButton.Image")));
            this.saveAllToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.saveAllToolButton.Name = "saveAllToolButton";
            this.saveAllToolButton.Size = new System.Drawing.Size(23, 22);
            this.saveAllToolButton.Text = "Save All";
            this.saveAllToolButton.Click += new System.EventHandler(this.saveAllToolButton_Click);
            // 
            // printToolStripButton
            // 
            this.printToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.printToolStripButton.Enabled = false;
            this.printToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("printToolStripButton.Image")));
            this.printToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.printToolStripButton.Name = "printToolStripButton";
            this.printToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.printToolStripButton.Text = "&Print";
            // 
            // cutToolStripButton
            // 
            this.cutToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.cutToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("cutToolStripButton.Image")));
            this.cutToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.cutToolStripButton.Name = "cutToolStripButton";
            this.cutToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.cutToolStripButton.Text = "C&ut";
            this.cutToolStripButton.Click += new System.EventHandler(this.cutToolButton_Click);
            // 
            // copyToolStripButton
            // 
            this.copyToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.copyToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("copyToolStripButton.Image")));
            this.copyToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.copyToolStripButton.Name = "copyToolStripButton";
            this.copyToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.copyToolStripButton.Text = "&Copy";
            this.copyToolStripButton.Click += new System.EventHandler(this.copyToolButton_Click);
            // 
            // pasteToolStripButton
            // 
            this.pasteToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.pasteToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("pasteToolStripButton.Image")));
            this.pasteToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.pasteToolStripButton.Name = "pasteToolStripButton";
            this.pasteToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.pasteToolStripButton.Text = "&Paste";
            this.pasteToolStripButton.Click += new System.EventHandler(this.pasteToolButton_Click);
            // 
            // runToolButton
            // 
            this.runToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.runToolButton.Image = ((System.Drawing.Image)(resources.GetObject("runToolButton.Image")));
            this.runToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.runToolButton.Name = "runToolButton";
            this.runToolButton.Size = new System.Drawing.Size(23, 22);
            this.runToolButton.Text = "Start Debug";
            this.runToolButton.Click += new System.EventHandler(this.startDebugMenuItem_Click);
            // 
            // documentParser
            // 
            this.documentParser.DoWork += new System.ComponentModel.DoWorkEventHandler(this.documentParser_DoWork);
            this.documentParser.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.documentParser_RunWorkerCompleted);
            // 
            // mainMenu1
            // 
            this.mainMenu1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileMenuItem,
            this.editMenuItem,
            this.viewMenuItem,
            this.refactorMenuItem,
            this.projMenuItem,
            this.asmMenuItem,
            this.macrosMenuItem,
            this.debugMenuItem,
            this.windowMenuItem,
            this.helpMenuItem});
            this.mainMenu1.Location = new System.Drawing.Point(0, 0);
            this.mainMenu1.MdiWindowListItem = this.windowMenuItem;
            this.mainMenu1.Name = "mainMenu1";
            this.mainMenu1.Size = new System.Drawing.Size(990, 24);
            this.mainMenu1.TabIndex = 7;
            // 
            // fileMenuItem
            // 
            this.fileMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newMenuItem,
            this.openMenuItem,
            this.saveMenuItem,
            this.saveAsMenuItem,
            this.saveAllMenuItem,
            this.saveProjectMenuItem,
            this.closeMenuItem,
            this.toolStripSeparator14,
            this.printMenuItem,
            this.printPrevMenuItem,
            this.pageSetMenuItem,
            this.toolStripSeparator15,
            this.recentFilesMenuItem,
            this.toolStripSeparator16,
            this.exitMenuItem});
            this.fileMenuItem.Name = "fileMenuItem";
            this.fileMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileMenuItem.Text = "File";
            // 
            // newMenuItem
            // 
            this.newMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newFileMenuItem,
            this.newProjectMenuItem});
            this.newMenuItem.Name = "newMenuItem";
            this.newMenuItem.Size = new System.Drawing.Size(187, 22);
            this.newMenuItem.Text = "New";
            // 
            // newFileMenuItem
            // 
            this.newFileMenuItem.Name = "newFileMenuItem";
            this.newFileMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.newFileMenuItem.Size = new System.Drawing.Size(186, 22);
            this.newFileMenuItem.Text = "File";
            this.newFileMenuItem.Click += new System.EventHandler(this.newFileMenuItem_Click);
            // 
            // newProjectMenuItem
            // 
            this.newProjectMenuItem.Name = "newProjectMenuItem";
            this.newProjectMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.N)));
            this.newProjectMenuItem.Size = new System.Drawing.Size(186, 22);
            this.newProjectMenuItem.Text = "Project";
            this.newProjectMenuItem.Click += new System.EventHandler(this.newProjectMenuItem_Click);
            // 
            // openMenuItem
            // 
            this.openMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openFileMenuItem,
            this.openProjectMenuItem});
            this.openMenuItem.Name = "openMenuItem";
            this.openMenuItem.Size = new System.Drawing.Size(187, 22);
            this.openMenuItem.Text = "Open";
            // 
            // openFileMenuItem
            // 
            this.openFileMenuItem.Name = "openFileMenuItem";
            this.openFileMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openFileMenuItem.Size = new System.Drawing.Size(186, 22);
            this.openFileMenuItem.Text = "File";
            this.openFileMenuItem.Click += new System.EventHandler(this.openFileMenuItem_Click);
            // 
            // openProjectMenuItem
            // 
            this.openProjectMenuItem.Name = "openProjectMenuItem";
            this.openProjectMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.O)));
            this.openProjectMenuItem.Size = new System.Drawing.Size(186, 22);
            this.openProjectMenuItem.Text = "Project";
            this.openProjectMenuItem.Click += new System.EventHandler(this.openProjectMenuItem_Click);
            // 
            // saveMenuItem
            // 
            this.saveMenuItem.Name = "saveMenuItem";
            this.saveMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveMenuItem.Size = new System.Drawing.Size(187, 22);
            this.saveMenuItem.Text = "Save";
            this.saveMenuItem.Click += new System.EventHandler(this.saveMenuItem_Click);
            // 
            // saveAsMenuItem
            // 
            this.saveAsMenuItem.Name = "saveAsMenuItem";
            this.saveAsMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F12;
            this.saveAsMenuItem.ShowShortcutKeys = false;
            this.saveAsMenuItem.Size = new System.Drawing.Size(187, 22);
            this.saveAsMenuItem.Text = "Save As...";
            this.saveAsMenuItem.Click += new System.EventHandler(this.saveAsMenuItem_Click);
            // 
            // saveAllMenuItem
            // 
            this.saveAllMenuItem.Name = "saveAllMenuItem";
            this.saveAllMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.S)));
            this.saveAllMenuItem.Size = new System.Drawing.Size(187, 22);
            this.saveAllMenuItem.Text = "Save All";
            this.saveAllMenuItem.Click += new System.EventHandler(this.saveProjectMenuItem_Click);
            // 
            // saveProjectMenuItem
            // 
            this.saveProjectMenuItem.Name = "saveProjectMenuItem";
            this.saveProjectMenuItem.Size = new System.Drawing.Size(187, 22);
            this.saveProjectMenuItem.Text = "Save Project";
            this.saveProjectMenuItem.Click += new System.EventHandler(this.saveProjectMenuItem_Click);
            // 
            // closeMenuItem
            // 
            this.closeMenuItem.Name = "closeMenuItem";
            this.closeMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.W)));
            this.closeMenuItem.ShowShortcutKeys = false;
            this.closeMenuItem.Size = new System.Drawing.Size(187, 22);
            this.closeMenuItem.Text = "Close";
            this.closeMenuItem.Click += new System.EventHandler(this.closeMenuItem_Click);
            // 
            // toolStripSeparator14
            // 
            this.toolStripSeparator14.Name = "toolStripSeparator14";
            this.toolStripSeparator14.Size = new System.Drawing.Size(184, 6);
            // 
            // printMenuItem
            // 
            this.printMenuItem.Enabled = false;
            this.printMenuItem.Name = "printMenuItem";
            this.printMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.P)));
            this.printMenuItem.Size = new System.Drawing.Size(187, 22);
            this.printMenuItem.Text = "Print";
            // 
            // printPrevMenuItem
            // 
            this.printPrevMenuItem.Enabled = false;
            this.printPrevMenuItem.Name = "printPrevMenuItem";
            this.printPrevMenuItem.Size = new System.Drawing.Size(187, 22);
            this.printPrevMenuItem.Text = "Print Preview";
            // 
            // pageSetMenuItem
            // 
            this.pageSetMenuItem.Enabled = false;
            this.pageSetMenuItem.Name = "pageSetMenuItem";
            this.pageSetMenuItem.Size = new System.Drawing.Size(187, 22);
            this.pageSetMenuItem.Text = "Page Setup";
            // 
            // toolStripSeparator15
            // 
            this.toolStripSeparator15.Name = "toolStripSeparator15";
            this.toolStripSeparator15.Size = new System.Drawing.Size(184, 6);
            // 
            // recentFilesMenuItem
            // 
            this.recentFilesMenuItem.Name = "recentFilesMenuItem";
            this.recentFilesMenuItem.Size = new System.Drawing.Size(187, 22);
            this.recentFilesMenuItem.Text = "Recent Files";
            // 
            // toolStripSeparator16
            // 
            this.toolStripSeparator16.Name = "toolStripSeparator16";
            this.toolStripSeparator16.Size = new System.Drawing.Size(184, 6);
            // 
            // exitMenuItem
            // 
            this.exitMenuItem.Name = "exitMenuItem";
            this.exitMenuItem.Size = new System.Drawing.Size(187, 22);
            this.exitMenuItem.Text = "Exit";
            this.exitMenuItem.Click += new System.EventHandler(this.exitMenuItem_Click);
            // 
            // editMenuItem
            // 
            this.editMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.undoMenuItem,
            this.redoMenuItem,
            this.toolStripSeparator10,
            this.cutMenuItem,
            this.copyMenuItem,
            this.pasteMenuItem,
            this.selectAllMenuItem,
            this.toolStripSeparator11,
            this.menuItem7,
            this.toolStripSeparator12,
            this.insertMenuItem,
            this.formattingMenuItem,
            this.bookmarkMenuItem,
            this.gotoMenuItem,
            this.toolStripSeparator13,
            this.prefsMenuItem});
            this.editMenuItem.Name = "editMenuItem";
            this.editMenuItem.Size = new System.Drawing.Size(39, 20);
            this.editMenuItem.Text = "Edit";
            // 
            // undoMenuItem
            // 
            this.undoMenuItem.Name = "undoMenuItem";
            this.undoMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.undoMenuItem.Size = new System.Drawing.Size(164, 22);
            this.undoMenuItem.Text = "Undo";
            this.undoMenuItem.Click += new System.EventHandler(this.undoMenuItem_Click);
            // 
            // redoMenuItem
            // 
            this.redoMenuItem.Name = "redoMenuItem";
            this.redoMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
            this.redoMenuItem.Size = new System.Drawing.Size(164, 22);
            this.redoMenuItem.Text = "Redo";
            this.redoMenuItem.Click += new System.EventHandler(this.redoMenuItem_Click);
            // 
            // toolStripSeparator10
            // 
            this.toolStripSeparator10.Name = "toolStripSeparator10";
            this.toolStripSeparator10.Size = new System.Drawing.Size(161, 6);
            // 
            // cutMenuItem
            // 
            this.cutMenuItem.Name = "cutMenuItem";
            this.cutMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
            this.cutMenuItem.Size = new System.Drawing.Size(164, 22);
            this.cutMenuItem.Text = "Cut";
            this.cutMenuItem.Click += new System.EventHandler(this.cutMenuItem_Click);
            // 
            // copyMenuItem
            // 
            this.copyMenuItem.Name = "copyMenuItem";
            this.copyMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
            this.copyMenuItem.Size = new System.Drawing.Size(164, 22);
            this.copyMenuItem.Text = "Copy";
            this.copyMenuItem.Click += new System.EventHandler(this.copyMenuItem_Click);
            // 
            // pasteMenuItem
            // 
            this.pasteMenuItem.Name = "pasteMenuItem";
            this.pasteMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.V)));
            this.pasteMenuItem.Size = new System.Drawing.Size(164, 22);
            this.pasteMenuItem.Text = "Paste";
            this.pasteMenuItem.Click += new System.EventHandler(this.pasteMenuItem_Click);
            // 
            // selectAllMenuItem
            // 
            this.selectAllMenuItem.Name = "selectAllMenuItem";
            this.selectAllMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.A)));
            this.selectAllMenuItem.Size = new System.Drawing.Size(164, 22);
            this.selectAllMenuItem.Text = "Select All";
            this.selectAllMenuItem.Click += new System.EventHandler(this.selectAllMenuItem_Click);
            // 
            // toolStripSeparator11
            // 
            this.toolStripSeparator11.Name = "toolStripSeparator11";
            this.toolStripSeparator11.Size = new System.Drawing.Size(161, 6);
            // 
            // menuItem7
            // 
            this.menuItem7.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.findMenuItem,
            this.replaceMenuItem,
            this.findInFilesMenuItem,
            this.replaceInFilesMenuItem,
            this.findAllRefsMenuItem});
            this.menuItem7.Name = "menuItem7";
            this.menuItem7.Size = new System.Drawing.Size(164, 22);
            this.menuItem7.Text = "Find and Replace";
            // 
            // findMenuItem
            // 
            this.findMenuItem.Name = "findMenuItem";
            this.findMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.F)));
            this.findMenuItem.Size = new System.Drawing.Size(227, 22);
            this.findMenuItem.Text = "Find";
            this.findMenuItem.Click += new System.EventHandler(this.findMenuItem_Click);
            // 
            // replaceMenuItem
            // 
            this.replaceMenuItem.Name = "replaceMenuItem";
            this.replaceMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.H)));
            this.replaceMenuItem.Size = new System.Drawing.Size(227, 22);
            this.replaceMenuItem.Text = "Replace";
            this.replaceMenuItem.Click += new System.EventHandler(this.replaceMenuItem_Click);
            // 
            // findInFilesMenuItem
            // 
            this.findInFilesMenuItem.Name = "findInFilesMenuItem";
            this.findInFilesMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.F)));
            this.findInFilesMenuItem.Size = new System.Drawing.Size(227, 22);
            this.findInFilesMenuItem.Text = "Find in files";
            this.findInFilesMenuItem.Click += new System.EventHandler(this.findInFilesMenuItem_Click);
            // 
            // replaceInFilesMenuItem
            // 
            this.replaceInFilesMenuItem.Name = "replaceInFilesMenuItem";
            this.replaceInFilesMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.H)));
            this.replaceInFilesMenuItem.Size = new System.Drawing.Size(227, 22);
            this.replaceInFilesMenuItem.Text = "Replace in files";
            this.replaceInFilesMenuItem.Click += new System.EventHandler(this.replaceInFilesMenuItem_Click);
            // 
            // findAllRefsMenuItem
            // 
            this.findAllRefsMenuItem.Name = "findAllRefsMenuItem";
            this.findAllRefsMenuItem.Size = new System.Drawing.Size(227, 22);
            this.findAllRefsMenuItem.Text = "Find all references";
            this.findAllRefsMenuItem.Click += new System.EventHandler(this.findAllRefsMenuItem_Click);
            // 
            // toolStripSeparator12
            // 
            this.toolStripSeparator12.Name = "toolStripSeparator12";
            this.toolStripSeparator12.Size = new System.Drawing.Size(161, 6);
            // 
            // insertMenuItem
            // 
            this.insertMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.iSpriteMenuItem,
            this.iMapMenuItem});
            this.insertMenuItem.Name = "insertMenuItem";
            this.insertMenuItem.Size = new System.Drawing.Size(164, 22);
            this.insertMenuItem.Text = "Insert";
            // 
            // iSpriteMenuItem
            // 
            this.iSpriteMenuItem.Name = "iSpriteMenuItem";
            this.iSpriteMenuItem.Size = new System.Drawing.Size(104, 22);
            this.iSpriteMenuItem.Text = "Sprite";
            // 
            // iMapMenuItem
            // 
            this.iMapMenuItem.Name = "iMapMenuItem";
            this.iMapMenuItem.Size = new System.Drawing.Size(104, 22);
            this.iMapMenuItem.Text = "Map";
            // 
            // formattingMenuItem
            // 
            this.formattingMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.makeUpperMenuItem,
            this.makeLowerMenuItem,
            this.invertCaseMenuItem,
            this.sentenceCaseMenuItem,
            this.toolStripSeparator21,
            this.formatDocMenuItem,
            this.convertSpacesToTabsMenuItem});
            this.formattingMenuItem.Name = "formattingMenuItem";
            this.formattingMenuItem.Size = new System.Drawing.Size(164, 22);
            this.formattingMenuItem.Text = "Formatting";
            // 
            // makeUpperMenuItem
            // 
            this.makeUpperMenuItem.Name = "makeUpperMenuItem";
            this.makeUpperMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.U)));
            this.makeUpperMenuItem.Size = new System.Drawing.Size(235, 22);
            this.makeUpperMenuItem.Text = "Make Uppercase";
            this.makeUpperMenuItem.Click += new System.EventHandler(this.makeUpperMenuItem_Click);
            // 
            // makeLowerMenuItem
            // 
            this.makeLowerMenuItem.Name = "makeLowerMenuItem";
            this.makeLowerMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.L)));
            this.makeLowerMenuItem.Size = new System.Drawing.Size(235, 22);
            this.makeLowerMenuItem.Text = "Make Lowercase";
            this.makeLowerMenuItem.Click += new System.EventHandler(this.makeLowerMenuItem_Click);
            // 
            // invertCaseMenuItem
            // 
            this.invertCaseMenuItem.Name = "invertCaseMenuItem";
            this.invertCaseMenuItem.Size = new System.Drawing.Size(235, 22);
            this.invertCaseMenuItem.Text = "Invert Case";
            this.invertCaseMenuItem.Click += new System.EventHandler(this.invertCaseMenuItem_Click);
            // 
            // sentenceCaseMenuItem
            // 
            this.sentenceCaseMenuItem.Name = "sentenceCaseMenuItem";
            this.sentenceCaseMenuItem.Size = new System.Drawing.Size(235, 22);
            this.sentenceCaseMenuItem.Text = "Sentence Case";
            this.sentenceCaseMenuItem.Click += new System.EventHandler(this.sentenceCaseMenuItem_Click);
            // 
            // toolStripSeparator21
            // 
            this.toolStripSeparator21.Name = "toolStripSeparator21";
            this.toolStripSeparator21.Size = new System.Drawing.Size(232, 6);
            // 
            // formatDocMenuItem
            // 
            this.formatDocMenuItem.Name = "formatDocMenuItem";
            this.formatDocMenuItem.Size = new System.Drawing.Size(235, 22);
            this.formatDocMenuItem.Text = "Format Document";
            this.formatDocMenuItem.Click += new System.EventHandler(this.formatDocMenuItem_Click);
            // 
            // convertSpacesToTabsMenuItem
            // 
            this.convertSpacesToTabsMenuItem.Name = "convertSpacesToTabsMenuItem";
            this.convertSpacesToTabsMenuItem.Size = new System.Drawing.Size(235, 22);
            this.convertSpacesToTabsMenuItem.Text = "Convert Spaces To Tabs";
            this.convertSpacesToTabsMenuItem.Click += new System.EventHandler(this.convertSpacesToTabsMenuItem_Click);
            // 
            // bookmarkMenuItem
            // 
            this.bookmarkMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toggleBookmarkMenuItem,
            this.prevBookmarkMenuItem,
            this.nextBookmarkMenuItem});
            this.bookmarkMenuItem.Name = "bookmarkMenuItem";
            this.bookmarkMenuItem.Size = new System.Drawing.Size(164, 22);
            this.bookmarkMenuItem.Text = "Bookmarks";
            // 
            // toggleBookmarkMenuItem
            // 
            this.toggleBookmarkMenuItem.Name = "toggleBookmarkMenuItem";
            this.toggleBookmarkMenuItem.Size = new System.Drawing.Size(176, 22);
            this.toggleBookmarkMenuItem.Text = "Toggle Bookmark";
            this.toggleBookmarkMenuItem.Click += new System.EventHandler(this.toggleBookmarkMenuItem_Click);
            // 
            // prevBookmarkMenuItem
            // 
            this.prevBookmarkMenuItem.Name = "prevBookmarkMenuItem";
            this.prevBookmarkMenuItem.Size = new System.Drawing.Size(176, 22);
            this.prevBookmarkMenuItem.Text = "Previous Bookmark";
            this.prevBookmarkMenuItem.Click += new System.EventHandler(this.prevBookmarkMenuItem_Click);
            // 
            // nextBookmarkMenuItem
            // 
            this.nextBookmarkMenuItem.Name = "nextBookmarkMenuItem";
            this.nextBookmarkMenuItem.Size = new System.Drawing.Size(176, 22);
            this.nextBookmarkMenuItem.Text = "Next Bookmark";
            this.nextBookmarkMenuItem.Click += new System.EventHandler(this.nextBookmarkMenuItem_Click);
            // 
            // gotoMenuItem
            // 
            this.gotoMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.gLineMenuItem,
            this.gSymbolMenuItem});
            this.gotoMenuItem.Name = "gotoMenuItem";
            this.gotoMenuItem.Size = new System.Drawing.Size(164, 22);
            this.gotoMenuItem.Text = "Goto";
            // 
            // gLineMenuItem
            // 
            this.gLineMenuItem.Name = "gLineMenuItem";
            this.gLineMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.G)));
            this.gLineMenuItem.Size = new System.Drawing.Size(155, 22);
            this.gLineMenuItem.Text = "Line";
            this.gLineMenuItem.Click += new System.EventHandler(this.gLineMenuItem_Click);
            // 
            // gSymbolMenuItem
            // 
            this.gSymbolMenuItem.Name = "gSymbolMenuItem";
            this.gSymbolMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.T)));
            this.gSymbolMenuItem.Size = new System.Drawing.Size(155, 22);
            this.gSymbolMenuItem.Text = "Symbol";
            this.gSymbolMenuItem.Click += new System.EventHandler(this.gLabelMenuItem_Click);
            // 
            // toolStripSeparator13
            // 
            this.toolStripSeparator13.Name = "toolStripSeparator13";
            this.toolStripSeparator13.Size = new System.Drawing.Size(161, 6);
            // 
            // prefsMenuItem
            // 
            this.prefsMenuItem.Name = "prefsMenuItem";
            this.prefsMenuItem.Size = new System.Drawing.Size(164, 22);
            this.prefsMenuItem.Text = "Preferences";
            this.prefsMenuItem.Click += new System.EventHandler(this.prefsMenuItem_Click);
            // 
            // viewMenuItem
            // 
            this.viewMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolBarMenuItem,
            this.toolStripSeparator6,
            this.labelListMenuItem,
            this.projViewMenuItem,
            this.macroManagerMenuItem,
            this.toolStripSeparator7,
            this.debugPanelsMenuItem,
            this.toolStripSeparator8,
            this.outWinMenuItem,
            this.errListMenuItem,
            this.findResultsMenuItem,
            this.statusBarMenuItem,
            this.toolStripSeparator9,
            this.lineNumMenuItem,
            this.iconBarMenuItem});
            this.viewMenuItem.Name = "viewMenuItem";
            this.viewMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewMenuItem.Text = "View";
            // 
            // toolBarMenuItem
            // 
            this.toolBarMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mainToolMenuItem,
            this.debugToolMenuItem,
            this.editorToolBarMenuItem});
            this.toolBarMenuItem.Name = "toolBarMenuItem";
            this.toolBarMenuItem.Size = new System.Drawing.Size(159, 22);
            this.toolBarMenuItem.Text = "Toolbars";
            // 
            // mainToolMenuItem
            // 
            this.mainToolMenuItem.Name = "mainToolMenuItem";
            this.mainToolMenuItem.Size = new System.Drawing.Size(153, 22);
            this.mainToolMenuItem.Tag = "mainToolBar";
            this.mainToolMenuItem.Text = "Main Toolbar";
            this.mainToolMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // debugToolMenuItem
            // 
            this.debugToolMenuItem.Name = "debugToolMenuItem";
            this.debugToolMenuItem.Size = new System.Drawing.Size(153, 22);
            this.debugToolMenuItem.Tag = "debugToolBar";
            this.debugToolMenuItem.Text = "Debug Toolbar";
            this.debugToolMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // editorToolBarMenuItem
            // 
            this.editorToolBarMenuItem.Name = "editorToolBarMenuItem";
            this.editorToolBarMenuItem.Size = new System.Drawing.Size(153, 22);
            this.editorToolBarMenuItem.Tag = "editorToolbar";
            this.editorToolBarMenuItem.Text = "Editor Toolbar";
            this.editorToolBarMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(156, 6);
            // 
            // labelListMenuItem
            // 
            this.labelListMenuItem.Name = "labelListMenuItem";
            this.labelListMenuItem.Size = new System.Drawing.Size(159, 22);
            this.labelListMenuItem.Tag = "labelsList";
            this.labelListMenuItem.Text = "Label List";
            this.labelListMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // projViewMenuItem
            // 
            this.projViewMenuItem.Name = "projViewMenuItem";
            this.projViewMenuItem.Size = new System.Drawing.Size(159, 22);
            this.projViewMenuItem.Tag = "projectViewer";
            this.projViewMenuItem.Text = "Project View";
            this.projViewMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // macroManagerMenuItem
            // 
            this.macroManagerMenuItem.Name = "macroManagerMenuItem";
            this.macroManagerMenuItem.Size = new System.Drawing.Size(159, 22);
            this.macroManagerMenuItem.Tag = "macroManager";
            this.macroManagerMenuItem.Text = "Macro Manager";
            this.macroManagerMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // toolStripSeparator7
            // 
            this.toolStripSeparator7.Name = "toolStripSeparator7";
            this.toolStripSeparator7.Size = new System.Drawing.Size(156, 6);
            // 
            // debugPanelsMenuItem
            // 
            this.debugPanelsMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.debugPanelMenuItem,
            this.callStackMenuItem,
            this.stackViewerMenuItem,
            this.varTrackMenuItem,
            this.breakManagerMenuItem});
            this.debugPanelsMenuItem.Name = "debugPanelsMenuItem";
            this.debugPanelsMenuItem.Size = new System.Drawing.Size(159, 22);
            this.debugPanelsMenuItem.Text = "Debug";
            // 
            // debugPanelMenuItem
            // 
            this.debugPanelMenuItem.Name = "debugPanelMenuItem";
            this.debugPanelMenuItem.Size = new System.Drawing.Size(181, 22);
            this.debugPanelMenuItem.Tag = "debugPanel";
            this.debugPanelMenuItem.Text = "Debug Panel";
            this.debugPanelMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // callStackMenuItem
            // 
            this.callStackMenuItem.Name = "callStackMenuItem";
            this.callStackMenuItem.Size = new System.Drawing.Size(181, 22);
            this.callStackMenuItem.Tag = "callStack";
            this.callStackMenuItem.Text = "Call MachineStack";
            this.callStackMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // stackViewerMenuItem
            // 
            this.stackViewerMenuItem.Name = "stackViewerMenuItem";
            this.stackViewerMenuItem.Size = new System.Drawing.Size(181, 22);
            this.stackViewerMenuItem.Tag = "stackViewer";
            this.stackViewerMenuItem.Text = "MachineStack Viewer";
            this.stackViewerMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // varTrackMenuItem
            // 
            this.varTrackMenuItem.Name = "varTrackMenuItem";
            this.varTrackMenuItem.Size = new System.Drawing.Size(181, 22);
            this.varTrackMenuItem.Tag = "varTrack";
            this.varTrackMenuItem.Text = "Variable Tracking";
            this.varTrackMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // breakManagerMenuItem
            // 
            this.breakManagerMenuItem.Name = "breakManagerMenuItem";
            this.breakManagerMenuItem.Size = new System.Drawing.Size(181, 22);
            this.breakManagerMenuItem.Tag = "breakManager";
            this.breakManagerMenuItem.Text = "Breakpoint Manager";
            this.breakManagerMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // toolStripSeparator8
            // 
            this.toolStripSeparator8.Name = "toolStripSeparator8";
            this.toolStripSeparator8.Size = new System.Drawing.Size(156, 6);
            // 
            // outWinMenuItem
            // 
            this.outWinMenuItem.Name = "outWinMenuItem";
            this.outWinMenuItem.Size = new System.Drawing.Size(159, 22);
            this.outWinMenuItem.Tag = "outputWindow";
            this.outWinMenuItem.Text = "Output Window";
            this.outWinMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // errListMenuItem
            // 
            this.errListMenuItem.Name = "errListMenuItem";
            this.errListMenuItem.Size = new System.Drawing.Size(159, 22);
            this.errListMenuItem.Tag = "errorList";
            this.errListMenuItem.Text = "Error List";
            this.errListMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // findResultsMenuItem
            // 
            this.findResultsMenuItem.Name = "findResultsMenuItem";
            this.findResultsMenuItem.Size = new System.Drawing.Size(159, 22);
            this.findResultsMenuItem.Tag = "FindResults";
            this.findResultsMenuItem.Text = "Find Results";
            this.findResultsMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // statusBarMenuItem
            // 
            this.statusBarMenuItem.Name = "statusBarMenuItem";
            this.statusBarMenuItem.Size = new System.Drawing.Size(159, 22);
            this.statusBarMenuItem.Tag = "statusBar";
            this.statusBarMenuItem.Text = "Status Bar";
            this.statusBarMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // toolStripSeparator9
            // 
            this.toolStripSeparator9.Name = "toolStripSeparator9";
            this.toolStripSeparator9.Size = new System.Drawing.Size(156, 6);
            // 
            // lineNumMenuItem
            // 
            this.lineNumMenuItem.Name = "lineNumMenuItem";
            this.lineNumMenuItem.Size = new System.Drawing.Size(159, 22);
            this.lineNumMenuItem.Tag = "lineNumbers";
            this.lineNumMenuItem.Text = "Line Numbers";
            this.lineNumMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // iconBarMenuItem
            // 
            this.iconBarMenuItem.Name = "iconBarMenuItem";
            this.iconBarMenuItem.Size = new System.Drawing.Size(159, 22);
            this.iconBarMenuItem.Tag = "iconBar";
            this.iconBarMenuItem.Text = "Icon Bar";
            this.iconBarMenuItem.Click += new System.EventHandler(this.viewMenuItem_Click);
            // 
            // refactorMenuItem
            // 
            this.refactorMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.renameMenuItem,
            this.extractMethodMenuItem});
            this.refactorMenuItem.Name = "refactorMenuItem";
            this.refactorMenuItem.Size = new System.Drawing.Size(63, 20);
            this.refactorMenuItem.Text = "Refactor";
            // 
            // renameMenuItem
            // 
            this.renameMenuItem.Name = "renameMenuItem";
            this.renameMenuItem.Size = new System.Drawing.Size(154, 22);
            this.renameMenuItem.Text = "Rename";
            this.renameMenuItem.Click += new System.EventHandler(this.renameMenuItem_Click);
            // 
            // extractMethodMenuItem
            // 
            this.extractMethodMenuItem.Name = "extractMethodMenuItem";
            this.extractMethodMenuItem.Size = new System.Drawing.Size(154, 22);
            this.extractMethodMenuItem.Text = "Extract Method";
            this.extractMethodMenuItem.Click += new System.EventHandler(this.extractMethodMenuItem_Click);
            // 
            // projMenuItem
            // 
            this.projMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addNewFileMenuItem,
            this.existingFileMenuItem,
            this.toolStripSeparator17,
            this.buildOrderButton,
            this.includeDirProjMenuItem,
            this.refreshMenuItem,
            this.toolStripSeparator18,
            this.closeProjMenuItem});
            this.projMenuItem.Name = "projMenuItem";
            this.projMenuItem.Size = new System.Drawing.Size(56, 20);
            this.projMenuItem.Text = "Project";
            this.projMenuItem.Visible = false;
            // 
            // addNewFileMenuItem
            // 
            this.addNewFileMenuItem.Name = "addNewFileMenuItem";
            this.addNewFileMenuItem.Size = new System.Drawing.Size(181, 22);
            this.addNewFileMenuItem.Text = "Add File";
            this.addNewFileMenuItem.Click += new System.EventHandler(this.addNewFileMenuItem_Click);
            // 
            // existingFileMenuItem
            // 
            this.existingFileMenuItem.Name = "existingFileMenuItem";
            this.existingFileMenuItem.Size = new System.Drawing.Size(181, 22);
            this.existingFileMenuItem.Text = "Add Existing File";
            this.existingFileMenuItem.Click += new System.EventHandler(this.existingFileMenuItem_Click);
            // 
            // toolStripSeparator17
            // 
            this.toolStripSeparator17.Name = "toolStripSeparator17";
            this.toolStripSeparator17.Size = new System.Drawing.Size(178, 6);
            // 
            // buildOrderButton
            // 
            this.buildOrderButton.Name = "buildOrderButton";
            this.buildOrderButton.Size = new System.Drawing.Size(181, 22);
            this.buildOrderButton.Text = "Build Steps...";
            this.buildOrderButton.Click += new System.EventHandler(this.buildOrderButton_Click);
            // 
            // includeDirProjMenuItem
            // 
            this.includeDirProjMenuItem.Name = "includeDirProjMenuItem";
            this.includeDirProjMenuItem.Size = new System.Drawing.Size(181, 22);
            this.includeDirProjMenuItem.Text = "Include Directories...";
            this.includeDirProjMenuItem.Click += new System.EventHandler(this.includeDirButton_Click);
            // 
            // refreshMenuItem
            // 
            this.refreshMenuItem.Name = "refreshMenuItem";
            this.refreshMenuItem.Size = new System.Drawing.Size(181, 22);
            this.refreshMenuItem.Text = "Refresh";
            this.refreshMenuItem.Click += new System.EventHandler(this.refreshViewMenuItem_Click);
            // 
            // toolStripSeparator18
            // 
            this.toolStripSeparator18.Name = "toolStripSeparator18";
            this.toolStripSeparator18.Size = new System.Drawing.Size(178, 6);
            // 
            // closeProjMenuItem
            // 
            this.closeProjMenuItem.Name = "closeProjMenuItem";
            this.closeProjMenuItem.Size = new System.Drawing.Size(181, 22);
            this.closeProjMenuItem.Text = "Close Project";
            this.closeProjMenuItem.Click += new System.EventHandler(this.closeProjMenuItem_Click);
            // 
            // asmMenuItem
            // 
            this.asmMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.assembleMenuItem,
            this.includeDirButton,
            this.symTableMenuItem,
            this.countCodeMenuItem,
            this.projStatsMenuItem,
            this.listFileMenuItem,
            this.hexFileMenuItem});
            this.asmMenuItem.Name = "asmMenuItem";
            this.asmMenuItem.Size = new System.Drawing.Size(70, 20);
            this.asmMenuItem.Text = "Assemble";
            // 
            // assembleMenuItem
            // 
            this.assembleMenuItem.Name = "assembleMenuItem";
            this.assembleMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.B)));
            this.assembleMenuItem.Size = new System.Drawing.Size(180, 22);
            this.assembleMenuItem.Text = "Assemble";
            this.assembleMenuItem.Click += new System.EventHandler(this.assembleMenuItem_Click);
            // 
            // includeDirButton
            // 
            this.includeDirButton.Name = "includeDirButton";
            this.includeDirButton.Size = new System.Drawing.Size(180, 22);
            this.includeDirButton.Text = "Include Directories";
            this.includeDirButton.Click += new System.EventHandler(this.includeDirButton_Click);
            // 
            // symTableMenuItem
            // 
            this.symTableMenuItem.Name = "symTableMenuItem";
            this.symTableMenuItem.Size = new System.Drawing.Size(180, 22);
            this.symTableMenuItem.Text = "Build Symbol Table";
            this.symTableMenuItem.Click += new System.EventHandler(this.symTableMenuItem_Click);
            // 
            // countCodeMenuItem
            // 
            this.countCodeMenuItem.Enabled = false;
            this.countCodeMenuItem.Name = "countCodeMenuItem";
            this.countCodeMenuItem.Size = new System.Drawing.Size(180, 22);
            this.countCodeMenuItem.Text = "Count Code";
            // 
            // projStatsMenuItem
            // 
            this.projStatsMenuItem.Name = "projStatsMenuItem";
            this.projStatsMenuItem.Size = new System.Drawing.Size(180, 22);
            this.projStatsMenuItem.Text = "Show Program Stats";
            this.projStatsMenuItem.Click += new System.EventHandler(this.projStatsMenuItem_Click);
            // 
            // listFileMenuItem
            // 
            this.listFileMenuItem.Name = "listFileMenuItem";
            this.listFileMenuItem.Size = new System.Drawing.Size(180, 22);
            this.listFileMenuItem.Text = "Build List File";
            this.listFileMenuItem.Click += new System.EventHandler(this.listFileMenuItem_Click);
            // 
            // hexFileMenuItem
            // 
            this.hexFileMenuItem.Enabled = false;
            this.hexFileMenuItem.Name = "hexFileMenuItem";
            this.hexFileMenuItem.Size = new System.Drawing.Size(180, 22);
            this.hexFileMenuItem.Text = "Build Hex File";
            // 
            // macrosMenuItem
            // 
            this.macrosMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.runMacroMenuItem,
            this.recMacroMenuItem,
            this.manMacroMenuItem});
            this.macrosMenuItem.Enabled = false;
            this.macrosMenuItem.Name = "macrosMenuItem";
            this.macrosMenuItem.Size = new System.Drawing.Size(58, 20);
            this.macrosMenuItem.Text = "Macros";
            // 
            // runMacroMenuItem
            // 
            this.runMacroMenuItem.Name = "runMacroMenuItem";
            this.runMacroMenuItem.Size = new System.Drawing.Size(159, 22);
            this.runMacroMenuItem.Text = "Run Macro";
            // 
            // recMacroMenuItem
            // 
            this.recMacroMenuItem.Name = "recMacroMenuItem";
            this.recMacroMenuItem.Size = new System.Drawing.Size(159, 22);
            this.recMacroMenuItem.Text = "Record Macro";
            // 
            // manMacroMenuItem
            // 
            this.manMacroMenuItem.Name = "manMacroMenuItem";
            this.manMacroMenuItem.Size = new System.Drawing.Size(159, 22);
            this.manMacroMenuItem.Text = "Manage Macros";
            // 
            // debugMenuItem
            // 
            this.debugMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.startDebugMenuItem,
            this.startWithoutDebugMenuItem,
            this.stopDebugMenuItem,
            this.toolStripSeparator19,
            this.runMenuItem,
            this.stepMenuItem,
            this.stepOverMenuItem,
            this.stepOutMenuItem,
            this.toolStripSeparator20,
            this.menuItem11,
            this.toggleBreakpointMenuItem,
            this.deleteAllBreakpointsMenuItem,
            this.disableAllBreakpointsMenuItem});
            this.debugMenuItem.Name = "debugMenuItem";
            this.debugMenuItem.Size = new System.Drawing.Size(54, 20);
            this.debugMenuItem.Text = "Debug";
            // 
            // startDebugMenuItem
            // 
            this.startDebugMenuItem.Name = "startDebugMenuItem";
            this.startDebugMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F5;
            this.startDebugMenuItem.Size = new System.Drawing.Size(228, 22);
            this.startDebugMenuItem.Text = "Start Debug";
            this.startDebugMenuItem.Click += new System.EventHandler(this.startDebugMenuItem_Click);
            // 
            // startWithoutDebugMenuItem
            // 
            this.startWithoutDebugMenuItem.Name = "startWithoutDebugMenuItem";
            this.startWithoutDebugMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.F5)));
            this.startWithoutDebugMenuItem.Size = new System.Drawing.Size(228, 22);
            this.startWithoutDebugMenuItem.Text = "Start Without Debug";
            this.startWithoutDebugMenuItem.Click += new System.EventHandler(this.startWithoutDebugMenuItem_Click);
            // 
            // stopDebugMenuItem
            // 
            this.stopDebugMenuItem.Enabled = false;
            this.stopDebugMenuItem.Name = "stopDebugMenuItem";
            this.stopDebugMenuItem.Size = new System.Drawing.Size(228, 22);
            this.stopDebugMenuItem.Text = "Stop Debug";
            this.stopDebugMenuItem.Click += new System.EventHandler(this.cancelDebug_Click);
            // 
            // toolStripSeparator19
            // 
            this.toolStripSeparator19.Name = "toolStripSeparator19";
            this.toolStripSeparator19.Size = new System.Drawing.Size(225, 6);
            // 
            // runMenuItem
            // 
            this.runMenuItem.Enabled = false;
            this.runMenuItem.Name = "runMenuItem";
            this.runMenuItem.Size = new System.Drawing.Size(228, 22);
            this.runMenuItem.Text = "Run";
            this.runMenuItem.Click += new System.EventHandler(this.startDebugMenuItem_Click);
            // 
            // stepMenuItem
            // 
            this.stepMenuItem.Enabled = false;
            this.stepMenuItem.Name = "stepMenuItem";
            this.stepMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.stepMenuItem.Size = new System.Drawing.Size(228, 22);
            this.stepMenuItem.Text = "Step";
            this.stepMenuItem.Click += new System.EventHandler(this.stepButton_Click);
            // 
            // stepOverMenuItem
            // 
            this.stepOverMenuItem.Enabled = false;
            this.stepOverMenuItem.Name = "stepOverMenuItem";
            this.stepOverMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F8;
            this.stepOverMenuItem.Size = new System.Drawing.Size(228, 22);
            this.stepOverMenuItem.Text = "Step Over";
            this.stepOverMenuItem.Click += new System.EventHandler(this.stepOverMenuItem_Click);
            // 
            // stepOutMenuItem
            // 
            this.stepOutMenuItem.Enabled = false;
            this.stepOutMenuItem.Name = "stepOutMenuItem";
            this.stepOutMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Shift | System.Windows.Forms.Keys.F8)));
            this.stepOutMenuItem.Size = new System.Drawing.Size(228, 22);
            this.stepOutMenuItem.Text = "Step Out";
            this.stepOutMenuItem.Click += new System.EventHandler(this.stepOutMenuItem_Click);
            // 
            // toolStripSeparator20
            // 
            this.toolStripSeparator20.Name = "toolStripSeparator20";
            this.toolStripSeparator20.Size = new System.Drawing.Size(225, 6);
            // 
            // menuItem11
            // 
            this.menuItem11.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newBreakpointMenuItem,
            this.newDataBreakpointMenuItem});
            this.menuItem11.Name = "menuItem11";
            this.menuItem11.Size = new System.Drawing.Size(228, 22);
            this.menuItem11.Text = "New Breakpoint";
            // 
            // newBreakpointMenuItem
            // 
            this.newBreakpointMenuItem.Name = "newBreakpointMenuItem";
            this.newBreakpointMenuItem.Size = new System.Drawing.Size(158, 22);
            this.newBreakpointMenuItem.Text = "New Breakpoint";
            this.newBreakpointMenuItem.Click += new System.EventHandler(this.newBreakpointMenuItem_Click);
            // 
            // newDataBreakpointMenuItem
            // 
            this.newDataBreakpointMenuItem.Name = "newDataBreakpointMenuItem";
            this.newDataBreakpointMenuItem.Size = new System.Drawing.Size(158, 22);
            this.newDataBreakpointMenuItem.Text = "Data Breakpoint";
            // 
            // toggleBreakpointMenuItem
            // 
            this.toggleBreakpointMenuItem.Name = "toggleBreakpointMenuItem";
            this.toggleBreakpointMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F9;
            this.toggleBreakpointMenuItem.Size = new System.Drawing.Size(228, 22);
            this.toggleBreakpointMenuItem.Text = "Toggle Breakpoint";
            this.toggleBreakpointMenuItem.Click += new System.EventHandler(this.toggleBreakpointMenuItem_Click);
            // 
            // deleteAllBreakpointsMenuItem
            // 
            this.deleteAllBreakpointsMenuItem.Name = "deleteAllBreakpointsMenuItem";
            this.deleteAllBreakpointsMenuItem.Size = new System.Drawing.Size(228, 22);
            this.deleteAllBreakpointsMenuItem.Text = "Delete All Breakpoints";
            // 
            // disableAllBreakpointsMenuItem
            // 
            this.disableAllBreakpointsMenuItem.Name = "disableAllBreakpointsMenuItem";
            this.disableAllBreakpointsMenuItem.Size = new System.Drawing.Size(228, 22);
            this.disableAllBreakpointsMenuItem.Text = "Disable All Breakpoints";
            // 
            // windowMenuItem
            // 
            this.windowMenuItem.Name = "windowMenuItem";
            this.windowMenuItem.Size = new System.Drawing.Size(63, 20);
            this.windowMenuItem.Text = "Window";
            // 
            // helpMenuItem
            // 
            this.helpMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.showHelpMenuItem,
            this.updateMenuItem,
            this.aboutMenuItem});
            this.helpMenuItem.Name = "helpMenuItem";
            this.helpMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpMenuItem.Text = "Help";
            // 
            // showHelpMenuItem
            // 
            this.showHelpMenuItem.Enabled = false;
            this.showHelpMenuItem.Name = "showHelpMenuItem";
            this.showHelpMenuItem.Size = new System.Drawing.Size(171, 22);
            this.showHelpMenuItem.Text = "Help";
            // 
            // updateMenuItem
            // 
            this.updateMenuItem.Name = "updateMenuItem";
            this.updateMenuItem.Size = new System.Drawing.Size(171, 22);
            this.updateMenuItem.Text = "Check for Updates";
            this.updateMenuItem.Click += new System.EventHandler(this.updateMenuItem_Click);
            // 
            // aboutMenuItem
            // 
            this.aboutMenuItem.Name = "aboutMenuItem";
            this.aboutMenuItem.Size = new System.Drawing.Size(171, 22);
            this.aboutMenuItem.Text = "About";
            this.aboutMenuItem.Click += new System.EventHandler(this.aboutMenuItem_Click);
            // 
            // toolStripContainer
            // 
            this.toolStripContainer.BottomToolStripPanelVisible = false;
            // 
            // toolStripContainer.ContentPanel
            // 
            this.toolStripContainer.ContentPanel.Controls.Add(this.debugToolStrip);
            this.toolStripContainer.ContentPanel.Size = new System.Drawing.Size(990, 0);
            this.toolStripContainer.Dock = System.Windows.Forms.DockStyle.Top;
            this.toolStripContainer.LeftToolStripPanelVisible = false;
            this.toolStripContainer.Location = new System.Drawing.Point(0, 24);
            this.toolStripContainer.Name = "toolStripContainer";
            this.toolStripContainer.RightToolStripPanelVisible = false;
            this.toolStripContainer.Size = new System.Drawing.Size(990, 25);
            this.toolStripContainer.TabIndex = 9;
            this.toolStripContainer.Text = "toolStripContainer1";
            // 
            // debugToolStrip
            // 
            this.debugToolStrip.AllowItemReorder = true;
            this.debugToolStrip.AllowMerge = false;
            this.debugToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.debugToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.debugToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.runDebuggerToolButton,
            this.pauseToolButton,
            this.stopToolButton,
            this.restartToolStripButton,
            this.toolStripSeparator1,
            this.gotoCurrentToolButton,
            this.stepToolButton,
            this.stepOverToolButton,
            this.stepOutToolButton});
            this.debugToolStrip.Location = new System.Drawing.Point(556, 0);
            this.debugToolStrip.Name = "debugToolStrip";
            this.debugToolStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.debugToolStrip.Size = new System.Drawing.Size(193, 25);
            this.debugToolStrip.TabIndex = 1;
            this.debugToolStrip.Text = "Debug Toolbar";
            this.debugToolStrip.Visible = false;
            // 
            // runDebuggerToolButton
            // 
            this.runDebuggerToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.runDebuggerToolButton.Image = ((System.Drawing.Image)(resources.GetObject("runDebuggerToolButton.Image")));
            this.runDebuggerToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.runDebuggerToolButton.Name = "runDebuggerToolButton";
            this.runDebuggerToolButton.Size = new System.Drawing.Size(23, 22);
            this.runDebuggerToolButton.Text = "Start Debug";
            this.runDebuggerToolButton.Click += new System.EventHandler(this.startDebugMenuItem_Click);
            // 
            // pauseToolButton
            // 
            this.pauseToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.pauseToolButton.Enabled = false;
            this.pauseToolButton.Image = ((System.Drawing.Image)(resources.GetObject("pauseToolButton.Image")));
            this.pauseToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.pauseToolButton.Name = "pauseToolButton";
            this.pauseToolButton.Size = new System.Drawing.Size(23, 22);
            this.pauseToolButton.Text = "Pause";
            this.pauseToolButton.Click += new System.EventHandler(this.pauseToolButton_Click);
            // 
            // stopToolButton
            // 
            this.stopToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.stopToolButton.Enabled = false;
            this.stopToolButton.Image = ((System.Drawing.Image)(resources.GetObject("stopToolButton.Image")));
            this.stopToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.stopToolButton.Name = "stopToolButton";
            this.stopToolButton.Size = new System.Drawing.Size(23, 22);
            this.stopToolButton.Text = "Stop";
            this.stopToolButton.Click += new System.EventHandler(this.cancelDebug_Click);
            // 
            // restartToolStripButton
            // 
            this.restartToolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.restartToolStripButton.Enabled = false;
            this.restartToolStripButton.Image = ((System.Drawing.Image)(resources.GetObject("restartToolStripButton.Image")));
            this.restartToolStripButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.restartToolStripButton.Name = "restartToolStripButton";
            this.restartToolStripButton.Size = new System.Drawing.Size(23, 22);
            this.restartToolStripButton.Text = "Restart";
            this.restartToolStripButton.Click += new System.EventHandler(this.restartToolStripButton_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            this.toolStripSeparator1.TextDirection = System.Windows.Forms.ToolStripTextDirection.Vertical90;
            // 
            // gotoCurrentToolButton
            // 
            this.gotoCurrentToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.gotoCurrentToolButton.Enabled = false;
            this.gotoCurrentToolButton.Image = ((System.Drawing.Image)(resources.GetObject("gotoCurrentToolButton.Image")));
            this.gotoCurrentToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.gotoCurrentToolButton.Name = "gotoCurrentToolButton";
            this.gotoCurrentToolButton.Size = new System.Drawing.Size(23, 22);
            this.gotoCurrentToolButton.Text = "Goto Current Line";
            this.gotoCurrentToolButton.Click += new System.EventHandler(this.gotoCurrentToolButton_Click);
            // 
            // stepToolButton
            // 
            this.stepToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.stepToolButton.Enabled = false;
            this.stepToolButton.Image = ((System.Drawing.Image)(resources.GetObject("stepToolButton.Image")));
            this.stepToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.stepToolButton.Name = "stepToolButton";
            this.stepToolButton.Size = new System.Drawing.Size(23, 22);
            this.stepToolButton.Text = "Step";
            this.stepToolButton.Click += new System.EventHandler(this.stepButton_Click);
            // 
            // stepOverToolButton
            // 
            this.stepOverToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.stepOverToolButton.Enabled = false;
            this.stepOverToolButton.Image = ((System.Drawing.Image)(resources.GetObject("stepOverToolButton.Image")));
            this.stepOverToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.stepOverToolButton.Name = "stepOverToolButton";
            this.stepOverToolButton.Overflow = System.Windows.Forms.ToolStripItemOverflow.Never;
            this.stepOverToolButton.Size = new System.Drawing.Size(23, 22);
            this.stepOverToolButton.Text = "Step Over";
            this.stepOverToolButton.Click += new System.EventHandler(this.stepOverMenuItem_Click);
            // 
            // stepOutToolButton
            // 
            this.stepOutToolButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.stepOutToolButton.Enabled = false;
            this.stepOutToolButton.Image = ((System.Drawing.Image)(resources.GetObject("stepOutToolButton.Image")));
            this.stepOutToolButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.stepOutToolButton.Name = "stepOutToolButton";
            this.stepOutToolButton.Size = new System.Drawing.Size(23, 22);
            this.stepOutToolButton.Text = "Step Out";
            this.stepOutToolButton.Click += new System.EventHandler(this.stepOutMenuItem_Click);
            // 
            // editorToolStrip
            // 
            this.editorToolStrip.AllowMerge = false;
            this.editorToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.editorToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.editorToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.toolStripButton2,
            this.toolStripSeparator4,
            this.toolStripButton3,
            this.toolStripButton4,
            this.toolStripSeparator5,
            this.toolStripButton5,
            this.toolStripButton6,
            this.toolStripButton7,
            this.toolStripButton8});
            this.editorToolStrip.Location = new System.Drawing.Point(648, 24);
            this.editorToolStrip.Name = "editorToolStrip";
            this.editorToolStrip.RenderMode = System.Windows.Forms.ToolStripRenderMode.System;
            this.editorToolStrip.Size = new System.Drawing.Size(199, 25);
            this.editorToolStrip.TabIndex = 1;
            this.editorToolStrip.Text = "Editor Toolbar";
            this.editorToolStrip.Visible = false;
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton1.Text = "toolStripButton1";
            // 
            // toolStripButton2
            // 
            this.toolStripButton2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton2.Name = "toolStripButton2";
            this.toolStripButton2.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton2.Text = "toolStripButton2";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripButton3
            // 
            this.toolStripButton3.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton3.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton3.Name = "toolStripButton3";
            this.toolStripButton3.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton3.Text = "toolStripButton3";
            // 
            // toolStripButton4
            // 
            this.toolStripButton4.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton4.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton4.Name = "toolStripButton4";
            this.toolStripButton4.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton4.Text = "toolStripButton4";
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripButton5
            // 
            this.toolStripButton5.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton5.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton5.Name = "toolStripButton5";
            this.toolStripButton5.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton5.Text = "toolStripButton5";
            // 
            // toolStripButton6
            // 
            this.toolStripButton6.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton6.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton6.Name = "toolStripButton6";
            this.toolStripButton6.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton6.Text = "toolStripButton6";
            // 
            // toolStripButton7
            // 
            this.toolStripButton7.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton7.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton7.Name = "toolStripButton7";
            this.toolStripButton7.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton7.Text = "toolStripButton7";
            // 
            // toolStripButton8
            // 
            this.toolStripButton8.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton8.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton8.Name = "toolStripButton8";
            this.toolStripButton8.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton8.Text = "toolStripButton8";
            // 
            // dockPanel
            // 
            this.dockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dockPanel.DockBackColor = System.Drawing.SystemColors.ControlDark;
            this.dockPanel.DockBottomPortion = 200D;
            this.dockPanel.DockLeftPortion = 200D;
            this.dockPanel.DockRightPortion = 200D;
            this.dockPanel.DockTopPortion = 150D;
            this.dockPanel.Location = new System.Drawing.Point(0, 49);
            this.dockPanel.Name = "dockPanel";
            this.dockPanel.RightToLeftLayout = true;
            this.dockPanel.Size = new System.Drawing.Size(990, 487);
            dockPanelGradient1.EndColor = System.Drawing.SystemColors.ControlDarkDark;
            dockPanelGradient1.StartColor = System.Drawing.SystemColors.ControlDarkDark;
            autoHideStripSkin1.DockStripGradient = dockPanelGradient1;
            tabGradient1.EndColor = System.Drawing.SystemColors.Control;
            tabGradient1.StartColor = System.Drawing.SystemColors.Control;
            tabGradient1.TextColor = System.Drawing.SystemColors.ControlDarkDark;
            autoHideStripSkin1.TabGradient = tabGradient1;
            autoHideStripSkin1.TextFont = new System.Drawing.Font("Segoe UI", 9F);
            dockPanelSkin1.AutoHideStripSkin = autoHideStripSkin1;
            tabGradient2.EndColor = System.Drawing.SystemColors.ControlLightLight;
            tabGradient2.StartColor = System.Drawing.SystemColors.ControlLightLight;
            tabGradient2.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripGradient1.ActiveTabGradient = tabGradient2;
            dockPanelGradient2.EndColor = System.Drawing.SystemColors.Control;
            dockPanelGradient2.StartColor = System.Drawing.SystemColors.Control;
            dockPaneStripGradient1.DockStripGradient = dockPanelGradient2;
            tabGradient3.EndColor = System.Drawing.SystemColors.ControlLight;
            tabGradient3.StartColor = System.Drawing.SystemColors.ControlLight;
            tabGradient3.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripGradient1.InactiveTabGradient = tabGradient3;
            dockPaneStripSkin1.DocumentGradient = dockPaneStripGradient1;
            dockPaneStripSkin1.TextFont = new System.Drawing.Font("Segoe UI", 9F);
            tabGradient4.EndColor = System.Drawing.SystemColors.ActiveCaption;
            tabGradient4.LinearGradientMode = System.Drawing.Drawing2D.LinearGradientMode.Vertical;
            tabGradient4.StartColor = System.Drawing.SystemColors.GradientActiveCaption;
            tabGradient4.TextColor = System.Drawing.SystemColors.ActiveCaptionText;
            dockPaneStripToolWindowGradient1.ActiveCaptionGradient = tabGradient4;
            tabGradient5.EndColor = System.Drawing.SystemColors.Control;
            tabGradient5.StartColor = System.Drawing.SystemColors.Control;
            tabGradient5.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripToolWindowGradient1.ActiveTabGradient = tabGradient5;
            dockPanelGradient3.EndColor = System.Drawing.SystemColors.ControlLight;
            dockPanelGradient3.StartColor = System.Drawing.SystemColors.ControlLight;
            dockPaneStripToolWindowGradient1.DockStripGradient = dockPanelGradient3;
            tabGradient6.EndColor = System.Drawing.SystemColors.GradientInactiveCaption;
            tabGradient6.LinearGradientMode = System.Drawing.Drawing2D.LinearGradientMode.Vertical;
            tabGradient6.StartColor = System.Drawing.SystemColors.GradientInactiveCaption;
            tabGradient6.TextColor = System.Drawing.SystemColors.ControlText;
            dockPaneStripToolWindowGradient1.InactiveCaptionGradient = tabGradient6;
            tabGradient7.EndColor = System.Drawing.Color.Transparent;
            tabGradient7.StartColor = System.Drawing.Color.Transparent;
            tabGradient7.TextColor = System.Drawing.SystemColors.ControlDarkDark;
            dockPaneStripToolWindowGradient1.InactiveTabGradient = tabGradient7;
            dockPaneStripSkin1.ToolWindowGradient = dockPaneStripToolWindowGradient1;
            dockPanelSkin1.DockPaneStripSkin = dockPaneStripSkin1;
            this.dockPanel.Skin = dockPanelSkin1;
            this.dockPanel.SkinStyle = WeifenLuo.WinFormsUI.Docking.Skins.Style.VisualStudio2005;
            this.dockPanel.TabIndex = 0;
            // 
            // MainForm
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(990, 558);
            this.Controls.Add(this.mainToolBar);
            this.Controls.Add(this.editorToolStrip);
            this.Controls.Add(this.dockPanel);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.toolStripContainer);
            this.Controls.Add(this.mainMenu1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.mainMenu1;
            this.Name = "MainForm";
            this.Text = "Wabbitcode";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainFormRedone_FormClosing);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.MainFormRedone_DragDrop);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.MainFormRedone_DragEnter);
            this.statusBar.ResumeLayout(false);
            this.statusBar.PerformLayout();
            this.mainToolBar.ResumeLayout(false);
            this.mainToolBar.PerformLayout();
            this.mainMenu1.ResumeLayout(false);
            this.mainMenu1.PerformLayout();
            this.toolStripContainer.ContentPanel.ResumeLayout(false);
            this.toolStripContainer.ContentPanel.PerformLayout();
            this.toolStripContainer.ResumeLayout(false);
            this.toolStripContainer.PerformLayout();
            this.debugToolStrip.ResumeLayout(false);
            this.debugToolStrip.PerformLayout();
            this.editorToolStrip.ResumeLayout(false);
            this.editorToolStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.StatusStrip statusBar;
		private System.Windows.Forms.ToolStripProgressBar progressBar;
		private System.Windows.Forms.SaveFileDialog saveFileDialog1;
		private System.Windows.Forms.ToolStripStatusLabel lineStatusLabel;
		private System.Windows.Forms.ToolStripStatusLabel colStatusLabel;
		private System.Windows.Forms.ImageList toolBarIcons;
		private WeifenLuo.WinFormsUI.Docking.DockPanel dockPanel;
		private System.Windows.Forms.ToolStripStatusLabel lineCodeInfo;
		private System.Windows.Forms.ToolStrip mainToolBar;
		private System.Windows.Forms.ToolStripButton newToolStripButton;
		private System.Windows.Forms.ToolStripButton openToolStripButton;
		private System.Windows.Forms.ToolStripButton saveToolStripButton;
		private System.Windows.Forms.ToolStripButton printToolStripButton;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator;
		private System.Windows.Forms.ToolStripButton cutToolStripButton;
		private System.Windows.Forms.ToolStripButton copyToolStripButton;
		private System.Windows.Forms.ToolStripButton pasteToolStripButton;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
		private System.Windows.Forms.ToolStripComboBox findBox;
		private System.Windows.Forms.ToolStripComboBox configBox;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
		private System.Windows.Forms.ToolStripButton runToolButton;
		public System.ComponentModel.BackgroundWorker documentParser;
		private System.Windows.Forms.ToolStripButton saveAllToolButton;
		private System.ComponentModel.BackgroundWorker wabbitemu;
		private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel;
		private System.Windows.Forms.MenuStrip mainMenu1;
		private System.Windows.Forms.ToolStripMenuItem fileMenuItem;
		private System.Windows.Forms.ToolStripMenuItem newMenuItem;
		private System.Windows.Forms.ToolStripMenuItem newFileMenuItem;
		private System.Windows.Forms.ToolStripMenuItem newProjectMenuItem;
		private System.Windows.Forms.ToolStripMenuItem openMenuItem;
		private System.Windows.Forms.ToolStripMenuItem openFileMenuItem;
		private System.Windows.Forms.ToolStripMenuItem openProjectMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveAsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveAllMenuItem;
		private System.Windows.Forms.ToolStripMenuItem saveProjectMenuItem;
		private System.Windows.Forms.ToolStripMenuItem closeMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator14;
		private System.Windows.Forms.ToolStripMenuItem printMenuItem;
		private System.Windows.Forms.ToolStripMenuItem printPrevMenuItem;
		private System.Windows.Forms.ToolStripMenuItem pageSetMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator15;
		private System.Windows.Forms.ToolStripMenuItem recentFilesMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator16;
		private System.Windows.Forms.ToolStripMenuItem exitMenuItem;
		private System.Windows.Forms.ToolStripMenuItem editMenuItem;
		private System.Windows.Forms.ToolStripMenuItem undoMenuItem;
		private System.Windows.Forms.ToolStripMenuItem redoMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator10;
		private System.Windows.Forms.ToolStripMenuItem cutMenuItem;
		private System.Windows.Forms.ToolStripMenuItem copyMenuItem;
		private System.Windows.Forms.ToolStripMenuItem pasteMenuItem;
		private System.Windows.Forms.ToolStripMenuItem selectAllMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator11;
		private System.Windows.Forms.ToolStripMenuItem menuItem7;
		private System.Windows.Forms.ToolStripMenuItem findMenuItem;
		private System.Windows.Forms.ToolStripMenuItem replaceMenuItem;
		private System.Windows.Forms.ToolStripMenuItem findInFilesMenuItem;
		private System.Windows.Forms.ToolStripMenuItem replaceInFilesMenuItem;
		private System.Windows.Forms.ToolStripMenuItem findAllRefsMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator12;
		private System.Windows.Forms.ToolStripMenuItem insertMenuItem;
		private System.Windows.Forms.ToolStripMenuItem iSpriteMenuItem;
		private System.Windows.Forms.ToolStripMenuItem iMapMenuItem;
		private System.Windows.Forms.ToolStripMenuItem formattingMenuItem;
		private System.Windows.Forms.ToolStripMenuItem makeUpperMenuItem;
		private System.Windows.Forms.ToolStripMenuItem makeLowerMenuItem;
		private System.Windows.Forms.ToolStripMenuItem invertCaseMenuItem;
		private System.Windows.Forms.ToolStripMenuItem sentenceCaseMenuItem;
		private System.Windows.Forms.ToolStripMenuItem formatDocMenuItem;
		private System.Windows.Forms.ToolStripMenuItem convertSpacesToTabsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem bookmarkMenuItem;
		private System.Windows.Forms.ToolStripMenuItem toggleBookmarkMenuItem;
		private System.Windows.Forms.ToolStripMenuItem prevBookmarkMenuItem;
		private System.Windows.Forms.ToolStripMenuItem nextBookmarkMenuItem;
		private System.Windows.Forms.ToolStripMenuItem gotoMenuItem;
		private System.Windows.Forms.ToolStripMenuItem gLineMenuItem;
		private System.Windows.Forms.ToolStripMenuItem gSymbolMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator13;
		private System.Windows.Forms.ToolStripMenuItem prefsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem viewMenuItem;
		private System.Windows.Forms.ToolStripMenuItem toolBarMenuItem;
		private System.Windows.Forms.ToolStripMenuItem mainToolMenuItem;
		private System.Windows.Forms.ToolStripMenuItem debugToolMenuItem;
		private System.Windows.Forms.ToolStripMenuItem editorToolBarMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
		private System.Windows.Forms.ToolStripMenuItem labelListMenuItem;
        private System.Windows.Forms.ToolStripMenuItem projViewMenuItem;
		private System.Windows.Forms.ToolStripMenuItem macroManagerMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator7;
		private System.Windows.Forms.ToolStripMenuItem debugPanelsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem debugPanelMenuItem;
		private System.Windows.Forms.ToolStripMenuItem callStackMenuItem;
		private System.Windows.Forms.ToolStripMenuItem stackViewerMenuItem;
		private System.Windows.Forms.ToolStripMenuItem varTrackMenuItem;
		private System.Windows.Forms.ToolStripMenuItem breakManagerMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator8;
		private System.Windows.Forms.ToolStripMenuItem outWinMenuItem;
		private System.Windows.Forms.ToolStripMenuItem errListMenuItem;
		private System.Windows.Forms.ToolStripMenuItem findResultsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem statusBarMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator9;
		private System.Windows.Forms.ToolStripMenuItem lineNumMenuItem;
		private System.Windows.Forms.ToolStripMenuItem iconBarMenuItem;
		private System.Windows.Forms.ToolStripMenuItem refactorMenuItem;
		private System.Windows.Forms.ToolStripMenuItem renameMenuItem;
		private System.Windows.Forms.ToolStripMenuItem extractMethodMenuItem;
		private System.Windows.Forms.ToolStripMenuItem projMenuItem;
		private System.Windows.Forms.ToolStripMenuItem addNewFileMenuItem;
		private System.Windows.Forms.ToolStripMenuItem existingFileMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator17;
		private System.Windows.Forms.ToolStripMenuItem buildOrderButton;
		private System.Windows.Forms.ToolStripMenuItem includeDirProjMenuItem;
		private System.Windows.Forms.ToolStripMenuItem refreshMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator18;
		private System.Windows.Forms.ToolStripMenuItem closeProjMenuItem;
		private System.Windows.Forms.ToolStripMenuItem macrosMenuItem;
		private System.Windows.Forms.ToolStripMenuItem runMacroMenuItem;
		private System.Windows.Forms.ToolStripMenuItem recMacroMenuItem;
		private System.Windows.Forms.ToolStripMenuItem manMacroMenuItem;
		private System.Windows.Forms.ToolStripMenuItem asmMenuItem;
		private System.Windows.Forms.ToolStripMenuItem assembleMenuItem;
		private System.Windows.Forms.ToolStripMenuItem includeDirButton;
		private System.Windows.Forms.ToolStripMenuItem symTableMenuItem;
		private System.Windows.Forms.ToolStripMenuItem countCodeMenuItem;
		private System.Windows.Forms.ToolStripMenuItem projStatsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem listFileMenuItem;
		private System.Windows.Forms.ToolStripMenuItem hexFileMenuItem;
		private System.Windows.Forms.ToolStripMenuItem debugMenuItem;
		private System.Windows.Forms.ToolStripMenuItem startDebugMenuItem;
		private System.Windows.Forms.ToolStripMenuItem startWithoutDebugMenuItem;
		private System.Windows.Forms.ToolStripMenuItem stopDebugMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator19;
		private System.Windows.Forms.ToolStripMenuItem runMenuItem;
		private System.Windows.Forms.ToolStripMenuItem stepMenuItem;
		private System.Windows.Forms.ToolStripMenuItem stepOverMenuItem;
		private System.Windows.Forms.ToolStripMenuItem stepOutMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator20;
		private System.Windows.Forms.ToolStripMenuItem menuItem11;
		private System.Windows.Forms.ToolStripMenuItem newBreakpointMenuItem;
		private System.Windows.Forms.ToolStripMenuItem newDataBreakpointMenuItem;
		private System.Windows.Forms.ToolStripMenuItem toggleBreakpointMenuItem;
		private System.Windows.Forms.ToolStripMenuItem deleteAllBreakpointsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem disableAllBreakpointsMenuItem;
		private System.Windows.Forms.ToolStripMenuItem windowMenuItem;
		private System.Windows.Forms.ToolStripMenuItem helpMenuItem;
		private System.Windows.Forms.ToolStripMenuItem showHelpMenuItem;
		private System.Windows.Forms.ToolStripMenuItem updateMenuItem;
		private System.Windows.Forms.ToolStripMenuItem aboutMenuItem;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator21;
		private System.Windows.Forms.ToolStripContainer toolStripContainer;
		private System.Windows.Forms.ToolStrip editorToolStrip;
		private System.Windows.Forms.ToolStripButton toolStripButton1;
		private System.Windows.Forms.ToolStripButton toolStripButton2;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
		private System.Windows.Forms.ToolStripButton toolStripButton3;
		private System.Windows.Forms.ToolStripButton toolStripButton4;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
		private System.Windows.Forms.ToolStripButton toolStripButton5;
		private System.Windows.Forms.ToolStripButton toolStripButton6;
		private System.Windows.Forms.ToolStripButton toolStripButton7;
		private System.Windows.Forms.ToolStripButton toolStripButton8;
		private System.Windows.Forms.ToolStrip debugToolStrip;
		private System.Windows.Forms.ToolStripButton runDebuggerToolButton;
		private System.Windows.Forms.ToolStripButton pauseToolButton;
		private System.Windows.Forms.ToolStripButton stopToolButton;
		private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
		private System.Windows.Forms.ToolStripButton gotoCurrentToolButton;
		private System.Windows.Forms.ToolStripButton stepToolButton;
		private System.Windows.Forms.ToolStripButton stepOverToolButton;
		private System.Windows.Forms.ToolStripButton stepOutToolButton;
		private System.Windows.Forms.ToolStripButton restartToolStripButton;
	}
}