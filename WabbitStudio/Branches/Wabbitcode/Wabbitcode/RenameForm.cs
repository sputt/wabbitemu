namespace Revsoft.Wabbitcode
{
    using System.Windows.Forms;

    public partial class RenameForm : Form
    {
        public RenameForm()
        {
            InitializeComponent();
        }

        public string NewText
        {
            get
            {
                return textBox.Text;
            }
        }
    }
}