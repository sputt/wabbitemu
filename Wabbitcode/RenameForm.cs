namespace Revsoft.Wabbitcode
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Data;
    using System.Drawing;
    using System.Text;
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