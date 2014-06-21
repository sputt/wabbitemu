using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Services
{
    public sealed class WabbitcodeDockPanel : DockPanel
    {
        public WabbitcodeDockPanel()
        {
            SuspendLayout();

            Dock = DockStyle.Fill;
            DockBackColor = System.Drawing.SystemColors.ControlDark;
            DockBottomPortion = 200D;
            DockLeftPortion = 200D;
            DockRightPortion = 200D;
            DockTopPortion = 150D;
            DocumentStyle = DocumentStyle.DockingWindow;
            Location = new System.Drawing.Point(0, 0);
            Name = "dockPanel";
            RightToLeftLayout = true;
            Size = new System.Drawing.Size(990, 511);

            ResumeLayout(false);
        }
    }
}