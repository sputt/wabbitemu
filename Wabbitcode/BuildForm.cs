using System;
using System.Drawing;
using System.Windows.Forms;
using System.Xml;
using Revsoft.Wabbitcode.Classes;

namespace Revsoft.Wabbitcode
{
    public partial class BuildForm : Form
    {
        //FIXME: make this directly access the list
        string projectFile;
        public XmlElement buildStep;
        XmlDocument doc = new XmlDocument();
        public BuildForm(string projectFile)
        {
            InitializeComponent();
            actionBox_SelectedIndexChanged(null, null);
            this.projectFile = projectFile;
            buildStep = doc.CreateElement("Step");
            actionBox.DropDownStyle = ComboBoxStyle.DropDownList;
            outputBox.DropDownStyle = ComboBoxStyle.DropDownList;
        }

        private void actionBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (actionBox.SelectedIndex)
            {
                case 0:
                    commandLabel.Visible = false;
                    commandBox.Visible = false;
                    outputLabel.Visible = true;
                    fileLabel.Visible = true;
                    fileLabel.Location = new Point(58, 62);
                    browseButton.Visible = true;
                    browseButton.Location = new Point(125, 57);
                    outputBox.Visible = true;
                    fileLabel.Visible = true;
                    fileLocBox.Visible = true;
                    break;
                case 1:
                case 2:
                    commandLabel.Visible = false;
                    commandBox.Visible = false;
                    outputLabel.Visible = false;
                    fileLabel.Visible = true;
                    fileLabel.Location = new Point(52, 42);
                    browseButton.Visible = true;
                    browseButton.Location = new Point(125, 37);
                    outputBox.Visible = false;
                    fileLabel.Visible = true;
                    fileLocBox.Visible = true;
                    break;
                case 3:
                    commandLabel.Visible = true;
                    commandBox.Visible = true;
                    outputLabel.Visible = false;
                    fileLabel.Visible = false;
                    browseButton.Visible = false;
                    outputBox.Visible = false;
                    fileLabel.Visible = false;
                    fileLocBox.Visible = false;
                    break;
            }
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            //fileContents = ((Wabbitcode.BuildSeq)(Parent)).fileContents;
            XmlAttribute stepType = doc.CreateAttribute("action");
            XmlAttribute outputType = doc.CreateAttribute("type");
            switch (actionBox.SelectedIndex)
            {
                case 0:
                    stepType.Value = "Compile";
                    outputType.Value = outputBox.SelectedIndex.ToString();
                    buildStep.InnerText = fileLocBox.Text;
                    buildStep.Attributes.Append(outputType);
                    break;
                case 1:
                    stepType.Value = "Table";
                    buildStep.InnerText = fileLocBox.Text;
                    break;
                case 2:
                    stepType.Value = "Listing";
                    buildStep.InnerText = fileLocBox.Text;
                    break;
                case 3:
                    stepType.Value = "External";
                    buildStep.InnerText = commandBox.Text;
                    break;
            }
            buildStep.Attributes.Append(stepType);
            Close();
        }

        private bool browsing;
        private void browseButton_Click(object sender, EventArgs e)
        {
			OpenFileDialog openFileDialog = new OpenFileDialog()
			{
				Filter = "All Know File Types | *.asm; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
						   " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
				FilterIndex = 0,
				RestoreDirectory = true,
				Title = "Open File",
			};
            browsing = true;
           	openFileDialog.InitialDirectory = projectFile.Substring(0, projectFile.LastIndexOf('\\'));
            if (openFileDialog.ShowDialog() == DialogResult.OK)
                fileLocBox.Text = openFileDialog.FileName;
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void BuildForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!browsing) 
                return;
            browsing = false;
            e.Cancel = true;
        }
    }
}
