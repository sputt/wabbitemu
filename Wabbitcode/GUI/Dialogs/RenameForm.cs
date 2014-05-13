using System.Windows.Forms;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    public partial class RenameForm : Form
    {
        public RenameForm()
        {
            InitializeComponent();
        }

        public string NewText
        {
            get { return textBox.Text; }
        }
    }
}