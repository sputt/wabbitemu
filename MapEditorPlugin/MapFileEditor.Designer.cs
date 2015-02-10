using System.Windows.Forms;

namespace MapEditorPlugin
{
    partial class MapFileEditor
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private WPFZ80MapEditor.MapEditorControl _editor;
        private System.Windows.Forms.Integration.ElementHost wpfHost;

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
            _editor = new WPFZ80MapEditor.MapEditorControl();
            wpfHost = new System.Windows.Forms.Integration.ElementHost();

            //
            // wpfHost
            //
            this.wpfHost.Child = _editor;
            this.wpfHost.Dock = DockStyle.Fill;
            

            this.components = new System.ComponentModel.Container();
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(wpfHost);
            this.Text = "MapFileEditor";
        }

        #endregion
    }
}