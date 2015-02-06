using System;
using System.Drawing;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    public partial class VariableFullViewer : Form
    {
        public VariableFullViewer(string variableName, object actualValue)
        {
            if (actualValue == null)
            {
                throw new ArgumentNullException("actualValue");
            }

            InitializeComponent();
            variableNameBox.Text = variableName;

            var actualImage = actualValue as Image;
            if (actualImage != null)
            {
                pictureBox.Image = actualImage;
                pictureBox.Size = actualImage.Size;
                pictureBox.BackColor = Color.White;
                displayBox.Visible = false;
                SizeGripStyle = SizeGripStyle.Hide;
                FormBorderStyle = FormBorderStyle.FixedDialog;
            }
            else
            {
                pictureBox.Visible = false;
                displayBox.Text = actualValue.ToString();
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}