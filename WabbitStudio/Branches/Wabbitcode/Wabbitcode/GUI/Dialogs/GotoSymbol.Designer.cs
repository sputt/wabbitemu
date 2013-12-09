namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    partial class GotoSymbol
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
			this.inputBox = new System.Windows.Forms.ComboBox();
			this.symbolLabel = new System.Windows.Forms.Label();
			this.okButton = new System.Windows.Forms.Button();
			this.cancelButton = new System.Windows.Forms.Button();
			this.errorProvider = new System.Windows.Forms.ErrorProvider(this.components);
			((System.ComponentModel.ISupportInitialize)(this.errorProvider)).BeginInit();
			this.SuspendLayout();
			// 
			// inputBox
			// 
			this.inputBox.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
			this.inputBox.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
			this.inputBox.Location = new System.Drawing.Point(62, 12);
			this.inputBox.Name = "inputBox";
			this.inputBox.Size = new System.Drawing.Size(209, 21);
			this.inputBox.TabIndex = 0;
			this.inputBox.TextChanged += new System.EventHandler(this.inputBox_TextChanged);
			this.inputBox.Validating += new System.ComponentModel.CancelEventHandler(this.inputBox_Validating);
			// 
			// symbolLabel
			// 
			this.symbolLabel.AutoSize = true;
			this.symbolLabel.Location = new System.Drawing.Point(12, 15);
			this.symbolLabel.Name = "symbolLabel";
			this.symbolLabel.Size = new System.Drawing.Size(44, 13);
			this.symbolLabel.TabIndex = 1;
			this.symbolLabel.Text = "Symbol:";
			// 
			// okButton
			// 
			this.okButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
			this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.okButton.Location = new System.Drawing.Point(115, 39);
			this.okButton.Name = "okButton";
			this.okButton.Size = new System.Drawing.Size(75, 23);
			this.okButton.TabIndex = 1;
			this.okButton.Text = "OK";
			this.okButton.Click += new System.EventHandler(this.okButton_Click);
			// 
			// cancelButton
			// 
			this.cancelButton.AccessibleRole = System.Windows.Forms.AccessibleRole.PushButton;
			this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelButton.Location = new System.Drawing.Point(196, 39);
			this.cancelButton.Name = "cancelButton";
			this.cancelButton.Size = new System.Drawing.Size(75, 23);
			this.cancelButton.TabIndex = 2;
			this.cancelButton.Text = "Cancel";
			// 
			// errorProvider
			// 
			this.errorProvider.BlinkStyle = System.Windows.Forms.ErrorBlinkStyle.NeverBlink;
			this.errorProvider.ContainerControl = this;
			// 
			// GotoSymbol
			// 
			this.AcceptButton = this.okButton;
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.cancelButton;
			this.ClientSize = new System.Drawing.Size(292, 71);
			this.Controls.Add(this.cancelButton);
			this.Controls.Add(this.okButton);
			this.Controls.Add(this.symbolLabel);
			this.Controls.Add(this.inputBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "GotoSymbol";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Goto Symbol";
			((System.ComponentModel.ISupportInitialize)(this.errorProvider)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label symbolLabel;
        private System.Windows.Forms.Button okButton;
        private System.Windows.Forms.Button cancelButton;
        public System.Windows.Forms.ComboBox inputBox;
		private System.Windows.Forms.ErrorProvider errorProvider;
    }
}