using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.GUI
{
    public sealed class WabbitcodeToolStripContainer : ToolStripContainer
    {
        public WabbitcodeToolStripContainer(StatusStrip statusBar, DockPanel dockPanel)
        {
            BottomToolStripPanel.SuspendLayout();
            ContentPanel.SuspendLayout();
            SuspendLayout();

            BottomToolStripPanel.Controls.Add(statusBar);
            ContentPanel.Controls.Add(dockPanel);
            ContentPanel.Size = new System.Drawing.Size(990, 511);
            Dock = System.Windows.Forms.DockStyle.Fill;
            LeftToolStripPanelVisible = false;
            Location = new System.Drawing.Point(0, 0);
            Name = "toolStripContainer";
            RightToolStripPanelVisible = false;
            Size = new System.Drawing.Size(990, 558);
            TabIndex = 9;
            Text = "toolStripContainer1";

            BottomToolStripPanel.ResumeLayout(false);
            ContentPanel.ResumeLayout(false);
            ResumeLayout(false);
        }
    }
}