/*
 * Created by SharpDevelop.
 * User: LOCAL-USER
 * Date: 3/1/2009
 * Time: 12:22 AM
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace Revsoft.Wabbitcode
{
	partial class Preferences
	{
		/// <summary>
		/// Designer variable used to keep track of non-visual components.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		
		/// <summary>
		/// Disposes resources used by the form.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}
		
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent()
		{
            this.preferencesTabs = new System.Windows.Forms.TabControl();
            this.generalTab = new System.Windows.Forms.TabPage();
            this.defaultProjBox = new System.Windows.Forms.TextBox();
            this.browseProjButton = new System.Windows.Forms.Button();
            this.outwinBox = new System.Windows.Forms.GroupBox();
            this.colorizeOutWinBox = new System.Windows.Forms.CheckBox();
            this.outFontSizeLabel = new System.Windows.Forms.Label();
            this.outFontSize = new System.Windows.Forms.Label();
            this.outFontSizeBox = new System.Windows.Forms.ComboBox();
            this.outFontBox = new System.Windows.Forms.ComboBox();
            this.textboxBox = new System.Windows.Forms.GroupBox();
            this.enableRefHighlighterBox = new System.Windows.Forms.CheckBox();
            this.inverseScrollingBox = new System.Windows.Forms.CheckBox();
            this.enableAutoTriggerBox = new System.Windows.Forms.CheckBox();
            this.enableFoldingBox = new System.Windows.Forms.CheckBox();
            this.antiAliasBox = new System.Windows.Forms.CheckBox();
            this.autoIndentBox = new System.Windows.Forms.CheckBox();
            this.fontSizeLabel = new System.Windows.Forms.Label();
            this.fontLabel = new System.Windows.Forms.Label();
            this.fontSizeBox = new System.Windows.Forms.ComboBox();
            this.fontBox = new System.Windows.Forms.ComboBox();
            this.defaultProjLabel = new System.Windows.Forms.Label();
            this.highlightingTab = new System.Windows.Forms.TabPage();
            this.extraHighlightBox = new System.Windows.Forms.GroupBox();
            this.useForLabel = new System.Windows.Forms.Label();
            this.browseHighButton = new System.Windows.Forms.Button();
            this.extraExtBox = new System.Windows.Forms.TextBox();
            this.externalHighBox = new System.Windows.Forms.TextBox();
            this.highlightBox = new System.Windows.Forms.GroupBox();
            this.commentsEnabled = new System.Windows.Forms.CheckBox();
            this.lineEnabled = new System.Windows.Forms.CheckBox();
            this.opcodesEnabled = new System.Windows.Forms.CheckBox();
            this.conditionsEnabled = new System.Windows.Forms.CheckBox();
            this.registersEnabled = new System.Windows.Forms.CheckBox();
            this.preprocessorEnabled = new System.Windows.Forms.CheckBox();
            this.stringsEnabled = new System.Windows.Forms.CheckBox();
            this.directivesEnabled = new System.Windows.Forms.CheckBox();
            this.labelsEnabled = new System.Windows.Forms.CheckBox();
            this.opcodeColor = new System.Windows.Forms.Button();
            this.conditionColor = new System.Windows.Forms.Button();
            this.registerColor = new System.Windows.Forms.Button();
            this.stringColor = new System.Windows.Forms.Button();
            this.preProcessorColor = new System.Windows.Forms.Button();
            this.directiveColor = new System.Windows.Forms.Button();
            this.commentColor = new System.Windows.Forms.Button();
            this.labelColor = new System.Windows.Forms.Button();
            this.lineColor = new System.Windows.Forms.Button();
            this.highlightEnabled = new System.Windows.Forms.CheckBox();
            this.assemblyTab = new System.Windows.Forms.TabPage();
            this.moreOutputOptions = new System.Windows.Forms.GroupBox();
            this.noBinaryBox = new System.Windows.Forms.CheckBox();
            this.sendFileEmu = new System.Windows.Forms.CheckBox();
            this.caseSenseBox = new System.Windows.Forms.CheckBox();
            this.outTypeBox = new System.Windows.Forms.GroupBox();
            this.ti86sRadio = new System.Windows.Forms.RadioButton();
            this.binRadio = new System.Windows.Forms.RadioButton();
            this.ti86pRadio = new System.Windows.Forms.RadioButton();
            this.ti85pRadio = new System.Windows.Forms.RadioButton();
            this.ti83kRadio = new System.Windows.Forms.RadioButton();
            this.ti85sRadio = new System.Windows.Forms.RadioButton();
            this.ti83Radio = new System.Windows.Forms.RadioButton();
            this.ti83pRadio = new System.Windows.Forms.RadioButton();
            this.ti82Radio = new System.Windows.Forms.RadioButton();
            this.ti73Radio = new System.Windows.Forms.RadioButton();
            this.colorDialog = new System.Windows.Forms.ColorDialog();
            this.okButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.applyButton = new System.Windows.Forms.Button();
            this.preferencesTabs.SuspendLayout();
            this.generalTab.SuspendLayout();
            this.outwinBox.SuspendLayout();
            this.textboxBox.SuspendLayout();
            this.highlightingTab.SuspendLayout();
            this.extraHighlightBox.SuspendLayout();
            this.highlightBox.SuspendLayout();
            this.assemblyTab.SuspendLayout();
            this.moreOutputOptions.SuspendLayout();
            this.outTypeBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // preferencesTabs
            // 
            this.preferencesTabs.Controls.Add(this.generalTab);
            this.preferencesTabs.Controls.Add(this.highlightingTab);
            this.preferencesTabs.Controls.Add(this.assemblyTab);
            this.preferencesTabs.Dock = System.Windows.Forms.DockStyle.Top;
            this.preferencesTabs.Location = new System.Drawing.Point(0, 0);
            this.preferencesTabs.Name = "preferencesTabs";
            this.preferencesTabs.SelectedIndex = 0;
            this.preferencesTabs.Size = new System.Drawing.Size(329, 358);
            this.preferencesTabs.TabIndex = 0;
            // 
            // generalTab
            // 
            this.generalTab.Controls.Add(this.defaultProjBox);
            this.generalTab.Controls.Add(this.browseProjButton);
            this.generalTab.Controls.Add(this.outwinBox);
            this.generalTab.Controls.Add(this.textboxBox);
            this.generalTab.Controls.Add(this.defaultProjLabel);
            this.generalTab.Location = new System.Drawing.Point(4, 22);
            this.generalTab.Name = "generalTab";
            this.generalTab.Padding = new System.Windows.Forms.Padding(3);
            this.generalTab.Size = new System.Drawing.Size(321, 332);
            this.generalTab.TabIndex = 0;
            this.generalTab.Text = "General";
            this.generalTab.UseVisualStyleBackColor = true;
            // 
            // defaultProjBox
            // 
            this.defaultProjBox.Location = new System.Drawing.Point(8, 301);
            this.defaultProjBox.Name = "defaultProjBox";
            this.defaultProjBox.Size = new System.Drawing.Size(213, 20);
            this.defaultProjBox.TabIndex = 2;
            this.defaultProjBox.TextChanged += new System.EventHandler(this.defaultProjBox_TextChanged);
            this.defaultProjBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.defaultProjBox_KeyPress);
            // 
            // browseProjButton
            // 
            this.browseProjButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.browseProjButton.Location = new System.Drawing.Point(232, 299);
            this.browseProjButton.Name = "browseProjButton";
            this.browseProjButton.Size = new System.Drawing.Size(75, 23);
            this.browseProjButton.TabIndex = 1;
            this.browseProjButton.Text = "Browse";
            this.browseProjButton.UseVisualStyleBackColor = true;
            this.browseProjButton.Click += new System.EventHandler(this.browseProjButton_Click);
            // 
            // outwinBox
            // 
            this.outwinBox.Controls.Add(this.colorizeOutWinBox);
            this.outwinBox.Controls.Add(this.outFontSizeLabel);
            this.outwinBox.Controls.Add(this.outFontSize);
            this.outwinBox.Controls.Add(this.outFontSizeBox);
            this.outwinBox.Controls.Add(this.outFontBox);
            this.outwinBox.Location = new System.Drawing.Point(8, 171);
            this.outwinBox.Name = "outwinBox";
            this.outwinBox.Size = new System.Drawing.Size(305, 105);
            this.outwinBox.TabIndex = 1;
            this.outwinBox.TabStop = false;
            this.outwinBox.Text = "Output Window";
            // 
            // colorizeOutWinBox
            // 
            this.colorizeOutWinBox.Location = new System.Drawing.Point(16, 75);
            this.colorizeOutWinBox.Name = "colorizeOutWinBox";
            this.colorizeOutWinBox.Size = new System.Drawing.Size(86, 24);
            this.colorizeOutWinBox.TabIndex = 2;
            this.colorizeOutWinBox.Text = "Colorize";
            this.colorizeOutWinBox.UseVisualStyleBackColor = true;
            this.colorizeOutWinBox.CheckedChanged += new System.EventHandler(this.autoIndentBox_CheckedChanged);
            // 
            // outFontSizeLabel
            // 
            this.outFontSizeLabel.Location = new System.Drawing.Point(13, 49);
            this.outFontSizeLabel.Name = "outFontSizeLabel";
            this.outFontSizeLabel.Size = new System.Drawing.Size(56, 18);
            this.outFontSizeLabel.TabIndex = 1;
            this.outFontSizeLabel.Text = "Font Size:";
            // 
            // outFontSize
            // 
            this.outFontSize.Location = new System.Drawing.Point(33, 22);
            this.outFontSize.Name = "outFontSize";
            this.outFontSize.Size = new System.Drawing.Size(36, 18);
            this.outFontSize.TabIndex = 1;
            this.outFontSize.Text = "Font:";
            // 
            // outFontSizeBox
            // 
            this.outFontSizeBox.FormattingEnabled = true;
            this.outFontSizeBox.Location = new System.Drawing.Point(75, 46);
            this.outFontSizeBox.Name = "outFontSizeBox";
            this.outFontSizeBox.Size = new System.Drawing.Size(53, 21);
            this.outFontSizeBox.TabIndex = 0;
            this.outFontSizeBox.SelectedIndexChanged += new System.EventHandler(this.outFontSizeBox_SelectedIndexChanged);
            // 
            // outFontBox
            // 
            this.outFontBox.FormattingEnabled = true;
            this.outFontBox.Location = new System.Drawing.Point(75, 19);
            this.outFontBox.Name = "outFontBox";
            this.outFontBox.Size = new System.Drawing.Size(139, 21);
            this.outFontBox.TabIndex = 0;
            this.outFontBox.SelectedIndexChanged += new System.EventHandler(this.outFontBox_SelectedIndexChanged);
            // 
            // textboxBox
            // 
            this.textboxBox.Controls.Add(this.enableRefHighlighterBox);
            this.textboxBox.Controls.Add(this.inverseScrollingBox);
            this.textboxBox.Controls.Add(this.enableAutoTriggerBox);
            this.textboxBox.Controls.Add(this.enableFoldingBox);
            this.textboxBox.Controls.Add(this.antiAliasBox);
            this.textboxBox.Controls.Add(this.autoIndentBox);
            this.textboxBox.Controls.Add(this.fontSizeLabel);
            this.textboxBox.Controls.Add(this.fontLabel);
            this.textboxBox.Controls.Add(this.fontSizeBox);
            this.textboxBox.Controls.Add(this.fontBox);
            this.textboxBox.Location = new System.Drawing.Point(8, 6);
            this.textboxBox.Name = "textboxBox";
            this.textboxBox.Size = new System.Drawing.Size(305, 159);
            this.textboxBox.TabIndex = 0;
            this.textboxBox.TabStop = false;
            this.textboxBox.Text = "Text Editor";
            // 
            // enableRefHighlighterBox
            // 
            this.enableRefHighlighterBox.Location = new System.Drawing.Point(133, 130);
            this.enableRefHighlighterBox.Name = "enableRefHighlighterBox";
            this.enableRefHighlighterBox.Size = new System.Drawing.Size(166, 24);
            this.enableRefHighlighterBox.TabIndex = 3;
            this.enableRefHighlighterBox.Text = "Enable Reference Highlighter";
            this.enableRefHighlighterBox.UseVisualStyleBackColor = true;
            this.enableRefHighlighterBox.CheckedChanged += new System.EventHandler(this.enableRefHighlighterBox_CheckedChanged);
            // 
            // inverseScrollingBox
            // 
            this.inverseScrollingBox.Location = new System.Drawing.Point(133, 70);
            this.inverseScrollingBox.Name = "inverseScrollingBox";
            this.inverseScrollingBox.Size = new System.Drawing.Size(152, 24);
            this.inverseScrollingBox.TabIndex = 2;
            this.inverseScrollingBox.Text = "Inverse Scolling";
            this.inverseScrollingBox.UseVisualStyleBackColor = true;
            this.inverseScrollingBox.CheckedChanged += new System.EventHandler(this.mouseWheelZoomBox_CheckedChanged);
            // 
            // enableAutoTriggerBox
            // 
            this.enableAutoTriggerBox.Location = new System.Drawing.Point(6, 130);
            this.enableAutoTriggerBox.Name = "enableAutoTriggerBox";
            this.enableAutoTriggerBox.Size = new System.Drawing.Size(139, 24);
            this.enableAutoTriggerBox.TabIndex = 2;
            this.enableAutoTriggerBox.Text = "Enable Autotrigger";
            this.enableAutoTriggerBox.UseVisualStyleBackColor = true;
            this.enableAutoTriggerBox.CheckedChanged += new System.EventHandler(this.enableAutoCompleteBox_CheckedChanged);
            // 
            // enableFoldingBox
            // 
            this.enableFoldingBox.Location = new System.Drawing.Point(6, 100);
            this.enableFoldingBox.Name = "enableFoldingBox";
            this.enableFoldingBox.Size = new System.Drawing.Size(104, 24);
            this.enableFoldingBox.TabIndex = 2;
            this.enableFoldingBox.Text = "Enable Folding";
            this.enableFoldingBox.UseVisualStyleBackColor = true;
            this.enableFoldingBox.CheckedChanged += new System.EventHandler(this.enableFoldingBox_CheckedChanged);
            // 
            // antiAliasBox
            // 
            this.antiAliasBox.Location = new System.Drawing.Point(6, 70);
            this.antiAliasBox.Name = "antiAliasBox";
            this.antiAliasBox.Size = new System.Drawing.Size(104, 24);
            this.antiAliasBox.TabIndex = 2;
            this.antiAliasBox.Text = "Antialias Text";
            this.antiAliasBox.UseVisualStyleBackColor = true;
            this.antiAliasBox.CheckedChanged += new System.EventHandler(this.antiAliasBox_CheckedChanged);
            // 
            // autoIndentBox
            // 
            this.autoIndentBox.Location = new System.Drawing.Point(133, 100);
            this.autoIndentBox.Name = "autoIndentBox";
            this.autoIndentBox.Size = new System.Drawing.Size(104, 24);
            this.autoIndentBox.TabIndex = 2;
            this.autoIndentBox.Text = "Auto Indent";
            this.autoIndentBox.UseVisualStyleBackColor = true;
            this.autoIndentBox.CheckedChanged += new System.EventHandler(this.autoIndentBox_CheckedChanged);
            // 
            // fontSizeLabel
            // 
            this.fontSizeLabel.Location = new System.Drawing.Point(16, 49);
            this.fontSizeLabel.Name = "fontSizeLabel";
            this.fontSizeLabel.Size = new System.Drawing.Size(56, 18);
            this.fontSizeLabel.TabIndex = 1;
            this.fontSizeLabel.Text = "Font Size:";
            // 
            // fontLabel
            // 
            this.fontLabel.Location = new System.Drawing.Point(36, 22);
            this.fontLabel.Name = "fontLabel";
            this.fontLabel.Size = new System.Drawing.Size(36, 18);
            this.fontLabel.TabIndex = 1;
            this.fontLabel.Text = "Font:";
            // 
            // fontSizeBox
            // 
            this.fontSizeBox.FormattingEnabled = true;
            this.fontSizeBox.Location = new System.Drawing.Point(78, 46);
            this.fontSizeBox.Name = "fontSizeBox";
            this.fontSizeBox.Size = new System.Drawing.Size(53, 21);
            this.fontSizeBox.TabIndex = 0;
            this.fontSizeBox.SelectedIndexChanged += new System.EventHandler(this.fontSizeBox_SelectedIndexChanged);
            // 
            // fontBox
            // 
            this.fontBox.FormattingEnabled = true;
            this.fontBox.Location = new System.Drawing.Point(78, 19);
            this.fontBox.Name = "fontBox";
            this.fontBox.Size = new System.Drawing.Size(139, 21);
            this.fontBox.TabIndex = 0;
            this.fontBox.SelectedIndexChanged += new System.EventHandler(this.fontBox_SelectedIndexChanged);
            // 
            // defaultProjLabel
            // 
            this.defaultProjLabel.Location = new System.Drawing.Point(8, 279);
            this.defaultProjLabel.Name = "defaultProjLabel";
            this.defaultProjLabel.Size = new System.Drawing.Size(102, 19);
            this.defaultProjLabel.TabIndex = 1;
            this.defaultProjLabel.Text = "Default Project:";
            // 
            // highlightingTab
            // 
            this.highlightingTab.Controls.Add(this.extraHighlightBox);
            this.highlightingTab.Controls.Add(this.highlightBox);
            this.highlightingTab.Controls.Add(this.highlightEnabled);
            this.highlightingTab.Location = new System.Drawing.Point(4, 22);
            this.highlightingTab.Name = "highlightingTab";
            this.highlightingTab.Padding = new System.Windows.Forms.Padding(3);
            this.highlightingTab.Size = new System.Drawing.Size(321, 332);
            this.highlightingTab.TabIndex = 1;
            this.highlightingTab.Text = "Highlighting";
            this.highlightingTab.UseVisualStyleBackColor = true;
            // 
            // extraHighlightBox
            // 
            this.extraHighlightBox.Controls.Add(this.useForLabel);
            this.extraHighlightBox.Controls.Add(this.browseHighButton);
            this.extraHighlightBox.Controls.Add(this.extraExtBox);
            this.extraHighlightBox.Controls.Add(this.externalHighBox);
            this.extraHighlightBox.Location = new System.Drawing.Point(8, 238);
            this.extraHighlightBox.Name = "extraHighlightBox";
            this.extraHighlightBox.Size = new System.Drawing.Size(305, 88);
            this.extraHighlightBox.TabIndex = 2;
            this.extraHighlightBox.TabStop = false;
            this.extraHighlightBox.Text = "Extra Highlighting";
            // 
            // useForLabel
            // 
            this.useForLabel.AutoSize = true;
            this.useForLabel.Location = new System.Drawing.Point(10, 61);
            this.useForLabel.Name = "useForLabel";
            this.useForLabel.Size = new System.Drawing.Size(44, 13);
            this.useForLabel.TabIndex = 3;
            this.useForLabel.Text = "Use for:";
            // 
            // browseHighButton
            // 
            this.browseHighButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.browseHighButton.Location = new System.Drawing.Point(224, 27);
            this.browseHighButton.Name = "browseHighButton";
            this.browseHighButton.Size = new System.Drawing.Size(75, 23);
            this.browseHighButton.TabIndex = 2;
            this.browseHighButton.Text = "Browse";
            this.browseHighButton.UseVisualStyleBackColor = true;
            this.browseHighButton.Click += new System.EventHandler(this.browseHighButton_Click);
            // 
            // extraExtBox
            // 
            this.extraExtBox.Location = new System.Drawing.Point(60, 58);
            this.extraExtBox.Name = "extraExtBox";
            this.extraExtBox.Size = new System.Drawing.Size(212, 20);
            this.extraExtBox.TabIndex = 0;
            // 
            // externalHighBox
            // 
            this.externalHighBox.Location = new System.Drawing.Point(6, 29);
            this.externalHighBox.Name = "externalHighBox";
            this.externalHighBox.Size = new System.Drawing.Size(212, 20);
            this.externalHighBox.TabIndex = 0;
            // 
            // highlightBox
            // 
            this.highlightBox.Controls.Add(this.commentsEnabled);
            this.highlightBox.Controls.Add(this.lineEnabled);
            this.highlightBox.Controls.Add(this.opcodesEnabled);
            this.highlightBox.Controls.Add(this.conditionsEnabled);
            this.highlightBox.Controls.Add(this.registersEnabled);
            this.highlightBox.Controls.Add(this.preprocessorEnabled);
            this.highlightBox.Controls.Add(this.stringsEnabled);
            this.highlightBox.Controls.Add(this.directivesEnabled);
            this.highlightBox.Controls.Add(this.labelsEnabled);
            this.highlightBox.Controls.Add(this.opcodeColor);
            this.highlightBox.Controls.Add(this.conditionColor);
            this.highlightBox.Controls.Add(this.registerColor);
            this.highlightBox.Controls.Add(this.stringColor);
            this.highlightBox.Controls.Add(this.preProcessorColor);
            this.highlightBox.Controls.Add(this.directiveColor);
            this.highlightBox.Controls.Add(this.commentColor);
            this.highlightBox.Controls.Add(this.labelColor);
            this.highlightBox.Controls.Add(this.lineColor);
            this.highlightBox.Location = new System.Drawing.Point(3, 49);
            this.highlightBox.Name = "highlightBox";
            this.highlightBox.Size = new System.Drawing.Size(310, 183);
            this.highlightBox.TabIndex = 1;
            this.highlightBox.TabStop = false;
            this.highlightBox.Text = "Options";
            // 
            // commentsEnabled
            // 
            this.commentsEnabled.Location = new System.Drawing.Point(5, 51);
            this.commentsEnabled.Name = "commentsEnabled";
            this.commentsEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.commentsEnabled.Size = new System.Drawing.Size(94, 24);
            this.commentsEnabled.TabIndex = 3;
            this.commentsEnabled.Text = "Comments";
            this.commentsEnabled.UseVisualStyleBackColor = true;
            this.commentsEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // lineEnabled
            // 
            this.lineEnabled.Location = new System.Drawing.Point(136, 119);
            this.lineEnabled.Name = "lineEnabled";
            this.lineEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.lineEnabled.Size = new System.Drawing.Size(94, 24);
            this.lineEnabled.TabIndex = 3;
            this.lineEnabled.Text = "Line";
            this.lineEnabled.UseVisualStyleBackColor = true;
            this.lineEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // opcodesEnabled
            // 
            this.opcodesEnabled.Location = new System.Drawing.Point(136, 83);
            this.opcodesEnabled.Name = "opcodesEnabled";
            this.opcodesEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.opcodesEnabled.Size = new System.Drawing.Size(94, 24);
            this.opcodesEnabled.TabIndex = 3;
            this.opcodesEnabled.Text = "Opcodes";
            this.opcodesEnabled.UseVisualStyleBackColor = true;
            this.opcodesEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // conditionsEnabled
            // 
            this.conditionsEnabled.Location = new System.Drawing.Point(136, 52);
            this.conditionsEnabled.Name = "conditionsEnabled";
            this.conditionsEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.conditionsEnabled.Size = new System.Drawing.Size(94, 24);
            this.conditionsEnabled.TabIndex = 3;
            this.conditionsEnabled.Text = "Conditions";
            this.conditionsEnabled.UseVisualStyleBackColor = true;
            this.conditionsEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // registersEnabled
            // 
            this.registersEnabled.Location = new System.Drawing.Point(136, 19);
            this.registersEnabled.Name = "registersEnabled";
            this.registersEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.registersEnabled.Size = new System.Drawing.Size(94, 24);
            this.registersEnabled.TabIndex = 3;
            this.registersEnabled.Text = "Registers";
            this.registersEnabled.UseVisualStyleBackColor = true;
            this.registersEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // preprocessorEnabled
            // 
            this.preprocessorEnabled.Location = new System.Drawing.Point(5, 116);
            this.preprocessorEnabled.Name = "preprocessorEnabled";
            this.preprocessorEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.preprocessorEnabled.Size = new System.Drawing.Size(94, 24);
            this.preprocessorEnabled.TabIndex = 3;
            this.preprocessorEnabled.Text = "Preprocessor";
            this.preprocessorEnabled.UseVisualStyleBackColor = true;
            this.preprocessorEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // stringsEnabled
            // 
            this.stringsEnabled.Location = new System.Drawing.Point(18, 148);
            this.stringsEnabled.Name = "stringsEnabled";
            this.stringsEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.stringsEnabled.Size = new System.Drawing.Size(81, 24);
            this.stringsEnabled.TabIndex = 3;
            this.stringsEnabled.Text = "Strings";
            this.stringsEnabled.UseVisualStyleBackColor = true;
            this.stringsEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // directivesEnabled
            // 
            this.directivesEnabled.Location = new System.Drawing.Point(18, 83);
            this.directivesEnabled.Name = "directivesEnabled";
            this.directivesEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.directivesEnabled.Size = new System.Drawing.Size(81, 24);
            this.directivesEnabled.TabIndex = 3;
            this.directivesEnabled.Text = "Directives";
            this.directivesEnabled.UseVisualStyleBackColor = true;
            this.directivesEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // labelsEnabled
            // 
            this.labelsEnabled.Location = new System.Drawing.Point(18, 19);
            this.labelsEnabled.Name = "labelsEnabled";
            this.labelsEnabled.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.labelsEnabled.Size = new System.Drawing.Size(81, 24);
            this.labelsEnabled.TabIndex = 3;
            this.labelsEnabled.Text = "Labels";
            this.labelsEnabled.UseVisualStyleBackColor = true;
            this.labelsEnabled.CheckedChanged += new System.EventHandler(this.highlightCheckChange);
            // 
            // opcodeColor
            // 
            this.opcodeColor.BackColor = System.Drawing.Color.Gray;
            this.opcodeColor.FlatAppearance.BorderSize = 0;
            this.opcodeColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.opcodeColor.Location = new System.Drawing.Point(236, 85);
            this.opcodeColor.Name = "opcodeColor";
            this.opcodeColor.Size = new System.Drawing.Size(20, 20);
            this.opcodeColor.TabIndex = 0;
            this.opcodeColor.UseVisualStyleBackColor = false;
            this.opcodeColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // conditionColor
            // 
            this.conditionColor.BackColor = System.Drawing.Color.Gray;
            this.conditionColor.FlatAppearance.BorderSize = 0;
            this.conditionColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.conditionColor.Location = new System.Drawing.Point(236, 53);
            this.conditionColor.Name = "conditionColor";
            this.conditionColor.Size = new System.Drawing.Size(20, 20);
            this.conditionColor.TabIndex = 0;
            this.conditionColor.UseVisualStyleBackColor = false;
            this.conditionColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // registerColor
            // 
            this.registerColor.BackColor = System.Drawing.Color.Gray;
            this.registerColor.FlatAppearance.BorderSize = 0;
            this.registerColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.registerColor.Location = new System.Drawing.Point(236, 20);
            this.registerColor.Name = "registerColor";
            this.registerColor.Size = new System.Drawing.Size(20, 20);
            this.registerColor.TabIndex = 0;
            this.registerColor.UseVisualStyleBackColor = false;
            this.registerColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // stringColor
            // 
            this.stringColor.BackColor = System.Drawing.Color.Gray;
            this.stringColor.FlatAppearance.BorderSize = 0;
            this.stringColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.stringColor.Location = new System.Drawing.Point(105, 150);
            this.stringColor.Name = "stringColor";
            this.stringColor.Size = new System.Drawing.Size(20, 20);
            this.stringColor.TabIndex = 0;
            this.stringColor.UseVisualStyleBackColor = false;
            this.stringColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // preProcessorColor
            // 
            this.preProcessorColor.BackColor = System.Drawing.Color.Gray;
            this.preProcessorColor.FlatAppearance.BorderSize = 0;
            this.preProcessorColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.preProcessorColor.Location = new System.Drawing.Point(105, 117);
            this.preProcessorColor.Name = "preProcessorColor";
            this.preProcessorColor.Size = new System.Drawing.Size(20, 20);
            this.preProcessorColor.TabIndex = 0;
            this.preProcessorColor.UseVisualStyleBackColor = false;
            this.preProcessorColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // directiveColor
            // 
            this.directiveColor.BackColor = System.Drawing.Color.Gray;
            this.directiveColor.FlatAppearance.BorderSize = 0;
            this.directiveColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.directiveColor.Location = new System.Drawing.Point(105, 83);
            this.directiveColor.Name = "directiveColor";
            this.directiveColor.Size = new System.Drawing.Size(20, 20);
            this.directiveColor.TabIndex = 0;
            this.directiveColor.UseVisualStyleBackColor = false;
            this.directiveColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // commentColor
            // 
            this.commentColor.BackColor = System.Drawing.Color.Gray;
            this.commentColor.FlatAppearance.BorderSize = 0;
            this.commentColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.commentColor.Location = new System.Drawing.Point(105, 52);
            this.commentColor.Name = "commentColor";
            this.commentColor.Size = new System.Drawing.Size(20, 20);
            this.commentColor.TabIndex = 0;
            this.commentColor.UseVisualStyleBackColor = false;
            this.commentColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // labelColor
            // 
            this.labelColor.BackColor = System.Drawing.Color.Gray;
            this.labelColor.FlatAppearance.BorderSize = 0;
            this.labelColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.labelColor.Location = new System.Drawing.Point(105, 19);
            this.labelColor.Name = "labelColor";
            this.labelColor.Size = new System.Drawing.Size(20, 20);
            this.labelColor.TabIndex = 0;
            this.labelColor.UseVisualStyleBackColor = false;
            this.labelColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // lineColor
            // 
            this.lineColor.BackColor = System.Drawing.Color.Gray;
            this.lineColor.FlatAppearance.BorderSize = 0;
            this.lineColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.lineColor.Location = new System.Drawing.Point(236, 120);
            this.lineColor.Name = "lineColor";
            this.lineColor.Size = new System.Drawing.Size(20, 20);
            this.lineColor.TabIndex = 0;
            this.lineColor.UseVisualStyleBackColor = false;
            this.lineColor.Click += new System.EventHandler(this.labelColor_Click);
            // 
            // highlightEnabled
            // 
            this.highlightEnabled.Location = new System.Drawing.Point(79, 19);
            this.highlightEnabled.Name = "highlightEnabled";
            this.highlightEnabled.Size = new System.Drawing.Size(127, 24);
            this.highlightEnabled.TabIndex = 0;
            this.highlightEnabled.Text = "Highlighting Enabled";
            this.highlightEnabled.UseVisualStyleBackColor = true;
            this.highlightEnabled.CheckedChanged += new System.EventHandler(this.highlightEnabled_CheckedChanged);
            // 
            // assemblyTab
            // 
            this.assemblyTab.Controls.Add(this.moreOutputOptions);
            this.assemblyTab.Controls.Add(this.outTypeBox);
            this.assemblyTab.Location = new System.Drawing.Point(4, 22);
            this.assemblyTab.Name = "assemblyTab";
            this.assemblyTab.Padding = new System.Windows.Forms.Padding(3);
            this.assemblyTab.Size = new System.Drawing.Size(321, 332);
            this.assemblyTab.TabIndex = 2;
            this.assemblyTab.Text = "Assembly";
            this.assemblyTab.UseVisualStyleBackColor = true;
            // 
            // moreOutputOptions
            // 
            this.moreOutputOptions.Controls.Add(this.noBinaryBox);
            this.moreOutputOptions.Controls.Add(this.sendFileEmu);
            this.moreOutputOptions.Controls.Add(this.caseSenseBox);
            this.moreOutputOptions.Location = new System.Drawing.Point(8, 153);
            this.moreOutputOptions.Name = "moreOutputOptions";
            this.moreOutputOptions.Size = new System.Drawing.Size(305, 96);
            this.moreOutputOptions.TabIndex = 1;
            this.moreOutputOptions.TabStop = false;
            this.moreOutputOptions.Text = "Options";
            // 
            // noBinaryBox
            // 
            this.noBinaryBox.AutoSize = true;
            this.noBinaryBox.Location = new System.Drawing.Point(154, 28);
            this.noBinaryBox.Name = "noBinaryBox";
            this.noBinaryBox.Size = new System.Drawing.Size(107, 17);
            this.noBinaryBox.TabIndex = 0;
            this.noBinaryBox.Text = "Do not create file";
            this.noBinaryBox.UseVisualStyleBackColor = true;
            // 
            // sendFileEmu
            // 
            this.sendFileEmu.AutoSize = true;
            this.sendFileEmu.Location = new System.Drawing.Point(25, 62);
            this.sendFileEmu.Name = "sendFileEmu";
            this.sendFileEmu.Size = new System.Drawing.Size(130, 17);
            this.sendFileEmu.TabIndex = 0;
            this.sendFileEmu.Text = "Send File To Emulator";
            this.sendFileEmu.UseVisualStyleBackColor = true;
            this.sendFileEmu.CheckedChanged += new System.EventHandler(this.sendFileEmu_CheckedChanged);
            // 
            // caseSenseBox
            // 
            this.caseSenseBox.AutoSize = true;
            this.caseSenseBox.Location = new System.Drawing.Point(25, 28);
            this.caseSenseBox.Name = "caseSenseBox";
            this.caseSenseBox.Size = new System.Drawing.Size(96, 17);
            this.caseSenseBox.TabIndex = 0;
            this.caseSenseBox.Text = "Case Sensitive";
            this.caseSenseBox.UseVisualStyleBackColor = true;
            this.caseSenseBox.CheckedChanged += new System.EventHandler(this.caseSenseBox_CheckedChanged);
            // 
            // outTypeBox
            // 
            this.outTypeBox.Controls.Add(this.ti86sRadio);
            this.outTypeBox.Controls.Add(this.binRadio);
            this.outTypeBox.Controls.Add(this.ti86pRadio);
            this.outTypeBox.Controls.Add(this.ti85pRadio);
            this.outTypeBox.Controls.Add(this.ti83kRadio);
            this.outTypeBox.Controls.Add(this.ti85sRadio);
            this.outTypeBox.Controls.Add(this.ti83Radio);
            this.outTypeBox.Controls.Add(this.ti83pRadio);
            this.outTypeBox.Controls.Add(this.ti82Radio);
            this.outTypeBox.Controls.Add(this.ti73Radio);
            this.outTypeBox.Location = new System.Drawing.Point(8, 6);
            this.outTypeBox.Name = "outTypeBox";
            this.outTypeBox.Size = new System.Drawing.Size(305, 141);
            this.outTypeBox.TabIndex = 0;
            this.outTypeBox.TabStop = false;
            this.outTypeBox.Text = "Output";
            // 
            // ti86sRadio
            // 
            this.ti86sRadio.AutoSize = true;
            this.ti86sRadio.Location = new System.Drawing.Point(203, 115);
            this.ti86sRadio.Name = "ti86sRadio";
            this.ti86sRadio.Size = new System.Drawing.Size(80, 17);
            this.ti86sRadio.TabIndex = 9;
            this.ti86sRadio.TabStop = true;
            this.ti86sRadio.Text = "TI-86 String";
            this.ti86sRadio.UseVisualStyleBackColor = true;
            this.ti86sRadio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // binRadio
            // 
            this.binRadio.AutoSize = true;
            this.binRadio.Location = new System.Drawing.Point(13, 18);
            this.binRadio.Name = "binRadio";
            this.binRadio.Size = new System.Drawing.Size(73, 17);
            this.binRadio.TabIndex = 0;
            this.binRadio.TabStop = true;
            this.binRadio.Text = "Binary File";
            this.binRadio.UseVisualStyleBackColor = true;
            this.binRadio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti86pRadio
            // 
            this.ti86pRadio.AutoSize = true;
            this.ti86pRadio.Location = new System.Drawing.Point(109, 115);
            this.ti86pRadio.Name = "ti86pRadio";
            this.ti86pRadio.Size = new System.Drawing.Size(92, 17);
            this.ti86pRadio.TabIndex = 8;
            this.ti86pRadio.TabStop = true;
            this.ti86pRadio.Text = "TI-86 Program";
            this.ti86pRadio.UseVisualStyleBackColor = true;
            this.ti86pRadio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti85pRadio
            // 
            this.ti85pRadio.AutoSize = true;
            this.ti85pRadio.Location = new System.Drawing.Point(203, 82);
            this.ti85pRadio.Name = "ti85pRadio";
            this.ti85pRadio.Size = new System.Drawing.Size(92, 17);
            this.ti85pRadio.TabIndex = 6;
            this.ti85pRadio.TabStop = true;
            this.ti85pRadio.Text = "TI-85 Program";
            this.ti85pRadio.UseVisualStyleBackColor = true;
            this.ti85pRadio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti83kRadio
            // 
            this.ti83kRadio.AutoSize = true;
            this.ti83kRadio.Location = new System.Drawing.Point(109, 82);
            this.ti83kRadio.Name = "ti83kRadio";
            this.ti83kRadio.Size = new System.Drawing.Size(95, 17);
            this.ti83kRadio.TabIndex = 5;
            this.ti83kRadio.TabStop = true;
            this.ti83kRadio.Text = "TI-83 Plus App";
            this.ti83kRadio.UseVisualStyleBackColor = true;
            this.ti83kRadio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti85sRadio
            // 
            this.ti85sRadio.AutoSize = true;
            this.ti85sRadio.Location = new System.Drawing.Point(13, 115);
            this.ti85sRadio.Name = "ti85sRadio";
            this.ti85sRadio.Size = new System.Drawing.Size(80, 17);
            this.ti85sRadio.TabIndex = 7;
            this.ti85sRadio.TabStop = true;
            this.ti85sRadio.Text = "TI-85 String";
            this.ti85sRadio.UseVisualStyleBackColor = true;
            this.ti85sRadio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti83Radio
            // 
            this.ti83Radio.AutoSize = true;
            this.ti83Radio.Location = new System.Drawing.Point(203, 49);
            this.ti83Radio.Name = "ti83Radio";
            this.ti83Radio.Size = new System.Drawing.Size(92, 17);
            this.ti83Radio.TabIndex = 3;
            this.ti83Radio.TabStop = true;
            this.ti83Radio.Text = "TI-83 Program";
            this.ti83Radio.UseVisualStyleBackColor = true;
            this.ti83Radio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti83pRadio
            // 
            this.ti83pRadio.AutoSize = true;
            this.ti83pRadio.Location = new System.Drawing.Point(13, 82);
            this.ti83pRadio.Name = "ti83pRadio";
            this.ti83pRadio.Size = new System.Drawing.Size(98, 17);
            this.ti83pRadio.TabIndex = 4;
            this.ti83pRadio.TabStop = true;
            this.ti83pRadio.Text = "TI-83 Plus Prog";
            this.ti83pRadio.UseVisualStyleBackColor = true;
            this.ti83pRadio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti82Radio
            // 
            this.ti82Radio.AutoSize = true;
            this.ti82Radio.Location = new System.Drawing.Point(109, 49);
            this.ti82Radio.Name = "ti82Radio";
            this.ti82Radio.Size = new System.Drawing.Size(92, 17);
            this.ti82Radio.TabIndex = 2;
            this.ti82Radio.TabStop = true;
            this.ti82Radio.Text = "TI-82 Program";
            this.ti82Radio.UseVisualStyleBackColor = true;
            this.ti82Radio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // ti73Radio
            // 
            this.ti73Radio.AutoSize = true;
            this.ti73Radio.Location = new System.Drawing.Point(13, 49);
            this.ti73Radio.Name = "ti73Radio";
            this.ti73Radio.Size = new System.Drawing.Size(92, 17);
            this.ti73Radio.TabIndex = 1;
            this.ti73Radio.TabStop = true;
            this.ti73Radio.Tag = "";
            this.ti73Radio.Text = "TI-73 Program";
            this.ti73Radio.UseVisualStyleBackColor = true;
            this.ti73Radio.CheckedChanged += new System.EventHandler(this.ti83Radio_CheckedChanged);
            // 
            // colorDialog
            // 
            this.colorDialog.AnyColor = true;
            this.colorDialog.FullOpen = true;
            // 
            // okButton
            // 
            this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.okButton.Location = new System.Drawing.Point(98, 364);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(69, 23);
            this.okButton.TabIndex = 1;
            this.okButton.Text = "OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cancelButton.Location = new System.Drawing.Point(179, 364);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(69, 23);
            this.cancelButton.TabIndex = 1;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // applyButton
            // 
            this.applyButton.Location = new System.Drawing.Point(255, 364);
            this.applyButton.Name = "applyButton";
            this.applyButton.Size = new System.Drawing.Size(69, 23);
            this.applyButton.TabIndex = 1;
            this.applyButton.Text = "Apply";
            this.applyButton.UseVisualStyleBackColor = true;
            this.applyButton.Click += new System.EventHandler(this.applyButton_Click);
            // 
            // Preferences
            // 
            this.AcceptButton = this.okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.cancelButton;
            this.ClientSize = new System.Drawing.Size(329, 395);
            this.Controls.Add(this.applyButton);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.preferencesTabs);
            this.HelpButton = true;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Preferences";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Preferences";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Preferences_FormClosing);
            this.preferencesTabs.ResumeLayout(false);
            this.generalTab.ResumeLayout(false);
            this.generalTab.PerformLayout();
            this.outwinBox.ResumeLayout(false);
            this.textboxBox.ResumeLayout(false);
            this.highlightingTab.ResumeLayout(false);
            this.extraHighlightBox.ResumeLayout(false);
            this.extraHighlightBox.PerformLayout();
            this.highlightBox.ResumeLayout(false);
            this.assemblyTab.ResumeLayout(false);
            this.moreOutputOptions.ResumeLayout(false);
            this.moreOutputOptions.PerformLayout();
            this.outTypeBox.ResumeLayout(false);
            this.outTypeBox.PerformLayout();
            this.ResumeLayout(false);

		}
		private System.Windows.Forms.CheckBox lineEnabled;
		private System.Windows.Forms.CheckBox labelsEnabled;
		private System.Windows.Forms.CheckBox directivesEnabled;
		private System.Windows.Forms.CheckBox preprocessorEnabled;
		private System.Windows.Forms.CheckBox registersEnabled;
		private System.Windows.Forms.CheckBox conditionsEnabled;
		private System.Windows.Forms.CheckBox opcodesEnabled;
		private System.Windows.Forms.CheckBox commentsEnabled;
		private System.Windows.Forms.Button labelColor;
		private System.Windows.Forms.Button commentColor;
		private System.Windows.Forms.Button directiveColor;
		private System.Windows.Forms.Button preProcessorColor;
		private System.Windows.Forms.Button registerColor;
		private System.Windows.Forms.Button conditionColor;
		private System.Windows.Forms.Button opcodeColor;
		private System.Windows.Forms.Button lineColor;
		private System.Windows.Forms.CheckBox highlightEnabled;
		private System.Windows.Forms.GroupBox highlightBox;
		private System.Windows.Forms.CheckBox enableAutoTriggerBox;
		private System.Windows.Forms.CheckBox enableFoldingBox;
		private System.Windows.Forms.CheckBox inverseScrollingBox;
		private System.Windows.Forms.CheckBox antiAliasBox;
		private System.Windows.Forms.ComboBox outFontBox;
		private System.Windows.Forms.ComboBox fontBox;
		private System.Windows.Forms.ComboBox fontSizeBox;
		private System.Windows.Forms.ComboBox outFontSizeBox;
		private System.Windows.Forms.Label fontLabel;
		private System.Windows.Forms.Label fontSizeLabel;
		private System.Windows.Forms.Label outFontSizeLabel;
		private System.Windows.Forms.Label outFontSize;
		private System.Windows.Forms.CheckBox colorizeOutWinBox;
		private System.Windows.Forms.GroupBox outwinBox;
		private System.Windows.Forms.CheckBox autoIndentBox;
		private System.Windows.Forms.GroupBox textboxBox;
		private System.Windows.Forms.TabPage assemblyTab;
		private System.Windows.Forms.TabPage highlightingTab;
		private System.Windows.Forms.TabPage generalTab;
		private System.Windows.Forms.TabControl preferencesTabs;
        private System.Windows.Forms.ColorDialog colorDialog;
        private System.Windows.Forms.GroupBox outTypeBox;
        private System.Windows.Forms.RadioButton ti86sRadio;
        private System.Windows.Forms.RadioButton ti86pRadio;
        private System.Windows.Forms.RadioButton ti85pRadio;
        private System.Windows.Forms.RadioButton ti83kRadio;
        private System.Windows.Forms.RadioButton ti85sRadio;
        private System.Windows.Forms.RadioButton ti83Radio;
        private System.Windows.Forms.RadioButton ti83pRadio;
        private System.Windows.Forms.RadioButton ti82Radio;
        private System.Windows.Forms.RadioButton ti73Radio;
        private System.Windows.Forms.RadioButton binRadio;
        private System.Windows.Forms.GroupBox moreOutputOptions;
        private System.Windows.Forms.CheckBox noBinaryBox;
        private System.Windows.Forms.CheckBox caseSenseBox;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.CheckBox sendFileEmu;
        private System.Windows.Forms.TextBox defaultProjBox;
        private System.Windows.Forms.Button browseProjButton;
        private System.Windows.Forms.Label defaultProjLabel;
        private System.Windows.Forms.GroupBox extraHighlightBox;
        private System.Windows.Forms.Button browseHighButton;
        private System.Windows.Forms.TextBox externalHighBox;
        private System.Windows.Forms.Label useForLabel;
        private System.Windows.Forms.TextBox extraExtBox;
		private System.Windows.Forms.CheckBox stringsEnabled;
		private System.Windows.Forms.Button stringColor;
		private System.Windows.Forms.Button applyButton;
        private System.Windows.Forms.CheckBox enableRefHighlighterBox;
	}
}
