using System.Windows.Forms;
using System.Windows.Forms.Integration;

namespace MapEditorPlugin
{
    partial class TilesPanel
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        private ElementHost elementHost;
        private WPFZ80MapEditor.TilesPanel tilesPanel;

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
            tilesPanel = new WPFZ80MapEditor.TilesPanel();

            this.elementHost = new ElementHost();
            this.elementHost.Dock = DockStyle.Fill;
            elementHost.Child = tilesPanel;

            this.components = new System.ComponentModel.Container();
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockLeft;
            this.Text = "Tiles Panel";
            this.TabText = "Tiles Panel";
            this.Controls.Add(elementHost);
        }

        #endregion
    }
}