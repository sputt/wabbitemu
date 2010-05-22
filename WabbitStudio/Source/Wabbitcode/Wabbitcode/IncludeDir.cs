using System;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
    public partial class IncludeDir : Form
    {
        public IncludeDir()
        {
            InitializeComponent();
            //includeDirList.Items.Clear();
            string[] directories = ProjectService.IsInternal ? Properties.Settings.Default.includeDir.Split('\n') : ProjectService.IncludeDirs;
            foreach (string dir in directories)
                if (!string.IsNullOrEmpty(dir))
                    includeDirList.Items.Add(dir);
        }

        private void addDirButton_Click(object sender, EventArgs e)
        {
            DialogResult include = includeDirBrowser.ShowDialog();
            if (include == DialogResult.OK && !includeDirList.Items.Contains(includeDirBrowser.SelectedPath))
                includeDirList.Items.Add(includeDirBrowser.SelectedPath);
        }

        private void deleteDirButton_Click(object sender, EventArgs e)
        {
            object index = includeDirList.SelectedItem;
            if (index != null)
                includeDirList.Items.Remove(index);
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            if (ProjectService.IsInternal)
                Properties.Settings.Default.includeDir = "";
            string temp = "";
            foreach (string includeDir in includeDirList.Items)
                temp += includeDir + '\n';
            if (ProjectService.IsInternal)
                Properties.Settings.Default.includeDir = temp;
            else
                ProjectService.IncludeDirs = temp.Split('\n');
        }
    }
}
