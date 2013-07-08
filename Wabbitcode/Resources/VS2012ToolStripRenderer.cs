namespace Revsoft.Wabbitcode.Resource
{
    using System.Windows.Forms;

    internal class VS2012ToolStripRenderer : ToolStripProfessionalRenderer
    {
        public VS2012ToolStripRenderer()
        : base(new VS2012ColorTable())
        {
        }
    }
}