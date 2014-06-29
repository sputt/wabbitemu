using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.ToolBars;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.GUI
{
	partial class MainForm
	{
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            // 
            // MainForm
            // 
            SuspendLayout();
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(990, 558);
            this.Controls.Add(_toolStripContainer);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.Name = "MainForm";
            this.Text = "Wabbitcode";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainFormRedone_FormClosing);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.MainFormRedone_DragDrop);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.MainFormRedone_DragEnter);
            this.ResumeLayout(false);

		}

	    private ToolStripContainer _toolStripContainer;

	    #endregion
	}
}