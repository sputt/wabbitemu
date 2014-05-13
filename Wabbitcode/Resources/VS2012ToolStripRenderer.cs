using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Resources
{
    internal class VS2012ToolStripRenderer : ToolStripProfessionalRenderer
    {
        public VS2012ToolStripRenderer()
            : base(new VS2012ColorTable())
        {
        }
    }
}