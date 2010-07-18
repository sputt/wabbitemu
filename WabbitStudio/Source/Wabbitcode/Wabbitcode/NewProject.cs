using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Revsoft.Wabbitcode.Classes;
using System.Collections;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode
{
    public partial class NewProjectDialog
    {
        public string projectFile = "";
        public NewProjectDialog()
        {
            InitializeComponent();
            if (!Directory.Exists(Path.Combine(Application.UserAppDataPath, "Templates")))
                CreateTemplates();
            getTemplates();
            calcOptions.SelectedIndex = 0;
            GetTemplatesInDir();
        }

        readonly string templatesdir = Path.Combine(Application.UserAppDataPath, "Templates");
        private void getTemplates()
        {
            string[] dirs = Directory.GetDirectories(templatesdir);
            foreach (string dir in dirs)
                calcOptions.Items.Add(Path.GetFileName(dir));
        }

        private void GetTemplatesInDir()
        {
            string[] templates = Directory.GetDirectories(Path.Combine(templatesdir, calcOptions.Items[calcOptions.SelectedIndex].ToString()));
            foreach (string template in templates)
                listView1.Items.Add(Path.GetFileName(template));
        }

        private void CreateTemplates()
        {
            Directory.CreateDirectory(templatesdir);
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-73"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-82"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-83"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-83 Plus"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-85"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-86"));

            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "App"));
            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "Mirage OS"));
            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "Ion"));
            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "No Stub"));
        }

        private void browseLoc_Click(object sender, EventArgs e)
        {
            DialogResult browseFile = folderBrowserDialog.ShowDialog();
            if (browseFile == DialogResult.OK)
                locTextBox.Text = folderBrowserDialog.SelectedPath;
        }

        string filePath;
        private void okTemplate_Click(object sender, EventArgs e)
        {
			string projectName = nameTextBox.Text;
            if (projectName != "")
            {				
                string extensionName = "0";
                filePath = locTextBox.Text;
                extensions = fileTypesBox.Text;
                if (!Directory.Exists(filePath) && !projFromDirBox.Checked)
                    Directory.CreateDirectory(filePath);
                projectFile = Path.Combine(filePath, projectName + ".wcodeproj");
				ProjectService.CreateNewProject(projectFile, projectName);
				ProjectFolder currentFolder = ProjectService.MainFolder;
				GetFiles(filePath, ref currentFolder);
				//ProjectService.MainFolder = currentFolder;
                //project = new NewProject(Path.Combine(filePath, nameTextBox.Text + ".wcodeproj"));
                //ProjectService.IncludeDirs = includeDirs;

                //ProjectService.DeleteFolder("", "Folderz4L33T\\");
                ProjectService.SaveProject();
            }
            else
            {
                MessageBox.Show("No project name entered!");
                cancelQuit = true;
            }
        }
        bool cancelQuit;

        private string extensions;
        private void GetFiles(string directory, ref ProjectFolder currentFolder)
        {
            foreach (string dir in Directory.GetDirectories(directory))
            {
				ProjectService.IncludeDirs.Add(dir);
				string dirName = Path.GetFileName(dir);
				ProjectFolder folderAdded = ProjectService.AddFolder(dirName, currentFolder);
                GetFiles(dir, ref folderAdded);
            }
            string[] files = Directory.GetFiles(directory);
            foreach (string file in files)
            {
                if (!extensions.Contains(Path.GetExtension(file)))
                    continue;
				ProjectService.AddFile(currentFolder, file);
            }
        }

        private void templateForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!cancelQuit) return;
            e.Cancel = true;
            cancelQuit = false;
        }

        private void calcOptions_SelectedIndexChanged(object sender, EventArgs e)
        {
            listView1.Items.Clear();
            GetTemplatesInDir();
        }

        private void projFromDirBox_CheckedChanged(object sender, EventArgs e)
        {
            fileTypesBox.Visible = projFromDirBox.Checked;
            fileTypesLabel.Visible = projFromDirBox.Checked;
        }
    }
}
