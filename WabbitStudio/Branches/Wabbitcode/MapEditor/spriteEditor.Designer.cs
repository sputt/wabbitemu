namespace Revsoft.Wabbitcode
{
    partial class spriteEditor
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(spriteEditor));
            this.heightInput = new System.Windows.Forms.NumericUpDown();
            this.widthInput = new System.Windows.Forms.NumericUpDown();
            this.heightLabel = new System.Windows.Forms.Label();
            this.widthLabel = new System.Windows.Forms.Label();
            this.grayscaleCheck = new System.Windows.Forms.CheckBox();
            this.colorBlack = new System.Windows.Forms.Label();
            this.colorDarkGray = new System.Windows.Forms.Label();
            this.colorLightGray = new System.Windows.Forms.Label();
            this.colorWhite = new System.Windows.Forms.Label();
            this.clearButton = new System.Windows.Forms.Button();
            this.okButton = new System.Windows.Forms.Button();
            this.pixelBox = new System.Windows.Forms.PictureBox();
            this.sizeBox = new System.Windows.Forms.GroupBox();
            this.colorBox = new System.Windows.Forms.GroupBox();
            this.spriteView = new System.Windows.Forms.ListView();
            ((System.ComponentModel.ISupportInitialize)(this.heightInput)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.widthInput)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pixelBox)).BeginInit();
            this.sizeBox.SuspendLayout();
            this.colorBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // heightInput
            // 
            this.heightInput.Location = new System.Drawing.Point(51, 44);
            this.heightInput.Maximum = new decimal(new int[] {
            64,
            0,
            0,
            0});
            this.heightInput.Minimum = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.heightInput.Name = "heightInput";
            this.heightInput.Size = new System.Drawing.Size(82, 20);
            this.heightInput.TabIndex = 1;
            this.heightInput.Value = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.heightInput.ValueChanged += new System.EventHandler(this.heightInput_ValueChanged);
            // 
            // widthInput
            // 
            this.widthInput.Location = new System.Drawing.Point(51, 19);
            this.widthInput.Maximum = new decimal(new int[] {
            96,
            0,
            0,
            0});
            this.widthInput.Minimum = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.widthInput.Name = "widthInput";
            this.widthInput.Size = new System.Drawing.Size(82, 20);
            this.widthInput.TabIndex = 0;
            this.widthInput.Value = new decimal(new int[] {
            8,
            0,
            0,
            0});
            this.widthInput.ValueChanged += new System.EventHandler(this.widthInput_ValueChanged);
            // 
            // heightLabel
            // 
            this.heightLabel.AutoSize = true;
            this.heightLabel.BackColor = System.Drawing.Color.Transparent;
            this.heightLabel.Location = new System.Drawing.Point(7, 46);
            this.heightLabel.Name = "heightLabel";
            this.heightLabel.Size = new System.Drawing.Size(41, 13);
            this.heightLabel.TabIndex = 1;
            this.heightLabel.Text = "Height:";
            // 
            // widthLabel
            // 
            this.widthLabel.AutoSize = true;
            this.widthLabel.BackColor = System.Drawing.Color.Transparent;
            this.widthLabel.Location = new System.Drawing.Point(7, 19);
            this.widthLabel.Name = "widthLabel";
            this.widthLabel.Size = new System.Drawing.Size(38, 13);
            this.widthLabel.TabIndex = 1;
            this.widthLabel.Text = "Width:";
            // 
            // grayscaleCheck
            // 
            this.grayscaleCheck.BackColor = System.Drawing.Color.Transparent;
            this.grayscaleCheck.Checked = true;
            this.grayscaleCheck.CheckState = System.Windows.Forms.CheckState.Checked;
            this.grayscaleCheck.Location = new System.Drawing.Point(7, 45);
            this.grayscaleCheck.Name = "grayscaleCheck";
            this.grayscaleCheck.Size = new System.Drawing.Size(118, 23);
            this.grayscaleCheck.TabIndex = 6;
            this.grayscaleCheck.Text = "Grayscale";
            this.grayscaleCheck.UseVisualStyleBackColor = false;
            this.grayscaleCheck.CheckedChanged += new System.EventHandler(this.grayscaleCheck_CheckedChanged);
            // 
            // colorBlack
            // 
            this.colorBlack.BackColor = System.Drawing.Color.Black;
            this.colorBlack.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.colorBlack.ForeColor = System.Drawing.SystemColors.ControlText;
            this.colorBlack.Location = new System.Drawing.Point(100, 17);
            this.colorBlack.Name = "colorBlack";
            this.colorBlack.Size = new System.Drawing.Size(25, 25);
            this.colorBlack.TabIndex = 5;
            this.colorBlack.Click += new System.EventHandler(this.colorBlack_Click);
            // 
            // colorDarkGray
            // 
            this.colorDarkGray.BackColor = System.Drawing.Color.DimGray;
            this.colorDarkGray.Location = new System.Drawing.Point(69, 17);
            this.colorDarkGray.Name = "colorDarkGray";
            this.colorDarkGray.Size = new System.Drawing.Size(25, 25);
            this.colorDarkGray.TabIndex = 4;
            this.colorDarkGray.Click += new System.EventHandler(this.colorDarkGray_Click);
            // 
            // colorLightGray
            // 
            this.colorLightGray.BackColor = System.Drawing.Color.LightGray;
            this.colorLightGray.Location = new System.Drawing.Point(38, 17);
            this.colorLightGray.Name = "colorLightGray";
            this.colorLightGray.Size = new System.Drawing.Size(25, 25);
            this.colorLightGray.TabIndex = 3;
            this.colorLightGray.Click += new System.EventHandler(this.colorLightGray_Click);
            // 
            // colorWhite
            // 
            this.colorWhite.BackColor = System.Drawing.Color.White;
            this.colorWhite.Location = new System.Drawing.Point(7, 17);
            this.colorWhite.Name = "colorWhite";
            this.colorWhite.Size = new System.Drawing.Size(25, 25);
            this.colorWhite.TabIndex = 2;
            this.colorWhite.Click += new System.EventHandler(this.colorWhite_Click);
            // 
            // clearButton
            // 
            this.clearButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.clearButton.Image = ((System.Drawing.Image)(resources.GetObject("clearButton.Image")));
            this.clearButton.Location = new System.Drawing.Point(156, 103);
            this.clearButton.Name = "clearButton";
            this.clearButton.Size = new System.Drawing.Size(28, 28);
            this.clearButton.TabIndex = 3;
            this.clearButton.Click += new System.EventHandler(this.clearButton_Click);
            // 
            // okButton
            // 
            this.okButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
            this.okButton.Location = new System.Drawing.Point(156, 203);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(28, 23);
            this.okButton.TabIndex = 3;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // pixelBox
            // 
            this.pixelBox.BackColor = System.Drawing.Color.White;
            this.pixelBox.Location = new System.Drawing.Point(190, 98);
            this.pixelBox.Name = "pixelBox";
            this.pixelBox.Size = new System.Drawing.Size(138, 138);
            this.pixelBox.TabIndex = 0;
            this.pixelBox.TabStop = false;
            this.pixelBox.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pixelBox_MouseMove);
            this.pixelBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pixelBox_MouseDown);
            this.pixelBox.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pixelBox_MouseUp);
            // 
            // sizeBox
            // 
            this.sizeBox.Controls.Add(this.heightInput);
            this.sizeBox.Controls.Add(this.widthInput);
            this.sizeBox.Controls.Add(this.widthLabel);
            this.sizeBox.Controls.Add(this.heightLabel);
            this.sizeBox.Location = new System.Drawing.Point(12, 82);
            this.sizeBox.Name = "sizeBox";
            this.sizeBox.Size = new System.Drawing.Size(138, 78);
            this.sizeBox.TabIndex = 4;
            this.sizeBox.TabStop = false;
            this.sizeBox.Text = "Size";
            // 
            // colorBox
            // 
            this.colorBox.Controls.Add(this.grayscaleCheck);
            this.colorBox.Controls.Add(this.colorDarkGray);
            this.colorBox.Controls.Add(this.colorBlack);
            this.colorBox.Controls.Add(this.colorWhite);
            this.colorBox.Controls.Add(this.colorLightGray);
            this.colorBox.Location = new System.Drawing.Point(12, 166);
            this.colorBox.Name = "colorBox";
            this.colorBox.Size = new System.Drawing.Size(138, 76);
            this.colorBox.TabIndex = 5;
            this.colorBox.TabStop = false;
            this.colorBox.Text = "Color";
            // 
            // spriteView
            // 
            this.spriteView.Location = new System.Drawing.Point(12, 12);
            this.spriteView.Name = "spriteView";
            this.spriteView.Size = new System.Drawing.Size(316, 64);
            this.spriteView.TabIndex = 6;
            this.spriteView.UseCompatibleStateImageBehavior = false;
            // 
            // spriteEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(340, 254);
            this.Controls.Add(this.spriteView);
            this.Controls.Add(this.colorBox);
            this.Controls.Add(this.sizeBox);
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.clearButton);
            this.Controls.Add(this.pixelBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.HelpButton = true;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "spriteEditor";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Sprite Editor";
            ((System.ComponentModel.ISupportInitialize)(this.heightInput)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.widthInput)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pixelBox)).EndInit();
            this.sizeBox.ResumeLayout(false);
            this.sizeBox.PerformLayout();
            this.colorBox.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label heightLabel;
        private System.Windows.Forms.Label widthLabel;
        private System.Windows.Forms.Label colorBlack;
        private System.Windows.Forms.Label colorDarkGray;
        private System.Windows.Forms.Label colorLightGray;
        private System.Windows.Forms.Label colorWhite;
        private System.Windows.Forms.NumericUpDown heightInput;
        private System.Windows.Forms.NumericUpDown widthInput;
        private System.Windows.Forms.CheckBox grayscaleCheck;
        private System.Windows.Forms.Button clearButton;
        private System.Windows.Forms.Button okButton;
        public System.Windows.Forms.PictureBox pixelBox;
        private System.Windows.Forms.GroupBox sizeBox;
        private System.Windows.Forms.GroupBox colorBox;
        private System.Windows.Forms.ListView spriteView;

    }
}